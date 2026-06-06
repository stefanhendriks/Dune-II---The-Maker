#include "../common/GPURenderer.hpp"

#include <cmath>
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

    SDL_Window* window = renderer.createWindow("SDL3 GPU - Step 12.5 Color Pulse (2nd Gen)", 800, 600);
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

    Vertex triangleVertices[3] = {
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

        float time = SDL_GetTicks() / 1000.0f;
        float pulse = (std::sin(time * 3.0f) + 1.0f) * 0.5f;
        triangleVertices[0].r = pulse;
        triangleVertices[1].g = 1.0f - pulse;
        triangleVertices[2].b = pulse;

        if (!renderer.uploadBufferData(vertexBuffer, triangleVertices, sizeof(triangleVertices))) {
            std::cerr << "Erreur upload vertex buffer\n";
            running = false;
            continue;
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
