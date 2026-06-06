#include "../common/GPURenderer.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    gpustudy::GPURenderer renderer;
    if (!renderer.init()) return -1;

    SDL_Window* window = renderer.createWindow("SDL3 GPU - Step 11 Triangle (toolbox)", 800, 600);
    if (!window) return -1;

    if (!renderer.claimWindow(window)) {
        SDL_DestroyWindow(window);
        return -1;
    }

    SDL_GPUGraphicsPipeline* pipeline = renderer.createTrianglePipeline(window, "triangle_vs.spirv", "triangle_ps.spirv");
    if (!pipeline) {
        SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
        SDL_DestroyWindow(window);
        return -1;
    }

    const SDL_FColor clearColor = SDL_FColor{0.1f, 0.15f, 0.2f, 1.0f};

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        if (!renderer.renderTriangleFrame(window, pipeline, clearColor)) {
            std::cerr << "Erreur rendu frame triangle\n";
            running = false;
        }
    }

    SDL_ReleaseGPUGraphicsPipeline(renderer.device(), pipeline);
    SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
    SDL_DestroyWindow(window);

    return 0;
}