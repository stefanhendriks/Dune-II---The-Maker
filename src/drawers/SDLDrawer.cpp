#include "SDLDrawer.hpp"
#include "utils/Graphics.hpp"

#include <iostream>
#include <algorithm>
#include <memory>
#include "include/cAssert.h"


SDLDrawer::SDLDrawer(SDL_Renderer *_renderer)
{
    d2tm_assert(_renderer != nullptr);
    renderer = _renderer;
}

SDLDrawer::~SDLDrawer()
{}

void SDLDrawer::renderSprite(Texture *src,int x, int y,Uint8 opacity)
{
    if (src == nullptr) return;
    SDL_FRect tmp = {(float)x, (float)y, (float)src->w, (float)src->h};
    SDL_SetTextureBlendMode(src->tex.get(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (!SDL_SetTextureAlphaMod(src->tex.get(), opacity)) {
        std::cerr << "no alpha mod "<< SDL_GetError() << std::endl;
    }
    SDL_RenderTexture(renderer, src->tex.get(), NULL, &tmp);
}

void SDLDrawer::renderTexture(SDL_Texture *tex, int x, int y, int w, int h, Uint8 opacity)
{
    if (tex == nullptr) return;
    SDL_FRect tmp = {(float)x, (float)y, (float)w, (float)h};
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (!SDL_SetTextureAlphaMod(tex, opacity)) {
        std::cerr << "no alpha mod "<< SDL_GetError() << std::endl;
    }
    SDL_RenderTexture(renderer, tex, NULL, &tmp);
}

void SDLDrawer::renderFromSurface(SDL_Surface *src, int x, int y,Uint8 opacity)
{
    SDL_FRect tmp = {(float)x, (float)y, (float)src->w, (float)src->h};
    transparentColorKey = SDL_MapRGB(SDL_GetPixelFormatDetails(src->format), SDL_GetSurfacePalette(src), 255, 0, 255);
    SDL_SetSurfaceColorKey(src, true, transparentColorKey);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
    if (!texture) {
        std::cerr << "error drawSprite : " << SDL_GetError();
        return;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opacity);
    SDL_RenderTexture(renderer, texture, NULL, &tmp);
    SDL_DestroyTexture(texture);
}

void SDLDrawer::renderStrechSprite(Texture *src, cRectangle src_pos, cRectangle dest_pos, Uint8 opacity)
{
    if (!SDL_SetTextureBlendMode(src->tex.get(), SDL_BLENDMODE_BLEND)) {
        std::cerr << "Error SDL_SetTextureBlendMode : " << SDL_GetError() << std::endl;
    }
    SDL_SetTextureAlphaMod(src->tex.get(), opacity);
    SDL_FRect srcRect = src_pos.toSDLF();
    SDL_FRect destRect = dest_pos.toSDLF();
    SDL_RenderTexture(renderer, src->tex.get(), &srcRect, &destRect);
}

void SDLDrawer::renderStrechFullSprite(Texture *src, cRectangle dest_pos, Uint8 opacity)
{
    if (!SDL_SetTextureBlendMode(src->tex.get(), SDL_BLENDMODE_BLEND)) {
        std::cerr << "Error SDL_SetTextureBlendMode : " << SDL_GetError() << std::endl;
    }
    SDL_SetTextureAlphaMod(src->tex.get(), opacity);
    SDL_FRect destRect = dest_pos.toSDLF();
    SDL_RenderTexture(renderer, src->tex.get(), nullptr, &destRect);
}

void SDLDrawer::renderRectFillColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity)
{
    SDL_SetRenderDrawColor(renderer, r,g,b, opacity);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_FRect carre = {(float)x, (float)y, (float)width, (float)height};
    SDL_RenderFillRect(renderer, &carre);
}

void SDLDrawer::renderRectFillColor(int x, int y, int width, int height, Color color)
{
    renderRectFillColor(x,y,width,height,color.r, color.g, color.b,color.a);
}

void SDLDrawer::renderRectColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity)
{
    SDL_SetRenderDrawColor(renderer, r,g,b, opacity);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_FRect carre = {(float)x, (float)y, (float)width, (float)height};
    SDL_RenderRect(renderer, &carre);
}

void SDLDrawer::renderRectFillColor(const cRectangle &rect, Color color, Uint8 opacity)
{
    renderRectFillColor(rect.getX(),rect.getY(), rect.getWidth(), rect.getHeight(),
        color.r,color.g,color.b,opacity);
}

void SDLDrawer::renderRectColor(const cRectangle &rect, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity)
{
    renderRectColor(rect.getX(),rect.getY(), rect.getWidth(), rect.getHeight(),r,g,b,opacity);
}

void SDLDrawer::renderRectColor(int x, int y, int width, int height, Color color)
{
    renderRectColor(x,y,width,height,color.r, color.g, color.b,color.a);
}

void SDLDrawer::renderRectColor(const cRectangle &rect, Color color, unsigned char opacity)
{
    renderRectColor(rect.getX(),rect.getY(), rect.getWidth(), rect.getHeight(),
        color.r,color.g,color.b,opacity);
}

void SDLDrawer::resetClippingFor()
{
    SDL_SetRenderClipRect(renderer, nullptr);
}

void SDLDrawer::setClippingFor(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY)
{
    auto tmp = SDL_Rect{topLeftX,topLeftY, bottomRightX-topLeftX, bottomRightY - topLeftY};
    SDL_SetRenderClipRect(renderer, &tmp);
}

void SDLDrawer::gui_DrawRect(const cRectangle &rectangle, Color gui_colorWindow, Color gui_colorBorderLight, Color gui_colorBorderDark)
{
    int x1 = rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();
    SDL_FRect tmp = {(float)x1, (float)y1, (float)width, (float)height};
    renderChangeColor(gui_colorWindow);
    SDL_RenderFillRect(renderer, &tmp);
    renderChangeColor(gui_colorBorderLight);
    SDL_RenderRect(renderer, &tmp);

    // lines to darken the right sides
    renderChangeColor(gui_colorBorderDark);
    // SDL Lines are drawn *including* end coordinate, so need to subtract 1 to make it match
    // with the SDL_RenderFillRect and DrawRect functions.
    int endX = (x1 + width) - 1;
    int endY = (y1 + height) - 1;

    SDL_RenderLine(renderer, (float)endX, (float)y1, (float)endX, (float)endY);
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderLine(renderer, (float)x1, (float)endY, (float)endX, (float)endY);
}

void SDLDrawer::gui_DrawRectBorder(const cRectangle &rectangle, Color gui_colorBorderLight, Color gui_colorBorderDark)
{
    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();
    SDL_FRect tmp = {(float)x1, (float)y1, (float)width, (float)height};
    renderChangeColor(gui_colorBorderLight);
    SDL_RenderRect(renderer, &tmp);
    renderChangeColor(gui_colorBorderDark);

    // SDL Lines are drawn *including* end coordinate, so need to subtract 1 to make it match
    // with the SDL_RenderFillRect and DrawRect functions.
    int endX = (x1 + width) - 1;
    int endY = (y1 + height) - 1;
    SDL_RenderLine(renderer, (float)endX, (float)y1, (float)endX, (float)endY);
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderLine(renderer, (float)x1, (float)endY, (float)endX, (float)endY);
}

void SDLDrawer::renderClearToColor(Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void SDLDrawer::renderLine(int x1, int y1, int x2, int y2, Color color)
{
    renderChangeColor(color);
    SDL_RenderLine(renderer, (float)x1, (float)y1, (float)x2, (float)y2);
}

void SDLDrawer::renderDot(int x, int y, Color color, int size)
{
    if (size < 1) return;
    renderChangeColor(color);
    if (size == 1) {
        SDL_RenderPoint(renderer, (float)x, (float)y);
        return;
    }

    int endX = x + size;
    int endY = y + size;
    for (int sx = x; sx < endX; sx++) {
        for (int sy = y; sy < endY; sy++) {
            SDL_RenderPoint(renderer, (float)sx, (float)sy);
        }
    }
}

bool SDLDrawer::isSurface8BitPaletted(SDL_Surface *bmp)
{
    if (bmp == nullptr) {
        return false;
    }
    return SDL_BITSPERPIXEL(bmp->format) == 8 && SDL_GetSurfacePalette(bmp) != nullptr;
}

// Utility to draw a pixel on a SDL_Surface — surface must be locked before calling!
void SDLDrawer::set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_color)
{
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        return;
    }

    int bpp = SDL_BYTESPERPIXEL(surface->format);
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch;

    switch (bpp) {
        case 1:
            *((Uint8 *)p + x) = (Uint8)pixel_color;
            break;
        case 2:
            *((Uint16 *)p + x) = (Uint16)pixel_color;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                *((Uint8 *)p + x * bpp + 0) = (pixel_color >> 16) & 0xFF;
                *((Uint8 *)p + x * bpp + 1) = (pixel_color >> 8) & 0xFF;
                *((Uint8 *)p + x * bpp + 2) = pixel_color & 0xFF;
            }
            else {
                *((Uint8 *)p + x * bpp + 0) = pixel_color & 0xFF;
                *((Uint8 *)p + x * bpp + 1) = (pixel_color >> 8) & 0xFF;
                *((Uint8 *)p + x * bpp + 2) = (pixel_color >> 16) & 0xFF;
            }
            break;
        case 4:
            *((Uint32 *)p + x) = pixel_color;
            break;
    }
}

void SDLDrawer::FillWithColor(SDL_Surface *src, Color color)
{
    Uint32 mappedColor = SDL_MapRGBA(SDL_GetPixelFormatDetails(src->format),
                                     SDL_GetSurfacePalette(src),
                                     color.r,
                                     color.g,
                                     color.b,
                                     color.a);
    bool result = SDL_FillSurfaceRect(src, NULL, mappedColor);
    if (!result) {
        SDL_Log("SDL_FillRect failed to clear surface: %s", SDL_GetError());
    }
}


void SDLDrawer::setPixel(SDL_Surface *bmp, int x, int y, Color color)
{
    Uint32 mappedColor = SDL_MapRGBA(SDL_GetPixelFormatDetails(bmp->format),
                                     SDL_GetSurfacePalette(bmp),
                                     color.r, color.g, color.b, color.a);

    if (!SDL_LockSurface(bmp)) {
        fprintf(stderr, "Error locking surface: %s\n", SDL_GetError());
        return;
    }
    set_pixel(bmp,x,y,mappedColor);
    SDL_UnlockSurface(bmp);
}


void SDLDrawer::renderChangeColor(Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

// Creates a render texture
Texture* SDLDrawer::createRenderTargetTexture(int width, int height)
{
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                              SDL_PIXELFORMAT_RGBA8888,
                                              SDL_TEXTUREACCESS_TARGET,
                                              width,
                                              height);
    if (!texture) {
        throw std::runtime_error("Error creating render texture: " + std::string(SDL_GetError()));
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return new Texture(texture, width, height, true);
}

// Sets a texture as a render target.
// Takes a pointer to the texture. If nullptr, returns to the main window.
void SDLDrawer::beginDrawingToTexture(Texture* targetTexture)
{
    d2tm_assert(targetTexture && targetTexture->isRenderTarget == true);
    SDL_Texture* currentTarget = SDL_GetRenderTarget(renderer);
    renderTargetStack.push(currentTarget);

    if (!SDL_SetRenderTarget(renderer, targetTexture->tex.get())) {
        renderTargetStack.pop();
        throw std::runtime_error("Error changing render target: " + std::string(SDL_GetError()));
    }
}

void SDLDrawer::endDrawingToTexture()
{
    if (renderTargetStack.empty()) {
        throw std::runtime_error("endDrawingToTexture without beginDrawingToTexture !");
    }
    SDL_Texture* previousTarget = renderTargetStack.top();
    renderTargetStack.pop();
    if (!SDL_SetRenderTarget(renderer, previousTarget)) {
        throw std::runtime_error("Error restoring default render target: " + std::string(SDL_GetError()));
    }
}

Texture *SDLDrawer::createTextureFromIndexedSurfaceWithPalette(SDL_Surface *referenceSurface, int paletteIndexForTransparency, int paletteSwapStart)
{
    d2tm_assert(referenceSurface && "referenceSurface must be given");

    // Step 1: Create a copy of the surface (same INDEX8 format)
    SDL_Surface *modifiableSurface = SDL_CreateSurface(referenceSurface->w, referenceSurface->h, SDL_PIXELFORMAT_INDEX8);
    if (!modifiableSurface) {
        SDL_Log("Error copying surface : %s", SDL_GetError());
        return nullptr;
    }

    // Copy pixels
    SDL_LockSurface(referenceSurface);
    SDL_LockSurface(modifiableSurface);
    Uint8* src = (Uint8*)referenceSurface->pixels;
    Uint8* dst = (Uint8*)modifiableSurface->pixels;

    for (int y = 0; y < referenceSurface->h; y++) {
        memcpy(dst + y * modifiableSurface->pitch,
               src + y * referenceSurface->pitch,
            referenceSurface->w); // 8 bits = 1 byte per pixel
    }
    SDL_UnlockSurface(referenceSurface);
    SDL_UnlockSurface(modifiableSurface);

    // copy palette
    SDL_Palette *refPalette = SDL_GetSurfacePalette(referenceSurface);
    if (!refPalette || refPalette->ncolors <= 0) {
        SDL_Log("No palette in the original image!");
        SDL_DestroySurface(modifiableSurface);
        return nullptr;
    }

    // SDL3 does not guarantee an auto-created palette on surfaces made via
    // SDL_CreateSurface on all platforms. Create one explicitly and attach it.
    SDL_Palette *palette = SDL_CreatePalette(refPalette->ncolors);
    if (!palette) {
        SDL_Log("Failed to create palette: %s", SDL_GetError());
        SDL_DestroySurface(modifiableSurface);
        return nullptr;
    }
    SDL_SetPaletteColors(palette, refPalette->colors, 0, refPalette->ncolors);

    // Apply optional palette swap if requested (caller decides swap start)
    if (paletteSwapStart > -1) {
        int start = paletteSwapStart;
        int s = 144; // original position (harkonnen)
        for (int j = start; j < (start + 7) && s < palette->ncolors; j++) {
            palette->colors[s].r = palette->colors[j].r;
            palette->colors[s].g = palette->colors[j].g;
            palette->colors[s].b = palette->colors[j].b;
            palette->colors[s].a = palette->colors[j].a;
            s++;
        }
    }

    SDL_SetSurfacePalette(modifiableSurface, palette);
    SDL_DestroyPalette(palette); // surface now holds the reference

    // Step 3: Set the transparency index
    if (!SDL_SetSurfaceColorKey(modifiableSurface, true, paletteIndexForTransparency)) {
        SDL_Log("SDL_SetColorKey error : %s", SDL_GetError());
        SDL_DestroySurface(modifiableSurface);
        return nullptr;
    }

    // Step 4: Convert to RGBA8888 to create the texture with alpha
    SDL_Surface *rgbaSurface = SDL_ConvertSurface(modifiableSurface, SDL_PIXELFORMAT_RGBA8888);
    SDL_DestroySurface(modifiableSurface);
    if (!rgbaSurface) {
        SDL_Log("SDL_ConvertSurface error : %s", SDL_GetError());
        return nullptr;
    }

    // Step 5: Create the final texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, rgbaSurface);
    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface error : %s", SDL_GetError());
        SDL_DestroySurface(rgbaSurface);
        return nullptr;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    auto *newTexture = new Texture(texture, rgbaSurface->w, rgbaSurface->h);
    SDL_DestroySurface(rgbaSurface);
    return newTexture;
}
