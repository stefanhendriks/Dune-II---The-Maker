#include "../common/GPURenderer.hpp"

#include <cstddef>
#include <iostream>

struct Vertex {
    float x, y;
    float r, g, b, a;
};

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    gpustudy::GPURenderer renderer;
    if (!renderer.init()) return -1;

    SDL_Window* window = renderer.createWindow("SDL3 GPU - Step 12 Vertex Buffer (2nd Gen)", 800, 600);
    if (!window) return -1;

    if (!renderer.claimWindow(window)) {
        SDL_DestroyWindow(window);
        return -1;
    }

    SDL_GPUGraphicsPipeline* pipeline = renderer.createPositionColorPipeline(window, "triangle_vs.spirv", "triangle_ps.spirv", sizeof(Vertex));
    if (!pipeline) {
        SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
        SDL_DestroyWindow(window);
        return -1;
    }

    const Vertex triangleVertices[3] = {
        {  0.1f,  0.5f,   1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f,   0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f, -0.5f,   0.0f, 0.0f, 1.0f, 1.0f }
    };

    SDL_GPUBuffer* vertexBuffer = renderer.createVertexBuffer(triangleVertices, sizeof(triangleVertices));
    if (!vertexBuffer) {
        SDL_ReleaseGPUGraphicsPipeline(renderer.device(), pipeline);
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

        if (!renderer.renderVertexBufferFrame(window, pipeline, vertexBuffer, 3, clearColor)) {
            std::cerr << "Erreur rendu frame vertex buffer\n";
            running = false;
        }
    }

    SDL_ReleaseGPUBuffer(renderer.device(), vertexBuffer);
    SDL_ReleaseGPUGraphicsPipeline(renderer.device(), pipeline);
    SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
    SDL_DestroyWindow(window);
    return 0;
}
