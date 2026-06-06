#include "../common/GPURenderer.hpp"

#include <iostream>

struct QuadVertex {
    float x, y;
    float u, v;
};

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    gpustudy::GPURenderer renderer;

    if (!renderer.init()) return -1;

    SDL_Window* window = renderer.createWindow("SDL3 GPU - Step 14 Texture (2nd Gen)", 800, 600);
    if (!window) return -1;

    if (!renderer.claimWindow(window)) {
        SDL_DestroyWindow(window);
        return -1;
    }

    SDL_GPUGraphicsPipeline* pipeline = renderer.createTexturedQuadPipeline(window, "triangle_vs.spirv", "triangle_ps.spirv");
    if (!pipeline) {
        SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
        SDL_DestroyWindow(window);
        return -1;
    }

    const QuadVertex quadVertices[6] = {
        { -1.0f,  0.5f, 0.0f, 1.0f },
        { -0.2f,  0.5f, 1.0f, 1.0f },
        { -0.2f, -0.5f, 1.0f, 0.0f },
        { -1.0f,  0.5f, 0.0f, 1.0f },
        { -0.2f, -0.5f, 1.0f, 0.0f },
        { -1.0f, -0.5f, 0.0f, 0.0f }
    };

    SDL_GPUBuffer* vertexBuffer = renderer.createQuadVertexBuffer(quadVertices, sizeof(quadVertices));
    if (!vertexBuffer) {
        SDL_ReleaseGPUGraphicsPipeline(renderer.device(), pipeline);
        SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
        SDL_DestroyWindow(window);
        return -1;
    }

    SDL_GPUTransferBuffer* imageTransferBuffer = nullptr;
    Uint32 imageWidth = 0;
    Uint32 imageHeight = 0;
    SDL_GPUTexture* texture = renderer.createTextureFromImage("../logo_invaders.png", &imageTransferBuffer, &imageWidth, &imageHeight);
    if (!texture) {
        SDL_ReleaseGPUBuffer(renderer.device(), vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(renderer.device(), pipeline);
        SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
        SDL_DestroyWindow(window);
        return -1;
    }
    if (imageTransferBuffer) {
        SDL_ReleaseGPUTransferBuffer(renderer.device(), imageTransferBuffer);
    }

    SDL_GPUSampler* sampler = renderer.createLinearClampSampler();
    if (!sampler) {
        SDL_ReleaseGPUTexture(renderer.device(), texture);
        SDL_ReleaseGPUBuffer(renderer.device(), vertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(renderer.device(), pipeline);
        SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
        SDL_DestroyWindow(window);
        return -1;
    }

    const SDL_FColor clearColor = SDL_FColor{0.08f, 0.10f, 0.14f, 1.0f};

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        if (!renderer.renderTexturedQuadFrame(window, pipeline, vertexBuffer, texture, sampler, clearColor)) {
            std::cerr << "Erreur rendu frame texture\n";
            running = false;
        }
    }

    SDL_ReleaseGPUSampler(renderer.device(), sampler);
    SDL_ReleaseGPUTexture(renderer.device(), texture);
    SDL_ReleaseGPUBuffer(renderer.device(), vertexBuffer);
    SDL_ReleaseGPUGraphicsPipeline(renderer.device(), pipeline);
    SDL_ReleaseWindowFromGPUDevice(renderer.device(), window);
    SDL_DestroyWindow(window);

    return 0;
}
