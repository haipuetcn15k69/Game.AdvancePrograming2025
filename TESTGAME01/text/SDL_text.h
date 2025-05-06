#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <string>
using namespace std;
TTF_Font* loadFont(const char* path, int size);
SDL_Texture* renderText(const char* text, TTF_Font* font, SDL_Color textColor,SDL_Renderer* renderer);
void RenderText (const string& text , int x , int y , SDL_Color color,TTF_Font* gFont,SDL_Renderer* gRenderer);

