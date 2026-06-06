#pragma once

#include <SDL3/SDL.h>

#include <string>

namespace gpustudy {

class GPURenderer {
public:
    GPURenderer();
    ~GPURenderer();

    GPURenderer(const GPURenderer&) = delete;
    GPURenderer& operator=(const GPURenderer&) = delete;

    bool init(const char* gpuDriverHint = "vulkan");
    bool createOffscreenTarget(Uint32 width,
                               Uint32 height,
                               SDL_GPUTextureFormat format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM);
    SDL_Window* createWindow(const char* windowTitle, int windowWidth, int windowHeight) const;
    bool claimWindow(SDL_Window* window) const;
    SDL_GPUGraphicsPipeline* createTrianglePipeline(SDL_Window* window,
                                                    const std::string& vertexSpirvPath,
                                                    const std::string& fragmentSpirvPath) const;
    SDL_GPUGraphicsPipeline* createPositionColorPipeline(SDL_Window* window,
                                                         const std::string& vertexSpirvPath,
                                                         const std::string& fragmentSpirvPath,
                                                         Uint32 stride) const;
    SDL_GPUGraphicsPipeline* createTexturedQuadPipeline(SDL_Window* window,
                                                        const std::string& vertexSpirvPath,
                                                        const std::string& fragmentSpirvPath) const;
    bool renderTriangleFrame(SDL_Window* window,
                             SDL_GPUGraphicsPipeline* pipeline,
                             const SDL_FColor& clearColor) const;
    bool uploadBufferData(SDL_GPUBuffer* buffer,
                          const void* data,
                          Uint32 size) const;
    SDL_GPUBuffer* createVertexBuffer(const void* vertices, Uint32 size) const;
    SDL_GPUBuffer* createQuadVertexBuffer(const void* vertices, Uint32 size) const;
    SDL_GPUTexture* createTextureFromImage(const std::string& imagePath,
                                           SDL_GPUTransferBuffer** transferBufferOut,
                                           Uint32* widthOut,
                                           Uint32* heightOut) const;
    SDL_GPUSampler* createLinearClampSampler() const;
    bool renderTexturedQuadFrame(SDL_Window* window,
                                 SDL_GPUGraphicsPipeline* pipeline,
                                 SDL_GPUBuffer* vertexBuffer,
                                 SDL_GPUTexture* texture,
                                 SDL_GPUSampler* sampler,
                                 const SDL_FColor& clearColor) const;
    bool renderVertexBufferFrame(SDL_Window* window,
                                 SDL_GPUGraphicsPipeline* pipeline,
                                 SDL_GPUBuffer* vertexBuffer,
                                 Uint32 vertexCount,
                                 const SDL_FColor& clearColor) const;
    bool createSimplePipeline(const std::string& vertexSpirvPath,
                              const std::string& fragmentSpirvPath);
    bool renderOffscreenTriangleFrame(const SDL_FColor& clearColor) const;
    bool saveOffscreenToBMP(const std::string& outputBmpPath) const;

    SDL_GPUDevice* device() const { return m_device; }

private:
    bool createReadbackBuffer();
    bool saveReadbackToBMP(const std::string& outputBmpPath) const;
    SDL_GPUShader* loadShader(const std::string& filename,
                              SDL_GPUShaderStage stage,
                              Uint32 numSamplers = 0) const;
    void releasePipeline();
    void releaseTargetResources();
    void shutdown();

private:
    SDL_GPUDevice* m_device;
    SDL_GPUGraphicsPipeline* m_pipeline;

    SDL_GPUTexture* m_offscreenTarget;
    SDL_GPUTransferBuffer* m_readbackBuffer;

    Uint32 m_width;
    Uint32 m_height;
    SDL_GPUTextureFormat m_format;
};

} // namespace gpustudy
