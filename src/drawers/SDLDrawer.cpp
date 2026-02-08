#include "SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "d2tmc.h"

#include <iostream>
#include <algorithm>
#include <memory>


SDLDrawer::SDLDrawer(SDL_Renderer *_renderer)
{
    renderer = _renderer;
}

SDLDrawer::~SDLDrawer()
{}

void SDLDrawer::renderSprite(Texture *src,int x, int y,Uint8 opacity)
{
    if (src == nullptr) return;
    SDL_Rect tmp = {x,y, src->w, src->h };
    SDL_SetTextureBlendMode(src->tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (SDL_SetTextureAlphaMod(src->tex, opacity)<0) {
        std::cerr << "no alpha mod "<< SDL_GetError() << std::endl;
    }
    SDL_RenderCopy(renderer, src->tex, NULL, &tmp);
}

void SDLDrawer::renderTexture(SDL_Texture *tex, int x, int y, int w, int h, Uint8 opacity)
{
    if (tex == nullptr) return;
    SDL_Rect tmp = SDL_Rect{x,y,w,h};
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (SDL_SetTextureAlphaMod(tex, opacity)<0) {
        std::cerr << "no alpha mod "<< SDL_GetError() << std::endl;
    }
    SDL_RenderCopy(renderer, tex, NULL, &tmp);
}

void SDLDrawer::renderFromSurface(SDL_Surface *src, int x, int y,Uint8 opacity)
{
    SDL_Rect tmp = {x,y,src->w, src->h};
    transparentColorKey = SDL_MapRGB(src->format, 255, 0, 255);
    SDL_SetColorKey(src, SDL_TRUE, transparentColorKey);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
    if (!texture) {
        std::cerr << "error drawSprite : " << SDL_GetError();
        return;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opacity);
    SDL_RenderCopy(renderer, texture, NULL, &tmp);
    SDL_DestroyTexture(texture);
}

void SDLDrawer::renderStrechSprite(Texture *src, cRectangle src_pos, cRectangle dest_pos, Uint8 opacity)
{
    if (SDL_SetTextureBlendMode(src->tex, SDL_BLENDMODE_BLEND) < 0) {
        std::cerr << "Error SDL_SetTextureBlendMode : " << SDL_GetError() << std::endl;
    }
    SDL_SetTextureAlphaMod(src->tex, opacity);
    SDL_Rect srcRect = src_pos.toSDL();
    SDL_Rect destRect = dest_pos.toSDL();
    SDL_RenderCopy(renderer, src->tex, &srcRect, &destRect);
}

void SDLDrawer::renderStrechFullSprite(Texture *src, cRectangle dest_pos, Uint8 opacity)
{
    if (SDL_SetTextureBlendMode(src->tex, SDL_BLENDMODE_BLEND) < 0) {
        std::cerr << "Error SDL_SetTextureBlendMode : " << SDL_GetError() << std::endl;
    }
    SDL_SetTextureAlphaMod(src->tex, opacity);
    SDL_Rect destRect = dest_pos.toSDL();
    SDL_RenderCopy(renderer, src->tex, nullptr, &destRect);
}

void SDLDrawer::renderRectFillColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity)
{
    SDL_SetRenderDrawColor(renderer, r,g,b, opacity);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect carre = {x, y, width, height};
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
    SDL_Rect carre = {x, y, width, height};
    SDL_RenderDrawRect(renderer, &carre);
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
    SDL_RenderSetClipRect(renderer, nullptr);
}

void SDLDrawer::setClippingFor(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY)
{
    auto tmp = SDL_Rect{topLeftX,topLeftY, bottomRightX-topLeftX, bottomRightY - topLeftY};
    SDL_RenderSetClipRect(renderer, &tmp);
}

void SDLDrawer::gui_DrawRect(const cRectangle &rectangle, Color gui_colorWindow, Color gui_colorBorderLight, Color gui_colorBorderDark)
{
    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();
    //std::cout << x1 << " " << y1 << " " << width << " " << height << std::endl;
    SDL_Rect tmp = {x1,y1, width, height};
    renderChangeColor(gui_colorWindow);
    SDL_RenderFillRect(renderer, &tmp );
    renderChangeColor(gui_colorBorderLight);
    SDL_RenderDrawRect(renderer, &tmp );

    // lines to darken the right sides
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderDrawLine(renderer, x1+width, y1, x1+width, y1+height);
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderDrawLine(renderer, x1, y1+height, x1+width, y1+height);
}

void SDLDrawer::gui_DrawRectBorder(const cRectangle &rectangle, Color gui_colorBorderLight, Color gui_colorBorderDark)
{
    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();
    //std::cout <<":" << x1 << " " << y1 << " " << width << " " << height << std::endl;
    SDL_Rect tmp = {x1,y1,width,height};
    renderChangeColor(gui_colorBorderLight);
    SDL_RenderDrawRect(renderer, &tmp);
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderDrawLine(renderer, x1+width, y1, x1+width, y1+height);
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderDrawLine(renderer, x1, y1+height, x1+width, y1+height);
}

void SDLDrawer::renderClearToColor(Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void SDLDrawer::renderLine(int x1, int y1, int x2, int y2, Color color)
{
    renderChangeColor(color);
    SDL_RenderDrawLine(renderer,x1, y1, x2, y2);
}

void SDLDrawer::renderDot(int x, int y, Color color, int size)
{
    if (size < 1) return;
    renderChangeColor(color);
    if (size == 1) {
        SDL_RenderDrawPoint(renderer,x, y);
        return;
    }

    int endX = x + size;
    int endY = y + size;
    for (int sx = x; sx < endX; sx++) {
        for (int sy = y; sy < endY; sy++) {
            SDL_RenderDrawPoint(renderer,sx, sy);
        }
    }
}

bool SDLDrawer::isSurface8BitPaletted(SDL_Surface *bmp)
{
    if (bmp == nullptr) {
        return false;
    }
    return bmp->format->BitsPerPixel == 8 && bmp->format->palette != nullptr;
}

// Fonction utilitaire pour dessiner un pixel sur une SDL_Surface
// Assurez-vous que la surface est verrouillée avant d'appeler cette fonction !
void SDLDrawer::set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_color)
{
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        return; // Hors des limites de la surface
    }

    int bpp = surface->format->BytesPerPixel;
    // Pointeur vers le début de la ligne y
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch;

    // Déplacer le pointeur jusqu'au pixel x
    // On convertit Uint32 en la taille appropriée (1, 2, 3 ou 4 octets)
    switch (bpp) {
        case 1: // 8 bits par pixel
            *((Uint8 *)p + x) = (Uint8)pixel_color;
            break;
        case 2: // 16 bits par pixel
            *((Uint16 *)p + x) = (Uint16)pixel_color;
            break;
        case 3: // 24 bits par pixel
            // C'est un peu plus complexe car 3 octets ne s'alignent pas directement avec les types C
            // Il faut copier octet par octet
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
        case 4: // 32 bits par pixel
            *((Uint32 *)p + x) = pixel_color;
            break;
    }
}

void SDLDrawer::FillWithColor(SDL_Surface *src, Color color)
{
    Uint32 mappedColor = SDL_MapRGBA(src->format,
                                     color.r,
                                     color.g,
                                     color.b,
                                     color.a);
    int result = SDL_FillRect(src, NULL, mappedColor);
    if (result < 0) {
        SDL_Log("SDL_FillRect failed to clear surface: %s", SDL_GetError());
    }
}


void SDLDrawer::setPixel(SDL_Surface *bmp, int x, int y, Color color)
{
    Uint32 mappedColor = SDL_MapRGBA(bmp->format,
                                     color.r, color.g, color.b, color.a);

    // Vérrouiller la surface avant de modifier les pixels
    if (SDL_LockSurface(bmp) < 0) {
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
    return new Texture(texture, width, height, true);
}

// Sets a texture as a render target.
// Takes a pointer to the texture. If nullptr, returns to the main window.
void SDLDrawer::beginDrawingToTexture(Texture* targetTexture)
{
    assert(targetTexture && targetTexture->isRenderTarget == true);
    SDL_Texture* currentTarget = SDL_GetRenderTarget(renderer);
    renderTargetStack.push(currentTarget);

    if (SDL_SetRenderTarget(renderer, targetTexture->tex) < 0) {
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
    if (SDL_SetRenderTarget(renderer, previousTarget) < 0) {
        throw std::runtime_error("Error restoring default render target: " + std::string(SDL_GetError()));
    }
}