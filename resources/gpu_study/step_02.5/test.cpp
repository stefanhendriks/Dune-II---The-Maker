#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <cmath> // Pour std::sin

// Structure de Vertex standard (24 octets : position 2D + couleur RGBA)
struct Vertex {
    float x, y;        // POSITION -> Location 0
    float r, g, b, a;  // COLOR0   -> Location 1
};

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
    
    // Zéro uniforme traditionnel (UBO), tout passe par le flot de sommets
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

    SDL_Window* window = SDL_CreateWindow("SDL3 GPU - Vertex Buffer Method", 800, 600, 0);
    if (!window) {
        std::cerr << "Erreur Window : " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }
    
    SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");
    // Création du Device GPU (Force le format SPIR-V pour l'exemple)
    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device) {
        std::cerr << "Erreur GPUDevice : " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Lier la fenêtre au device GPU
    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        std::cerr << "Erreur Claim Window\n";
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Chargement de nos shaders compilés
    SDL_GPUShader* vertexShader = LoadShader(device, "triangle_vs.spirv", SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader* pixelShader  = LoadShader(device, "triangle_ps.spirv", SDL_GPU_SHADERSTAGE_FRAGMENT);

    if (!vertexShader || !pixelShader) {
        std::cerr << "Erreur au chargement des shaders.\n";
        if (vertexShader) SDL_ReleaseGPUShader(device, vertexShader);
        if (pixelShader) SDL_ReleaseGPUShader(device, pixelShader);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // --- CONFIGURATION DU LAYOUT DES ATTRIBUTS DE SOMMETS ---
    SDL_GPUVertexBufferDescription bufferDesc{};
    bufferDesc.slot = 0;
    bufferDesc.pitch = sizeof(Vertex); // 24 octets (2x float pos + 4x float color)
    bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

    SDL_GPUVertexAttribute attributes[2]{};
    // Attribut 0 : POSITION (x, y) -> Mappe sur la Location 0 du Shader
    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2; // Seulement X et Y !
    attributes[0].offset = offsetof(Vertex, x);

    // Attribut 1 : COLOR0 (r, g, b, a) -> Mappe sur la Location 1 du Shader
    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    attributes[1].offset = offsetof(Vertex, r);

    // --- CRÉATION DU PIPELINE (PSO) ---
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    
    // Configuration des Shaders
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = pixelShader;
    
    // Topologie : on dessine des triangles
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    // Injection explicite du layout des sommets pour satisfaire Vulkan
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDesc;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;

    SDL_GPUColorTargetDescription colorTargetDesc{};
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(device, window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    if (!pipeline) {
        std::cerr << "Erreur lors de la création du Pipeline Graphique : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUShader(device, vertexShader);
        SDL_ReleaseGPUShader(device, pixelShader);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Une fois liés au PSO, les shaders intermédiaires peuvent être libérés
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, pixelShader);

    // Initialisation géométrique classique
    Vertex triangleVertices[3] = {
        {  0.1f,  0.5f,   1.0f, 0.0f, 0.0f, 1.0f }, // Sommet 0
        {  0.5f, -0.5f,   0.0f, 1.0f, 0.0f, 1.0f }, // Sommet 1
        { -0.5f, -0.5f,   0.0f, 0.0f, 1.0f, 1.0f }  // Sommet 2
    };

    SDL_GPUBufferCreateInfo vertexBufferInfo{};
    vertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBufferInfo.size = sizeof(triangleVertices);
    vertexBufferInfo.props = 0;
    SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(device, &vertexBufferInfo);
    if (!vertexBuffer) {
        std::cerr << "Erreur création vertex buffer : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GPUTransferBufferCreateInfo transferBufferInfo{};
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferInfo.size = sizeof(triangleVertices);
    transferBufferInfo.props = 0;
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);
    if (!transferBuffer) {
        std::cerr << "Erreur création transfer buffer : " << SDL_GetError() << "\n";
        SDL_ReleaseGPUBuffer(device, vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // --- ANIMATION PAR SINUS ---
        float time = SDL_GetTicks() / 1000.0f;
        float pulse = (std::sin(time * 3.0f) + 1.0f) * 0.5f; 

        triangleVertices[0].r = pulse;          
        triangleVertices[1].g = 1.0f - pulse;   

        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(device);
        if (!cmdBuffer) continue;

        void* mapped = SDL_MapGPUTransferBuffer(device, transferBuffer, true);
        if (!mapped) {
            std::cerr << "Erreur map transfer buffer : " << SDL_GetError() << "\n";
            SDL_SubmitGPUCommandBuffer(cmdBuffer);
            continue;
        }
        SDL_memcpy(mapped, triangleVertices, sizeof(triangleVertices));
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);
        SDL_GPUTransferBufferLocation sourceLocation{};
        sourceLocation.transfer_buffer = transferBuffer;
        sourceLocation.offset = 0;

        SDL_GPUBufferRegion destinationRegion{};
        destinationRegion.buffer = vertexBuffer;
        destinationRegion.offset = 0;
        destinationRegion.size = sizeof(triangleVertices);

        SDL_UploadToGPUBuffer(copyPass, &sourceLocation, &destinationRegion, false);
        SDL_EndGPUCopyPass(copyPass);

        SDL_GPUTexture* swapchainTexture;
        if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, window, &swapchainTexture, nullptr, nullptr)) {
            SDL_SubmitGPUCommandBuffer(cmdBuffer);
            continue;
        }

        if (swapchainTexture != nullptr) {
            SDL_GPUColorTargetInfo colorTarget{};
            colorTarget.texture = swapchainTexture;
            colorTarget.clear_color = SDL_FColor{0.1f, 0.15f, 0.2f, 1.0f};
            colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTarget.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
            
            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

            SDL_GPUBufferBinding vertexBinding{};
            vertexBinding.buffer = vertexBuffer;
            vertexBinding.offset = 0;
            SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);

            SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

            // 6. Fermer la passe
            SDL_EndGPURenderPass(renderPass);
        }

        // 7. Envoyer les commandes au GPU (L'affichage bascule à l'écran)
        SDL_SubmitGPUCommandBuffer(cmdBuffer);
    }

    // --- NETTOYAGE FINAL ---
    if (pipeline) {
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
    }
    if (transferBuffer) {
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
    }
    if (vertexBuffer) {
        SDL_ReleaseGPUBuffer(device, vertexBuffer);
    }
    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
