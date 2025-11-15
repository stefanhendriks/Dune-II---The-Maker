#include "drawers/cTextTextureCache.h"
#include "drawers/SDLDrawer.hpp"
#include "include/d2tmc.h"

cTextTextureCache::cTextTextureCache(TTF_Font *font)
    : m_font(font)
{}

cTextTextureCache::~cTextTextureCache()
{
    for (auto &pair : m_textCache) {
        auto &cacheEntry = pair.second;
        if (cacheEntry->texture) {
            SDL_DestroyTexture(cacheEntry->texture);
        }
        if (cacheEntry->shadowsTexture) {
            SDL_DestroyTexture(cacheEntry->shadowsTexture);
        }
    }
}

std::unique_ptr<textCacheEntry> cTextTextureCache::createCacheEntry(Color color, const std::string &msg) const
{
    auto newCacheEntry = std::make_unique<textCacheEntry>();
    SDL_Surface *textSurface = TTF_RenderText_Blended(m_font, msg.c_str(), Color::black().toSDL());
    newCacheEntry->shadowsTexture = SDL_CreateTextureFromSurface(renderDrawer->getRenderer(), textSurface);
    SDL_FreeSurface(textSurface);

    textSurface = TTF_RenderText_Blended(m_font, msg.c_str(), color.toSDL());
    newCacheEntry->texture = SDL_CreateTextureFromSurface(renderDrawer->getRenderer(), textSurface);
    newCacheEntry->width = textSurface->w;
    newCacheEntry->height = textSurface->h;
    newCacheEntry->lifeCounter = 10;
    SDL_FreeSurface(textSurface);

    return newCacheEntry;
}

textCacheEntry* cTextTextureCache::findOrCreate(Color color, const std::string &msg) const 
{
    textKey key{msg, color};
    auto it = m_textCache.find(key);
    if (it == m_textCache.end()) {
        auto newCacheEntry = createCacheEntry(color, msg);
        auto result = m_textCache.emplace(key, std::move(newCacheEntry));
        return result.first->second.get();
    }
    return it->second.get();
}

void cTextTextureCache::resetCache() const
{
    auto it = m_textCache.begin();
    while (it != m_textCache.end()) {
        auto &cacheEntry = it->second;
        if (cacheEntry->lifeCounter == 0) {
            if (cacheEntry->texture) SDL_DestroyTexture(cacheEntry->texture);
            if (cacheEntry->shadowsTexture) SDL_DestroyTexture(cacheEntry->shadowsTexture);
            it = m_textCache.erase(it);
        } else {
            cacheEntry->lifeCounter = 0;
            ++it;
        }
    }
}