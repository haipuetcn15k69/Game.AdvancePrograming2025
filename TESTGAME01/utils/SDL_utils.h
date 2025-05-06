#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
void logErrorAndExit(const char* msg, const char* error);
SDL_Renderer* createRenderer(SDL_Window* window,int SCREEN_WIDTH,int  SCREEN_HEIGHT);
SDL_Window* initSDL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char* WINDOW_TITLE);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
SDL_Texture *loadTexture(const char *filename, SDL_Renderer* renderer);
void renderTexture(SDL_Texture *texture, int x, int y,
                   SDL_Renderer* renderer);
void quitSDL(Mix_Chunk* gEat,SDL_Texture* gHeadTexture,SDL_Texture* gBodyTexture,SDL_Texture* gFoodTexture,SDL_Texture* gBackgroundTexture,Mix_Chunk* gLose,TTF_Font* gFont,Mix_Music* gMusic,SDL_Renderer* gRenderer,SDL_Window* gWindow);
