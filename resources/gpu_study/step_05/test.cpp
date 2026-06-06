#include <SDL3/SDL.h>
#include <iostream>
#include <string>

bool SaveTextureToBMP(SDL_GPUDevice* device,
                      SDL_GPUTransferBuffer* downloadBuffer,
                      SDL_GPUTextureFormat textureFormat,
                      Uint32 width,
                      Uint32 height,
                      const std::string& outputPath) {
    void* mappedPixels = SDL_MapGPUTransferBuffer(device, downloadBuffer, false);
    if (!mappedPixels) {
        std::cerr << "Erreur map transfer buffer (download) : " << SDL_GetError() << "\n";
        return false;
    }

    const Uint32 bytesPerPixel = SDL_GPUTextureFormatTexelBlockSize(textureFormat);
    if (bytesPerPixel == 0) {
        std::cerr << "Erreur : format de texture non supporte pour l'export.\n";
        SDL_UnmapGPUTransferBuffer(device, downloadBuffer);
        return false;
    }

    const int pitch = static_cast<int>(width * bytesPerPixel);
    const SDL_PixelFormat pixelFormat = SDL_GetPixelFormatFromGPUTextureFormat(textureFormat);
    SDL_Surface* surface = SDL_CreateSurfaceFrom(
        static_cast<int>(width),
        static_cast<int>(height),
        pixelFormat,
        mappedPixels,
        pitch
    );

    if (!surface) {
        std::cerr << "Erreur creation surface CPU : " << SDL_GetError() << "\n";
        SDL_UnmapGPUTransferBuffer(device, downloadBuffer);
        return false;
    }

    const bool saved = SDL_SaveBMP(surface, outputPath.c_str());
    if (!saved) {
        std::cerr << "Erreur sauvegarde BMP : " << SDL_GetError() << "\n";
    }

    SDL_DestroySurface(surface);
    SDL_UnmapGPUTransferBuffer(device, downloadBuffer);
    return saved;
}

// Fonction utilitaire pour charger le bytecode SPIR-V
SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const std::string& filename, SDL_GPUShaderStage stage) {
    size_t size;
    // Charge le fichier binaire de manière sécurisée via SDL3
    void* code = SDL_LoadFile(filename.c_str(), &size);
    if (!code) {
        std::cerr << "Erreur : Impossible de lire le shader : " << filename << "\n";
        return nullptr;
    }

    SDL_GPUShaderCreateInfo shaderInfo{};
    shaderInfo.code_size = size;
    shaderInfo.code = static_cast<const uint8_t*>(code);
    // Correspond exactement aux noms des fonctions dans Triangle.hlsl
    shaderInfo.entrypoint = (stage == SDL_GPU_SHADERSTAGE_VERTEX) ? "VS_Main" : "PS_Main";
    shaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shaderInfo.stage = stage;
    
    // Notre shader de test n'utilise ni samplers (textures) ni UBO (uniforms)
    shaderInfo.num_samplers = 0;
    shaderInfo.num_uniform_buffers = 0;
    shaderInfo.num_storage_buffers = 0;
    shaderInfo.num_storage_textures = 0;

    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
    
    SDL_free(code); // Libère la mémoire allouée par SDL_LoadFile
    return shader;
}

int main(int argc, char* argv[]) {
    // Initialisation standard de SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Erreur SDL_Init : " << SDL_GetError() << "\n";
        return -1;
    }
    
    SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");
    // Création du Device GPU (Force le format SPIR-V pour l'exemple)
    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device) {
        std::cerr << "Erreur GPUDevice : " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Chargement de nos shaders compilés
    SDL_GPUShader* vertexShader = LoadShader(device, "triangle_vs.spirv", SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader* pixelShader  = LoadShader(device, "triangle_ps.spirv", SDL_GPU_SHADERSTAGE_FRAGMENT);

    if (!vertexShader || !pixelShader) {
        std::cerr << "Erreur au chargement des shaders. Vérifiez que les fichiers .spirv sont au bon endroit.\n";
        // Nettoyage prématuré si échec
        if (vertexShader) SDL_ReleaseGPUShader(device, vertexShader);
        if (pixelShader) SDL_ReleaseGPUShader(device, pixelShader);
        SDL_DestroyGPUDevice(device);
        SDL_Quit();
        return -1;
    }

    constexpr Uint32 kOutputWidth = 800;
    constexpr Uint32 kOutputHeight = 600;
    const SDL_GPUTextureFormat kOutputFormat = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;

    SDL_GPUTextureCreateInfo outputTextureInfo{};
    outputTextureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    outputTextureInfo.format = kOutputFormat;
    outputTextureInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
    outputTextureInfo.width = kOutputWidth;
    outputTextureInfo.height = kOutputHeight;
    outputTextureInfo.layer_count_or_depth = 1;
    outputTextureInfo.num_levels = 1;
    outputTextureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    outputTextureInfo.props = 0;

    SDL_GPUTexture* outputTexture = SDL_CreateGPUTexture(device, &outputTextureInfo);
    if (!outputTexture) {
        std::cerr << "Erreur creation texture de rendu : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUShader(device, vertexShader);
        SDL_ReleaseGPUShader(device, pixelShader);
        SDL_DestroyGPUDevice(device);
        SDL_Quit();
        return -1;
    }

    const Uint32 bytesPerPixel = SDL_GPUTextureFormatTexelBlockSize(kOutputFormat);
    SDL_GPUTransferBufferCreateInfo readbackBufferInfo{};
    readbackBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
    readbackBufferInfo.size = kOutputWidth * kOutputHeight * bytesPerPixel;
    readbackBufferInfo.props = 0;

    SDL_GPUTransferBuffer* readbackBuffer = SDL_CreateGPUTransferBuffer(device, &readbackBufferInfo);
    if (!readbackBuffer) {
        std::cerr << "Erreur creation transfer buffer de readback : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUTexture(device, outputTexture);
        SDL_ReleaseGPUShader(device, vertexShader);
        SDL_ReleaseGPUShader(device, pixelShader);
        SDL_DestroyGPUDevice(device);
        SDL_Quit();
        return -1;
    }

    // --- CRÉATION DU PIPELINE (Le fameux PSO immuable) ---
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    
    // Configuration des Shaders
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = pixelShader;
    
    // Topologie : on dessine des triangles
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    // 1. On crée notre propre description de cible de couleur
    SDL_GPUColorTargetDescription colorTargetDesc{};
    colorTargetDesc.format = kOutputFormat;

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

    // Pas de gestion de Vertex Layout ici (car géré par l'ID dans le shader !)
    pipelineInfo.vertex_input_state.num_vertex_buffers = 0;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 0;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    if (!pipeline) {
        std::cerr << "Erreur lors de la création du Pipeline Graphique : " << SDL_GetError() << "\n";
    }

    // Les shaders individuels peuvent être libérés une fois injectés dans le pipeline
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, pixelShader);

    if (pipeline) {
        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(device);
        if (!cmdBuffer) {
            std::cerr << "Erreur acquisition command buffer : " << SDL_GetError() << "\n";
        } else {
            SDL_GPUColorTargetInfo colorTarget{};
            colorTarget.texture = outputTexture;
            colorTarget.clear_color = SDL_FColor{0.1f, 0.15f, 0.2f, 1.0f};
            colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTarget.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
            SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
            SDL_EndGPURenderPass(renderPass);

            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);

            SDL_GPUTextureRegion sourceRegion{};
            sourceRegion.texture = outputTexture;
            sourceRegion.mip_level = 0;
            sourceRegion.layer = 0;
            sourceRegion.x = 0;
            sourceRegion.y = 0;
            sourceRegion.z = 0;
            sourceRegion.w = kOutputWidth;
            sourceRegion.h = kOutputHeight;
            sourceRegion.d = 1;

            SDL_GPUTextureTransferInfo destinationInfo{};
            destinationInfo.transfer_buffer = readbackBuffer;
            destinationInfo.offset = 0;
            destinationInfo.pixels_per_row = kOutputWidth;
            destinationInfo.rows_per_layer = kOutputHeight;

            SDL_DownloadFromGPUTexture(copyPass, &sourceRegion, &destinationInfo);
            SDL_EndGPUCopyPass(copyPass);

            SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmdBuffer);
            if (!fence) {
                std::cerr << "Erreur submit avec fence : " << SDL_GetError() << "\n";
            } else {
                SDL_GPUFence* fences[1] = { fence };
                if (!SDL_WaitForGPUFences(device, true, fences, 1)) {
                    std::cerr << "Erreur attente fence GPU : " << SDL_GetError() << "\n";
                } else {
                    const std::string outputFile = "triangle_capture.bmp";
                    if (SaveTextureToBMP(device, readbackBuffer, kOutputFormat, kOutputWidth, kOutputHeight, outputFile)) {
                        std::cout << "Image sauvegardee : " << outputFile << "\n";
                    }
                }
                SDL_ReleaseGPUFence(device, fence);
            }
        }
    }

    // --- NETTOYAGE FINAL ---
    if (pipeline) {
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
    }
    if (readbackBuffer) {
        SDL_ReleaseGPUTransferBuffer(device, readbackBuffer);
    }
    if (outputTexture) {
        SDL_ReleaseGPUTexture(device, outputTexture);
    }
    SDL_DestroyGPUDevice(device);
    SDL_Quit();
    
    return 0;
}