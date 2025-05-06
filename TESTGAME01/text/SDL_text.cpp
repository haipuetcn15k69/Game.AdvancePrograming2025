
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "SDL_text.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;
TTF_Font* loadFont(const char* path, int size)
    {
        TTF_Font* gFont = TTF_OpenFont( path, size );
        if (gFont == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                           SDL_LOG_PRIORITY_ERROR,
                           "Load font %s", TTF_GetError());
        }
}
SDL_Texture* renderText(const char* text, TTF_Font* font, SDL_Color textColor,SDL_Renderer* renderer)
    {
        SDL_Surface* textSurface =
                TTF_RenderText_Solid( font, text, textColor );
        if( textSurface == nullptr ) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                           SDL_LOG_PRIORITY_ERROR,
                           "Render text surface %s", TTF_GetError());
            return nullptr;
        }

        SDL_Texture* texture =
        SDL_CreateTextureFromSurface( renderer, textSurface );
        if( texture == nullptr ) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                           SDL_LOG_PRIORITY_ERROR,
                           "Create texture from text %s", SDL_GetError());
        }
        SDL_FreeSurface( textSurface );
        return texture;
    }
void RenderText (const string& text , int x , int y , SDL_Color color,TTF_Font* gFont,SDL_Renderer* gRenderer)
{
     if (!gFont || !gRenderer) return;
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
    if (!textSurface) { cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << endl; return; }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (!textTexture) { cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl; SDL_FreeSurface(textSurface); return; }
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(gRenderer, textTexture, nullptr, &renderQuad);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}
