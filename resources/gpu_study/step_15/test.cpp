#include "../common/GPURenderer.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    gpustudy::GPURenderer renderer;

    if (!renderer.init()) return -1;
    if (!renderer.createOffscreenTarget(800, 600)) return -1;
    if (!renderer.createSimplePipeline("triangle_vs.spirv", "triangle_ps.spirv")) return -1;

    const SDL_FColor clearColor = SDL_FColor{0.1f, 0.15f, 0.2f, 1.0f};
    if (!renderer.renderOffscreenTriangleFrame(clearColor)) {
        return -1;
    }

    if (!renderer.saveOffscreenToBMP("triangle_capture.bmp")) {
        return -1;
    }

    std::cout << "Image sauvegardee : triangle_capture.bmp\n";
    return 0;
}
