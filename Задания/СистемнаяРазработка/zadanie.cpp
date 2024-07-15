#include <iostream>
#include <string>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <SDL.h>
#include <SDL_ttf.h>

void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, bool vertical) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
    
    if (vertical) {
        for (size_t i = 0; i < text.length(); ++i) {
            std::string letter(1, text[i]);
            surface = TTF_RenderUTF8_Blended(font, letter.c_str(), color);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            dstrect = { x, y + static_cast<int>(i * surface->h), surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_FreeSurface(surface);
        }
    } else {
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    }

    SDL_DestroyTexture(texture);
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void draw(const char* orientation, const char* text) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return;
    }
    
    SDL_Window* window = SDL_CreateWindow("WebAssembly Text Renderer", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }

    TTF_Font* font = TTF_OpenFont("DejaVuSans-Bold.ttf", 24);
    if (font == nullptr) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    std::string text_str = text;
    renderText(renderer, font, text_str, 100, 100, false); // Горизонтальный текст
    renderText(renderer, font, ":", 100 + static_cast<int>(text_str.length() * 24), 100, false); // Символ ':'
    
    bool vertical = (orientation[0] == 'v');
    renderText(renderer, font, text_str, 100 + static_cast<int>((text_str.length() + 1) * 24), 100, vertical); // Вертикальный или горизонтальный текст

    SDL_RenderPresent(renderer);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

}

int main() {
    return 0;
}
