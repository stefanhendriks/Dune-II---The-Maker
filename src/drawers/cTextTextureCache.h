#pragma once

#include "utils/Color.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <memory>

struct textKey {
    std::string msg;
    Color color;
};

struct textKeyHash {
    std::size_t operator()(const textKey &k) const {
        return std::hash<std::string>()(k.msg) ^ std::hash<uint32_t>()( (k.color.r << 24) | (k.color.g <<16) | (k.color.b <<8) | (k.color.a) );
    }
};

struct textKeyEqual {
    bool operator()(const textKey &lhs, const textKey &rhs) const {
        return lhs.msg == rhs.msg && lhs.color.r == rhs.color.r && lhs.color.g == rhs.color.g &&
               lhs.color.b == rhs.color.b && lhs.color.a == rhs.color.a;
    }
};

struct textCacheEntry {
    SDL_Texture *texture;
    SDL_Texture *shadowsTexture;
    int width;
    int height;
    int lifeCounter;
};

class cTextTextureCache {
public:
    // Constructor, initializes the cache with a TTF font
    explicit cTextTextureCache(TTF_Font *font);
    ~cTextTextureCache();
    // Finds a cached entry for the given color and message, or creates it if not present
    textCacheEntry* findOrCreate(Color color, const std::string &msg) const;
    // Clears the cache, destroying all stored textures
    void resetCache() const;
private:
    std::unique_ptr<textCacheEntry> createCacheEntry(Color color, const std::string &msg) const;
    TTF_Font *m_font;
    mutable std::unordered_map<textKey, std::unique_ptr<textCacheEntry>, textKeyHash, textKeyEqual> m_textCache;
};