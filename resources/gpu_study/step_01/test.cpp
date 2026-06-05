#include <SDL3/SDL.h>
#include <iostream>
#include <string>

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

    SDL_Window* window = SDL_CreateWindow("SDL3 GPU - Le Triangle en HLSL", 800, 600, 0);
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
        std::cerr << "Erreur au chargement des shaders. Vérifiez que les fichiers .spirv sont au bon endroit.\n";
        // Nettoyage prématuré si échec
        if (vertexShader) SDL_ReleaseGPUShader(device, vertexShader);
        if (pixelShader) SDL_ReleaseGPUShader(device, pixelShader);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
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
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(device, window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

    // Pas de gestion de Vertex Layout ici (car géré par l'ID dans le shader !)
    pipelineInfo.vertex_input_state.num_vertex_buffers = 0;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 0;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
    if (!pipeline) {
        std::cerr << "Erreur lors de la création du Pipeline Graphique\n";
    }

    // Les shaders individuels peuvent être libérés une fois injectés dans le pipeline
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, pixelShader);

    // Boucle d'événements
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // --- ENREGISTREMENT ET SOUMISSION DES COMMANDES ---
        
        // 1. Acquérir un Command Buffer
        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(device);
        if (!cmdBuffer) continue;

        // 2. Acquérir la texture d'affichage pour cette frame
        SDL_GPUTexture* swapchainTexture;
        if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, window, &swapchainTexture, nullptr, nullptr)) {
            SDL_SubmitGPUCommandBuffer(cmdBuffer);
            continue;
        }

        if (swapchainTexture != nullptr) {
            // Configuration de la passe (Équivalent moderne de glClear)
            SDL_GPUColorTargetInfo colorTarget{};
            colorTarget.texture = swapchainTexture;
            colorTarget.clear_color = SDL_FColor{0.1f, 0.15f, 0.2f, 1.0f}; // Fond bleu nuit poli
            colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;                    // Demande d'effacer au début
            colorTarget.store_op = SDL_GPU_STOREOP_STORE;                  // Demande de conserver le dessin à la fin

            // 3. Ouvrir la Render Pass
            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
            
            // 4. Lier notre état graphique complet (Shaders + configurations)
            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
            
            // 5. Demander le rendu : 3 sommets, 1 instance, index de départ 0, instance de départ 0
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
    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}