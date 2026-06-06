#include "GPURenderer.hpp"

#include <SDL3_image/SDL_image.h>

#include <cstddef>
#include <iostream>

namespace gpustudy {

GPURenderer::GPURenderer()
    : m_device(nullptr),
      m_pipeline(nullptr),
      m_offscreenTarget(nullptr),
      m_readbackBuffer(nullptr),
      m_width(0),
      m_height(0),
      m_format(SDL_GPU_TEXTUREFORMAT_INVALID) {
}

GPURenderer::~GPURenderer() {
    shutdown();
}

bool GPURenderer::init(const char* gpuDriverHint) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Erreur SDL_Init : " << SDL_GetError() << "\n";
        return false;
    }

    SDL_SetHint(SDL_HINT_GPU_DRIVER, gpuDriverHint);

    m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!m_device) {
        std::cerr << "Erreur GPUDevice : " << SDL_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    return true;
}

bool GPURenderer::createOffscreenTarget(Uint32 width,
                                        Uint32 height,
                                        SDL_GPUTextureFormat format) {
    if (!m_device) {
        std::cerr << "Erreur: init() doit etre appele avant createOffscreenTarget().\n";
        return false;
    }

    releaseTargetResources();

    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = format;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureInfo.props = 0;

    m_offscreenTarget = SDL_CreateGPUTexture(m_device, &textureInfo);
    if (!m_offscreenTarget) {
        std::cerr << "Erreur creation texture offscreen : " << SDL_GetError() << "\n";
        return false;
    }

    m_width = width;
    m_height = height;
    m_format = format;

    return createReadbackBuffer();
}

SDL_Window* GPURenderer::createWindow(const char* windowTitle, int windowWidth, int windowHeight) const {
    SDL_Window* window = SDL_CreateWindow(windowTitle, windowWidth, windowHeight, 0);
    if (!window) {
        std::cerr << "Erreur creation window : " << SDL_GetError() << "\n";
    }
    return window;
}

bool GPURenderer::claimWindow(SDL_Window* window) const {
    if (!window) {
        return false;
    }

    if (!SDL_ClaimWindowForGPUDevice(m_device, window)) {
        std::cerr << "Erreur claim window : " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

SDL_GPUGraphicsPipeline* GPURenderer::createTrianglePipeline(SDL_Window* window,
                                                             const std::string& vertexSpirvPath,
                                                             const std::string& fragmentSpirvPath) const {
    if (!m_device || !window) {
        std::cerr << "Erreur: device ou window manquant pour createTrianglePipeline().\n";
        return nullptr;
    }

    SDL_GPUShader* vertexShader = loadShader(vertexSpirvPath, SDL_GPU_SHADERSTAGE_VERTEX, 0);
    SDL_GPUShader* fragmentShader = loadShader(fragmentSpirvPath, SDL_GPU_SHADERSTAGE_FRAGMENT, 0);
    if (!vertexShader || !fragmentShader) {
        if (vertexShader) SDL_ReleaseGPUShader(m_device, vertexShader);
        if (fragmentShader) SDL_ReleaseGPUShader(m_device, fragmentShader);
        return nullptr;
    }

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 0;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 0;

    SDL_GPUColorTargetDescription colorTargetDesc{};
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(m_device, window);
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);
    SDL_ReleaseGPUShader(m_device, vertexShader);
    SDL_ReleaseGPUShader(m_device, fragmentShader);

    if (!pipeline) {
        std::cerr << "Erreur creation pipeline triangle : " << SDL_GetError() << "\n";
    }

    return pipeline;
}

SDL_GPUGraphicsPipeline* GPURenderer::createPositionColorPipeline(SDL_Window* window,
                                                                  const std::string& vertexSpirvPath,
                                                                  const std::string& fragmentSpirvPath,
                                                                  Uint32 stride) const {
    if (!m_device || !window) {
        std::cerr << "Erreur: device ou window manquant pour createPositionColorPipeline().\n";
        return nullptr;
    }

    SDL_GPUShader* vertexShader = loadShader(vertexSpirvPath, SDL_GPU_SHADERSTAGE_VERTEX, 0);
    SDL_GPUShader* fragmentShader = loadShader(fragmentSpirvPath, SDL_GPU_SHADERSTAGE_FRAGMENT, 0);
    if (!vertexShader || !fragmentShader) {
        if (vertexShader) SDL_ReleaseGPUShader(m_device, vertexShader);
        if (fragmentShader) SDL_ReleaseGPUShader(m_device, fragmentShader);
        return nullptr;
    }

    SDL_GPUVertexBufferDescription bufferDesc{};
    bufferDesc.slot = 0;
    bufferDesc.pitch = stride;
    bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

    SDL_GPUVertexAttribute attributes[2]{};
    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = 0;
    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    attributes[1].offset = sizeof(float) * 2;

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDesc;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;

    SDL_GPUColorTargetDescription colorTargetDesc{};
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(m_device, window);
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);
    SDL_ReleaseGPUShader(m_device, vertexShader);
    SDL_ReleaseGPUShader(m_device, fragmentShader);

    if (!pipeline) {
        std::cerr << "Erreur creation pipeline position+couleur : " << SDL_GetError() << "\n";
    }

    return pipeline;
}

SDL_GPUGraphicsPipeline* GPURenderer::createTexturedQuadPipeline(SDL_Window* window,
                                                                 const std::string& vertexSpirvPath,
                                                                 const std::string& fragmentSpirvPath) const {
    if (!m_device || !window) {
        std::cerr << "Erreur: device ou window manquant pour createTexturedQuadPipeline().\n";
        return nullptr;
    }

    SDL_GPUShader* vertexShader = loadShader(vertexSpirvPath, SDL_GPU_SHADERSTAGE_VERTEX, 0);
    SDL_GPUShader* fragmentShader = loadShader(fragmentSpirvPath, SDL_GPU_SHADERSTAGE_FRAGMENT, 1);
    if (!vertexShader || !fragmentShader) {
        if (vertexShader) SDL_ReleaseGPUShader(m_device, vertexShader);
        if (fragmentShader) SDL_ReleaseGPUShader(m_device, fragmentShader);
        return nullptr;
    }

    SDL_GPUVertexBufferDescription bufferDesc{};
    bufferDesc.slot = 0;
    bufferDesc.pitch = sizeof(float) * 4;
    bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

    SDL_GPUVertexAttribute attributes[2]{};
    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = 0;
    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[1].offset = sizeof(float) * 2;

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDesc;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;

    SDL_GPUColorTargetDescription colorTargetDesc{};
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(m_device, window);
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);
    SDL_ReleaseGPUShader(m_device, vertexShader);
    SDL_ReleaseGPUShader(m_device, fragmentShader);

    if (!pipeline) {
        std::cerr << "Erreur creation pipeline texture : " << SDL_GetError() << "\n";
    }

    return pipeline;
}

bool GPURenderer::renderTriangleFrame(SDL_Window* window,
                                      SDL_GPUGraphicsPipeline* pipeline,
                                      const SDL_FColor& clearColor) const {
    if (!m_device || !window || !pipeline) {
        return false;
    }

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmdBuffer) {
        std::cerr << "Erreur acquisition command buffer : " << SDL_GetError() << "\n";
        return false;
    }

    SDL_GPUTexture* swapchainTexture = nullptr;
    if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, window, &swapchainTexture, nullptr, nullptr)) {
        SDL_SubmitGPUCommandBuffer(cmdBuffer);
        return false;
    }

    if (swapchainTexture) {
        SDL_GPUColorTargetInfo colorTarget{};
        colorTarget.texture = swapchainTexture;
        colorTarget.clear_color = clearColor;
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
        SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
        SDL_EndGPURenderPass(renderPass);
    }

    return SDL_SubmitGPUCommandBuffer(cmdBuffer);
}

bool GPURenderer::uploadBufferData(SDL_GPUBuffer* buffer,
                                   const void* data,
                                   Uint32 size) const {
    if (!m_device || !buffer || !data || size == 0) {
        return false;
    }

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = size;
    transferInfo.props = 0;

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferInfo);
    if (!transferBuffer) {
        std::cerr << "Erreur creation transfer buffer upload : " << SDL_GetError() << "\n";
        return false;
    }

    void* mapped = SDL_MapGPUTransferBuffer(m_device, transferBuffer, true);
    if (!mapped) {
        std::cerr << "Erreur map transfer buffer upload : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        return false;
    }

    SDL_memcpy(mapped, data, size);
    SDL_UnmapGPUTransferBuffer(m_device, transferBuffer);

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmdBuffer) {
        std::cerr << "Erreur acquisition command buffer : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        return false;
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);
    SDL_GPUTransferBufferLocation source{};
    source.transfer_buffer = transferBuffer;
    source.offset = 0;

    SDL_GPUBufferRegion destination{};
    destination.buffer = buffer;
    destination.offset = 0;
    destination.size = size;

    SDL_UploadToGPUBuffer(copyPass, &source, &destination, false);
    SDL_EndGPUCopyPass(copyPass);

    SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmdBuffer);
    if (!fence) {
        std::cerr << "Erreur submit upload buffer : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        return false;
    }

    SDL_GPUFence* fences[1] = { fence };
    const bool waited = SDL_WaitForGPUFences(m_device, true, fences, 1);
    SDL_ReleaseGPUFence(m_device, fence);

    SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
    return waited;
}

SDL_GPUBuffer* GPURenderer::createVertexBuffer(const void* vertices, Uint32 size) const {
    if (!m_device) {
        return nullptr;
    }

    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bufferInfo.size = size;
    bufferInfo.props = 0;

    SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(m_device, &bufferInfo);
    if (!vertexBuffer) {
        std::cerr << "Erreur creation vertex buffer : " << SDL_GetError() << "\n";
        return nullptr;
    }

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = size;
    transferInfo.props = 0;

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferInfo);
    if (!transferBuffer) {
        std::cerr << "Erreur creation transfer buffer vertex : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
        return nullptr;
    }

    void* mapped = SDL_MapGPUTransferBuffer(m_device, transferBuffer, true);
    if (!mapped) {
        std::cerr << "Erreur map vertex transfer buffer : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
        return nullptr;
    }

    SDL_memcpy(mapped, vertices, size);
    SDL_UnmapGPUTransferBuffer(m_device, transferBuffer);

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmdBuffer) {
        std::cerr << "Erreur acquisition command buffer : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
        return nullptr;
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);
    SDL_GPUTransferBufferLocation source{};
    source.transfer_buffer = transferBuffer;
    source.offset = 0;

    SDL_GPUBufferRegion destination{};
    destination.buffer = vertexBuffer;
    destination.offset = 0;
    destination.size = size;

    SDL_UploadToGPUBuffer(copyPass, &source, &destination, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmdBuffer);

    SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
    return vertexBuffer;
}

SDL_GPUBuffer* GPURenderer::createQuadVertexBuffer(const void* vertices, Uint32 size) const {
    return createVertexBuffer(vertices, size);
}

SDL_GPUTexture* GPURenderer::createTextureFromImage(const std::string& imagePath,
                                                    SDL_GPUTransferBuffer** transferBufferOut,
                                                    Uint32* widthOut,
                                                    Uint32* heightOut) const {
    if (!m_device) {
        return nullptr;
    }

    SDL_Surface* loadedSurface = IMG_Load(imagePath.c_str());
    if (!loadedSurface) {
        std::cerr << "Erreur chargement image " << imagePath << " : " << SDL_GetError() << "\n";
        return nullptr;
    }

    SDL_Surface* imageSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(loadedSurface);
    if (!imageSurface) {
        std::cerr << "Erreur conversion image RGBA32 : " << SDL_GetError() << "\n";
        return nullptr;
    }

    const Uint32 imageWidth = static_cast<Uint32>(imageSurface->w);
    const Uint32 imageHeight = static_cast<Uint32>(imageSurface->h);

    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = imageWidth;
    textureInfo.height = imageHeight;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureInfo.props = 0;

    SDL_GPUTexture* texture = SDL_CreateGPUTexture(m_device, &textureInfo);
    if (!texture) {
        std::cerr << "Erreur creation texture : " << SDL_GetError() << "\n";
        SDL_DestroySurface(imageSurface);
        return nullptr;
    }

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = imageWidth * imageHeight * 4;
    transferInfo.props = 0;
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferInfo);
    if (!transferBuffer) {
        std::cerr << "Erreur creation transfer buffer texture : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTexture(m_device, texture);
        SDL_DestroySurface(imageSurface);
        return nullptr;
    }

    void* mapped = SDL_MapGPUTransferBuffer(m_device, transferBuffer, true);
    if (!mapped) {
        std::cerr << "Erreur map texture transfer buffer : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        SDL_ReleaseGPUTexture(m_device, texture);
        SDL_DestroySurface(imageSurface);
        return nullptr;
    }

    const Uint8* srcPixels = static_cast<const Uint8*>(imageSurface->pixels);
    Uint8* dstPixels = static_cast<Uint8*>(mapped);
    const int srcPitch = imageSurface->pitch;
    const int dstPitch = static_cast<int>(imageWidth * 4);
    for (Uint32 y = 0; y < imageHeight; ++y) {
        SDL_memcpy(dstPixels + (y * dstPitch), srcPixels + (y * srcPitch), static_cast<size_t>(dstPitch));
    }
    SDL_UnmapGPUTransferBuffer(m_device, transferBuffer);

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmdBuffer) {
        std::cerr << "Erreur acquisition command buffer : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        SDL_ReleaseGPUTexture(m_device, texture);
        SDL_DestroySurface(imageSurface);
        return nullptr;
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);
    SDL_GPUTextureTransferInfo source{};
    source.transfer_buffer = transferBuffer;
    source.offset = 0;
    source.pixels_per_row = imageWidth;
    source.rows_per_layer = imageHeight;

    SDL_GPUTextureRegion destination{};
    destination.texture = texture;
    destination.mip_level = 0;
    destination.layer = 0;
    destination.x = 0;
    destination.y = 0;
    destination.z = 0;
    destination.w = imageWidth;
    destination.h = imageHeight;
    destination.d = 1;

    SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmdBuffer);

    SDL_DestroySurface(imageSurface);

    if (transferBufferOut) {
        *transferBufferOut = transferBuffer;
    } else {
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
    }

    if (widthOut) *widthOut = imageWidth;
    if (heightOut) *heightOut = imageHeight;

    return texture;
}

SDL_GPUSampler* GPURenderer::createLinearClampSampler() const {
    if (!m_device) {
        return nullptr;
    }

    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.mip_lod_bias = 0.0f;
    samplerInfo.max_anisotropy = 1.0f;
    samplerInfo.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
    samplerInfo.min_lod = 0.0f;
    samplerInfo.max_lod = 0.0f;
    samplerInfo.enable_anisotropy = false;
    samplerInfo.enable_compare = false;
    samplerInfo.props = 0;

    SDL_GPUSampler* sampler = SDL_CreateGPUSampler(m_device, &samplerInfo);
    if (!sampler) {
        std::cerr << "Erreur creation sampler : " << SDL_GetError() << "\n";
    }

    return sampler;
}

bool GPURenderer::renderTexturedQuadFrame(SDL_Window* window,
                                          SDL_GPUGraphicsPipeline* pipeline,
                                          SDL_GPUBuffer* vertexBuffer,
                                          SDL_GPUTexture* texture,
                                          SDL_GPUSampler* sampler,
                                          const SDL_FColor& clearColor) const {
    if (!m_device || !window || !pipeline || !vertexBuffer || !texture || !sampler) {
        return false;
    }

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmdBuffer) {
        std::cerr << "Erreur acquisition command buffer : " << SDL_GetError() << "\n";
        return false;
    }

    SDL_GPUTexture* swapchainTexture = nullptr;
    if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, window, &swapchainTexture, nullptr, nullptr)) {
        SDL_SubmitGPUCommandBuffer(cmdBuffer);
        return false;
    }

    if (swapchainTexture) {
        SDL_GPUColorTargetInfo colorTarget{};
        colorTarget.texture = swapchainTexture;
        colorTarget.clear_color = clearColor;
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
        SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

        SDL_GPUBufferBinding vertexBinding{};
        vertexBinding.buffer = vertexBuffer;
        vertexBinding.offset = 0;
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);

        SDL_GPUTextureSamplerBinding fragmentSamplerBinding{};
        fragmentSamplerBinding.texture = texture;
        fragmentSamplerBinding.sampler = sampler;
        SDL_BindGPUFragmentSamplers(renderPass, 0, &fragmentSamplerBinding, 1);

        SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);
        SDL_EndGPURenderPass(renderPass);
    }

    return SDL_SubmitGPUCommandBuffer(cmdBuffer);
}

bool GPURenderer::renderVertexBufferFrame(SDL_Window* window,
                                          SDL_GPUGraphicsPipeline* pipeline,
                                          SDL_GPUBuffer* vertexBuffer,
                                          Uint32 vertexCount,
                                          const SDL_FColor& clearColor) const {
    if (!m_device || !window || !pipeline || !vertexBuffer) {
        return false;
    }

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmdBuffer) {
        std::cerr << "Erreur acquisition command buffer : " << SDL_GetError() << "\n";
        return false;
    }

    SDL_GPUTexture* swapchainTexture = nullptr;
    if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, window, &swapchainTexture, nullptr, nullptr)) {
        SDL_SubmitGPUCommandBuffer(cmdBuffer);
        return false;
    }

    if (swapchainTexture) {
        SDL_GPUColorTargetInfo colorTarget{};
        colorTarget.texture = swapchainTexture;
        colorTarget.clear_color = clearColor;
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
        SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

        SDL_GPUBufferBinding vertexBinding{};
        vertexBinding.buffer = vertexBuffer;
        vertexBinding.offset = 0;
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);

        SDL_DrawGPUPrimitives(renderPass, vertexCount, 1, 0, 0);
        SDL_EndGPURenderPass(renderPass);
    }

    return SDL_SubmitGPUCommandBuffer(cmdBuffer);
}

bool GPURenderer::createSimplePipeline(const std::string& vertexSpirvPath,
                                       const std::string& fragmentSpirvPath) {
    if (!m_device || !m_offscreenTarget) {
        std::cerr << "Erreur: init() et createOffscreenTarget() doivent etre appeles avant createSimplePipeline().\n";
        return false;
    }

    releasePipeline();

    SDL_GPUShader* vertexShader = loadShader(vertexSpirvPath, SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader* fragmentShader = loadShader(fragmentSpirvPath, SDL_GPU_SHADERSTAGE_FRAGMENT);

    if (!vertexShader || !fragmentShader) {
        if (vertexShader) SDL_ReleaseGPUShader(m_device, vertexShader);
        if (fragmentShader) SDL_ReleaseGPUShader(m_device, fragmentShader);
        return false;
    }

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    SDL_GPUColorTargetDescription colorTargetDesc{};
    colorTargetDesc.format = m_format;

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

    pipelineInfo.vertex_input_state.num_vertex_buffers = 0;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 0;

    m_pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);

    SDL_ReleaseGPUShader(m_device, vertexShader);
    SDL_ReleaseGPUShader(m_device, fragmentShader);

    if (!m_pipeline) {
        std::cerr << "Erreur creation pipeline graphique : " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

bool GPURenderer::renderOffscreenTriangleFrame(const SDL_FColor& clearColor) const {
    if (!m_device || !m_pipeline || !m_offscreenTarget || !m_readbackBuffer) {
        std::cerr << "Erreur: ressources non initialisees pour renderOffscreenTriangleFrame().\n";
        return false;
    }

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmdBuffer) {
        std::cerr << "Erreur acquisition command buffer : " << SDL_GetError() << "\n";
        return false;
    }

    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = m_offscreenTarget;
    colorTarget.clear_color = clearColor;
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_pipeline);
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(renderPass);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);

    SDL_GPUTextureRegion sourceRegion{};
    sourceRegion.texture = m_offscreenTarget;
    sourceRegion.mip_level = 0;
    sourceRegion.layer = 0;
    sourceRegion.x = 0;
    sourceRegion.y = 0;
    sourceRegion.z = 0;
    sourceRegion.w = m_width;
    sourceRegion.h = m_height;
    sourceRegion.d = 1;

    SDL_GPUTextureTransferInfo destinationInfo{};
    destinationInfo.transfer_buffer = m_readbackBuffer;
    destinationInfo.offset = 0;
    destinationInfo.pixels_per_row = m_width;
    destinationInfo.rows_per_layer = m_height;

    SDL_DownloadFromGPUTexture(copyPass, &sourceRegion, &destinationInfo);
    SDL_EndGPUCopyPass(copyPass);

    SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmdBuffer);
    if (!fence) {
        std::cerr << "Erreur submit avec fence : " << SDL_GetError() << "\n";
        return false;
    }

    SDL_GPUFence* fences[1] = { fence };
    const bool fenceOk = SDL_WaitForGPUFences(m_device, true, fences, 1);
    SDL_ReleaseGPUFence(m_device, fence);

    if (!fenceOk) {
        std::cerr << "Erreur attente fence GPU : " << SDL_GetError() << "\n";
        return false;
    }

    return true;

}

bool GPURenderer::createReadbackBuffer() {
    const Uint32 bytesPerPixel = SDL_GPUTextureFormatTexelBlockSize(m_format);
    if (bytesPerPixel == 0) {
        std::cerr << "Erreur: format de texture invalide pour readback.\n";
        return false;
    }

    SDL_GPUTransferBufferCreateInfo readbackInfo{};
    readbackInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
    readbackInfo.size = m_width * m_height * bytesPerPixel;
    readbackInfo.props = 0;

    m_readbackBuffer = SDL_CreateGPUTransferBuffer(m_device, &readbackInfo);
    if (!m_readbackBuffer) {
        std::cerr << "Erreur creation transfer buffer readback : " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

bool GPURenderer::saveReadbackToBMP(const std::string& outputBmpPath) const {
    void* mappedPixels = SDL_MapGPUTransferBuffer(m_device, m_readbackBuffer, false);
    if (!mappedPixels) {
        std::cerr << "Erreur map transfer buffer (download) : " << SDL_GetError() << "\n";
        return false;
    }

    const Uint32 bytesPerPixel = SDL_GPUTextureFormatTexelBlockSize(m_format);
    const int pitch = static_cast<int>(m_width * bytesPerPixel);

    const SDL_PixelFormat pixelFormat = SDL_GetPixelFormatFromGPUTextureFormat(m_format);
    SDL_Surface* surface = SDL_CreateSurfaceFrom(
        static_cast<int>(m_width),
        static_cast<int>(m_height),
        pixelFormat,
        mappedPixels,
        pitch
    );

    if (!surface) {
        std::cerr << "Erreur creation surface CPU : " << SDL_GetError() << "\n";
        SDL_UnmapGPUTransferBuffer(m_device, m_readbackBuffer);
        return false;
    }

    const bool saved = SDL_SaveBMP(surface, outputBmpPath.c_str());
    if (!saved) {
        std::cerr << "Erreur sauvegarde BMP : " << SDL_GetError() << "\n";
    }

    SDL_DestroySurface(surface);
    SDL_UnmapGPUTransferBuffer(m_device, m_readbackBuffer);

    return saved;
}

bool GPURenderer::saveOffscreenToBMP(const std::string& outputBmpPath) const {
    return saveReadbackToBMP(outputBmpPath);
}

SDL_GPUShader* GPURenderer::loadShader(const std::string& filename,
                                       SDL_GPUShaderStage stage,
                                       Uint32 numSamplers) const {
    size_t codeSize = 0;
    void* code = SDL_LoadFile(filename.c_str(), &codeSize);
    if (!code) {
        std::cerr << "Erreur : impossible de lire le shader : " << filename << "\n";
        return nullptr;
    }

    SDL_GPUShaderCreateInfo shaderInfo{};
    shaderInfo.code_size = codeSize;
    shaderInfo.code = static_cast<const Uint8*>(code);
    shaderInfo.entrypoint = (stage == SDL_GPU_SHADERSTAGE_VERTEX) ? "VS_Main" : "PS_Main";
    shaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shaderInfo.stage = stage;
    shaderInfo.num_samplers = numSamplers;
    shaderInfo.num_uniform_buffers = 0;
    shaderInfo.num_storage_buffers = 0;
    shaderInfo.num_storage_textures = 0;

    SDL_GPUShader* shader = SDL_CreateGPUShader(m_device, &shaderInfo);
    SDL_free(code);

    if (!shader) {
        std::cerr << "Erreur creation shader " << filename << " : " << SDL_GetError() << "\n";
    }

    return shader;
}

void GPURenderer::releasePipeline() {
    if (m_pipeline) {
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_pipeline);
        m_pipeline = nullptr;
    }
}

void GPURenderer::releaseTargetResources() {
    if (m_readbackBuffer) {
        SDL_ReleaseGPUTransferBuffer(m_device, m_readbackBuffer);
        m_readbackBuffer = nullptr;
    }

    if (m_offscreenTarget) {
        SDL_ReleaseGPUTexture(m_device, m_offscreenTarget);
        m_offscreenTarget = nullptr;
    }

    m_width = 0;
    m_height = 0;
    m_format = SDL_GPU_TEXTUREFORMAT_INVALID;
}

void GPURenderer::shutdown() {
    releasePipeline();
    releaseTargetResources();

    if (m_device) {
        SDL_DestroyGPUDevice(m_device);
        m_device = nullptr;
    }

    SDL_Quit();
}

} // namespace gpustudy
