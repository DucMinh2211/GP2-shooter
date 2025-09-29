#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

enum Resources {
    PLAYER_1,
    PLAYER_2,
    BULLET,
};

const std::unordered_map<Resources, std::string> RESOURCES_NAME {
    {PLAYER_1, "player1"},
    {PLAYER_2, "player2"},
    {BULLET, "bullet"}
};

class ResourceManager {
public:
    ResourceManager(SDL_Renderer* renderer) : _renderer(renderer) {}
    ~ResourceManager() { unload_all(); }

    SDL_Texture* load_texture(const std::string& id, const std::string& path) {
        if (_textures.count(id)) return _textures[id];
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) return nullptr;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
        if (tex) _textures[id] = tex;
        return tex;
    }

    SDL_Texture* get_texture(const std::string& id) const {
        auto it = _textures.find(id);
        return (it != _textures.end()) ? it->second : nullptr;
    }

    void unload_all() {
        for (auto& pair : _textures) SDL_DestroyTexture(pair.second);
        _textures.clear();
    }

private:
    SDL_Renderer* _renderer;
    std::unordered_map<std::string, SDL_Texture*> _textures;
};
