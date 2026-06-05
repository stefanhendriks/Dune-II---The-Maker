#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <cstddef>
#include <iostream>
#include <string>

struct Vertex {
    float x, y;
    float u, v;
};

SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const std::string& filename, SDL_GPUShaderStage stage) {
    size_t size = 0;
    void* code = SDL_LoadFile(filename.c_str(), &size);
    if (!code) {
        std::cerr << "Erreur: impossible de lire le shader " << filename << "\n";
        return nullptr;
    }

    SDL_GPUShaderCreateInfo shaderInfo{};
    shaderInfo.code_size = size;
    shaderInfo.code = static_cast<const Uint8*>(code);
    shaderInfo.entrypoint = (stage == SDL_GPU_SHADERSTAGE_VERTEX) ? "VS_Main" : "PS_Main";
    shaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shaderInfo.stage = stage;
    shaderInfo.num_samplers = (stage == SDL_GPU_SHADERSTAGE_FRAGMENT) ? 1 : 0;
    shaderInfo.num_uniform_buffers = 0;
    shaderInfo.num_storage_buffers = 0;
    shaderInfo.num_storage_textures = 0;

    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
    SDL_free(code);
    return shader;
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Erreur SDL_Init: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL3 GPU - Step 4 Texture", 800, 600, 0);
    if (!window) {
        std::cerr << "Erreur Window: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");

    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device) {
        std::cerr << "Erreur GPUDevice: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        std::cerr << "Erreur Claim Window\n";
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GPUShader* vertexShader = LoadShader(device, "triangle_vs.spirv", SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader* fragmentShader = LoadShader(device, "triangle_ps.spirv", SDL_GPU_SHADERSTAGE_FRAGMENT);
    if (!vertexShader || !fragmentShader) {
        std::cerr << "Erreur au chargement des shaders.\n";
        if (vertexShader) SDL_ReleaseGPUShader(device, vertexShader);
        if (fragmentShader) SDL_ReleaseGPUShader(device, fragmentShader);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Surface* loadedSurface = IMG_Load("../logo_invaders.png");
    if (!loadedSurface) {
        std::cerr << "Erreur chargement image ../logo_invaders.png: " << SDL_GetError() << "\n";
        SDL_ReleaseGPUShader(device, vertexShader);
        SDL_ReleaseGPUShader(device, fragmentShader);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Surface* imageSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(loadedSurface);
    if (!imageSurface) {
        std::cerr << "Erreur conversion image RGBA32: " << SDL_GetError() << "\n";
        SDL_ReleaseGPUShader(device, vertexShader);
        SDL_ReleaseGPUShader(device, fragmentShader);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    const int imageWidth = imageSurface->w;
    const int imageHeight = imageSurface->h;

    int windowWidth = 800;
    int windowHeight = 600;
    SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);

    const int ileft = 300;
    const int iright = 600;
    const int ibottom = 200;
    const int itop = 500;

    const float left = -1.0f + (2.0f * static_cast<float>(ileft) / static_cast<float>(windowWidth));
    const float right = -1.0f + (2.0f * static_cast<float>(iright) / static_cast<float>(windowWidth));
    const float bottom = -1.0f + (2.0f * static_cast<float>(ibottom) / static_cast<float>(windowHeight));
    const float top = -1.0f + (2.0f * static_cast<float>(itop) / static_cast<float>(windowHeight));

    Vertex quadVertices[6] = {
        { left,  bottom, 0.0f, 1.0f },
        { right, bottom, 1.0f, 1.0f },
        { right, top,    1.0f, 0.0f },
        { left,  bottom, 0.0f, 1.0f },
        { right, top,    1.0f, 0.0f },
        { left,  top,    0.0f, 0.0f }
    };

    SDL_GPUVertexBufferDescription bufferDesc{};
    bufferDesc.slot = 0;
    bufferDesc.pitch = sizeof(Vertex);
    bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

    SDL_GPUVertexAttribute attributes[2]{};
    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = offsetof(Vertex, x);

    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[1].offset = offsetof(Vertex, u);

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDesc;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;

    SDL_GPUColorTargetDescription colorTargetDesc{};
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(device, window);
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

    if (!pipeline) {
        std::cerr << "Erreur création pipeline: " << SDL_GetError() << "\n";
        SDL_DestroySurface(imageSurface);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GPUBufferCreateInfo vertexBufferInfo{};
    vertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBufferInfo.size = sizeof(quadVertices);
    vertexBufferInfo.props = 0;
    SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(device, &vertexBufferInfo);
    if (!vertexBuffer) {
        std::cerr << "Erreur création vertex buffer: " << SDL_GetError() << "\n";
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroySurface(imageSurface);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = static_cast<Uint32>(imageWidth);
    textureInfo.height = static_cast<Uint32>(imageHeight);
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureInfo.props = 0;

    SDL_GPUTexture* logoTexture = SDL_CreateGPUTexture(device, &textureInfo);
    if (!logoTexture) {
        std::cerr << "Erreur création texture: " << SDL_GetError() << "\n";
        SDL_ReleaseGPUBuffer(device, vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroySurface(imageSurface);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
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

    SDL_GPUSampler* logoSampler = SDL_CreateGPUSampler(device, &samplerInfo);
    if (!logoSampler) {
        std::cerr << "Erreur création sampler: " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTexture(device, logoTexture);
        SDL_ReleaseGPUBuffer(device, vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroySurface(imageSurface);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GPUTransferBufferCreateInfo vertexTransferInfo{};
    vertexTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    vertexTransferInfo.size = sizeof(quadVertices);
    vertexTransferInfo.props = 0;
    SDL_GPUTransferBuffer* vertexTransferBuffer = SDL_CreateGPUTransferBuffer(device, &vertexTransferInfo);

    const Uint32 textureUploadSize = static_cast<Uint32>(imageWidth * imageHeight * 4);
    SDL_GPUTransferBufferCreateInfo textureTransferInfo{};
    textureTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    textureTransferInfo.size = textureUploadSize;
    textureTransferInfo.props = 0;
    SDL_GPUTransferBuffer* textureTransferBuffer = SDL_CreateGPUTransferBuffer(device, &textureTransferInfo);

    if (!vertexTransferBuffer || !textureTransferBuffer) {
        std::cerr << "Erreur création transfer buffers: " << SDL_GetError() << "\n";
        if (vertexTransferBuffer) SDL_ReleaseGPUTransferBuffer(device, vertexTransferBuffer);
        if (textureTransferBuffer) SDL_ReleaseGPUTransferBuffer(device, textureTransferBuffer);
        SDL_ReleaseGPUSampler(device, logoSampler);
        SDL_ReleaseGPUTexture(device, logoTexture);
        SDL_ReleaseGPUBuffer(device, vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroySurface(imageSurface);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    void* vertexMapped = SDL_MapGPUTransferBuffer(device, vertexTransferBuffer, true);
    if (!vertexMapped) {
        std::cerr << "Erreur map vertex transfer buffer: " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(device, textureTransferBuffer);
        SDL_ReleaseGPUTransferBuffer(device, vertexTransferBuffer);
        SDL_ReleaseGPUSampler(device, logoSampler);
        SDL_ReleaseGPUTexture(device, logoTexture);
        SDL_ReleaseGPUBuffer(device, vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroySurface(imageSurface);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_memcpy(vertexMapped, quadVertices, sizeof(quadVertices));
    SDL_UnmapGPUTransferBuffer(device, vertexTransferBuffer);

    void* textureMapped = SDL_MapGPUTransferBuffer(device, textureTransferBuffer, true);
    if (!textureMapped) {
        std::cerr << "Erreur map texture transfer buffer: " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(device, textureTransferBuffer);
        SDL_ReleaseGPUTransferBuffer(device, vertexTransferBuffer);
        SDL_ReleaseGPUSampler(device, logoSampler);
        SDL_ReleaseGPUTexture(device, logoTexture);
        SDL_ReleaseGPUBuffer(device, vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroySurface(imageSurface);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    const Uint8* srcPixels = static_cast<const Uint8*>(imageSurface->pixels);
    Uint8* dstPixels = static_cast<Uint8*>(textureMapped);
    const int srcPitch = imageSurface->pitch;
    const int dstPitch = imageWidth * 4;
    for (int y = 0; y < imageHeight; ++y) {
        SDL_memcpy(dstPixels + (y * dstPitch), srcPixels + (y * srcPitch), static_cast<size_t>(dstPitch));
    }
    SDL_UnmapGPUTransferBuffer(device, textureTransferBuffer);

    SDL_GPUCommandBuffer* initCmdBuffer = SDL_AcquireGPUCommandBuffer(device);
    if (!initCmdBuffer) {
        std::cerr << "Erreur acquire init command buffer: " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTransferBuffer(device, textureTransferBuffer);
        SDL_ReleaseGPUTransferBuffer(device, vertexTransferBuffer);
        SDL_ReleaseGPUSampler(device, logoSampler);
        SDL_ReleaseGPUTexture(device, logoTexture);
        SDL_ReleaseGPUBuffer(device, vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroySurface(imageSurface);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GPUCopyPass* initCopyPass = SDL_BeginGPUCopyPass(initCmdBuffer);

    SDL_GPUTransferBufferLocation vertexSource{};
    vertexSource.transfer_buffer = vertexTransferBuffer;
    vertexSource.offset = 0;

    SDL_GPUBufferRegion vertexDestination{};
    vertexDestination.buffer = vertexBuffer;
    vertexDestination.offset = 0;
    vertexDestination.size = sizeof(quadVertices);

    SDL_UploadToGPUBuffer(initCopyPass, &vertexSource, &vertexDestination, false);

    SDL_GPUTextureTransferInfo textureSource{};
    textureSource.transfer_buffer = textureTransferBuffer;
    textureSource.offset = 0;
    textureSource.pixels_per_row = static_cast<Uint32>(imageWidth);
    textureSource.rows_per_layer = static_cast<Uint32>(imageHeight);

    SDL_GPUTextureRegion textureDestination{};
    textureDestination.texture = logoTexture;
    textureDestination.mip_level = 0;
    textureDestination.layer = 0;
    textureDestination.x = 0;
    textureDestination.y = 0;
    textureDestination.z = 0;
    textureDestination.w = static_cast<Uint32>(imageWidth);
    textureDestination.h = static_cast<Uint32>(imageHeight);
    textureDestination.d = 1;

    SDL_UploadToGPUTexture(initCopyPass, &textureSource, &textureDestination, false);

    SDL_EndGPUCopyPass(initCopyPass);
    SDL_SubmitGPUCommandBuffer(initCmdBuffer);

    SDL_ReleaseGPUTransferBuffer(device, textureTransferBuffer);
    SDL_ReleaseGPUTransferBuffer(device, vertexTransferBuffer);
    SDL_DestroySurface(imageSurface);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(device);
        if (!cmdBuffer) {
            continue;
        }

        SDL_GPUTexture* swapchainTexture = nullptr;
        if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, window, &swapchainTexture, nullptr, nullptr)) {
            SDL_SubmitGPUCommandBuffer(cmdBuffer);
            continue;
        }

        if (swapchainTexture) {
            SDL_GPUColorTargetInfo colorTarget{};
            colorTarget.texture = swapchainTexture;
            colorTarget.clear_color = SDL_FColor{0.08f, 0.10f, 0.14f, 1.0f};
            colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTarget.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

            SDL_GPUBufferBinding vertexBinding{};
            vertexBinding.buffer = vertexBuffer;
            vertexBinding.offset = 0;
            SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);

            SDL_GPUTextureSamplerBinding fragmentSamplerBinding{};
            fragmentSamplerBinding.texture = logoTexture;
            fragmentSamplerBinding.sampler = logoSampler;
            SDL_BindGPUFragmentSamplers(renderPass, 0, &fragmentSamplerBinding, 1);

            SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);
            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(cmdBuffer);
    }

    SDL_ReleaseGPUSampler(device, logoSampler);
    SDL_ReleaseGPUTexture(device, logoTexture);
    SDL_ReleaseGPUBuffer(device, vertexBuffer);
    SDL_ReleaseGPUGraphicsPipeline(device, pipeline);

    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
