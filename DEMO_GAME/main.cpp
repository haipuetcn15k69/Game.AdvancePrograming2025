#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "utils/SDL_utils.h"
#include <SDL_mixer.h>
#include "mixer/SDL-Mix.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL_ttf.h>
#include "text/SDL_text.h"
#include <fstream>
#include <algorithm>
#include "score/score.h"
 using namespace std;
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 800;
const char* WINDOW_TITLE = "SnakeGame New Edition: Dragon World";
const int RECT_SIZE = 20;
bool gameIsRunning = true;
bool   showDouble   = false;
Uint32 doubleStart  = 0;
const Uint32 DOUBLE_DURATION = 1000;

//khai báo con trỏ âm thanh
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font* gFont = nullptr;
TTF_Font* gBigFont = nullptr;
Mix_Music* gMusic = nullptr;
Mix_Chunk* gEat = nullptr;
Mix_Chunk* gLose = nullptr;

//khai báo con trỏ ảnh
SDL_Texture* gHeadTexture = nullptr;
SDL_Texture* gBodyTexture = nullptr;
SDL_Texture* gFoodTexture = nullptr;
SDL_Texture* gBackgroundTexture = nullptr;
SDL_Texture* menuBackground = nullptr;
SDL_Texture* gNewFood = nullptr;
SDL_Texture* gBestFood = nullptr;
// Class trạng thái
enum GameState {MENU , PLAYING , PAUSED , HIGH_SCORE_DISPLAY, SPEED_SELECTION};
GameState currentState = MENU;
// khai bao score
int score = 0,highScore  = 0, gameSpeedDelay = 150;
//Class dữ liệu điểm và hàm so sánh trùng điểm
struct Point
{
    int x,y;
    bool operator== (const Point other) const{
    return (other.x == x && other.y == y);
    }

};
vector <Point> body;
vector <Point> foods;
const int FOOD_COUNT = 5;
//nextstate
Point direction = {RECT_SIZE,0};
Point next_direction = direction;
Uint32 lastMoveTime = 0;
Uint32 lastShrinkTime = 0;
const Uint32 SHRINK_INTERVAL = 10000;
SDL_Rect startRect , speedRect , highScoreRect , quitRect , resumeRect , menuRect;
SDL_Rect slowRect , mediumRect , fastRect;
void generateFoods()
{
    foods.clear();
    int columns = SCREEN_WIDTH / RECT_SIZE;
    int rows    = SCREEN_HEIGHT / RECT_SIZE;
    srand(static_cast<unsigned> (time (nullptr))); //Notice
    while ((int) foods.size() < FOOD_COUNT )
    {
        Point f = { rand() % columns  * RECT_SIZE,
                    rand() % rows * RECT_SIZE };
        if(find(body.begin(), body.end(),f) == body.end() && find(foods.begin(), foods.end(),f) == foods.end())
        foods.push_back(f);

        }
}
 void ResetGame()
 {
     score = 0;
     body.clear();
     int startX = SCREEN_WIDTH/2;
     int startY = SCREEN_HEIGHT/2;
     body.push_back({startX,startY});
     direction = {RECT_SIZE,0};
     next_direction = direction;
     lastMoveTime = SDL_GetTicks();
     lastShrinkTime = lastMoveTime;
     generateFoods();
 }
 void drawSnake(const Point& currentDirection)
 {
     if (!gRenderer || body.empty()) return;
     SDL_Rect destRect = {0,0,RECT_SIZE,RECT_SIZE};
     if(gHeadTexture) {
        destRect.x = body[0].x;
        destRect.y = body[0].y;
        double angle = 0.0;
        if(currentDirection.x > 0) angle = 90.0;
        else if(currentDirection.x < 0) angle = 270.0;
        else if(currentDirection.y > 0 ) angle = 180.0;
        SDL_RenderCopyEx(gRenderer,gHeadTexture , NULL,&destRect , angle , NULL , SDL_FLIP_NONE);
     }
     else {
        destRect.x = body[0].x;
        destRect.y = body[0].y;
        SDL_SetRenderDrawColor(gRenderer,0,200,0,255);
        SDL_RenderFillRect(gRenderer,&destRect);
     }
     if(gBodyTexture) {
        for(int i=1; i < body.size() ; i++)
        {

            destRect.x = body[i].x;
            destRect.y = body[i].y;
            SDL_RenderCopy(gRenderer, gBodyTexture , NULL , &destRect);
        }
     }
     else {
        SDL_SetRenderDrawColor(gRenderer , 0 , 150 , 0 ,255);
        for(int i=1; i< body.size() ;i++)
        {

            destRect.x = body[i].x;
            destRect.y = body[i].y;
            SDL_RenderFillRect(gRenderer, &destRect);
        }
     }

 }
  void drawFoods()
  {
      if(!gRenderer) return;
      for(const auto& f: foods)
      {
          SDL_Rect dest = {f.x ,f.y , RECT_SIZE , RECT_SIZE};
          if(gFoodTexture) SDL_RenderCopy(gRenderer , gFoodTexture , nullptr ,&dest);
          else {
            SDL_SetRenderDrawColor (gRenderer,255,0,0,255);
            SDL_RenderFillRect (gRenderer , &dest);
          }
      }
  }
 void drawNewFoods()
  {   int size = (int) foods.size();
      if(!gRenderer) return;
      for(const auto& f: foods)
      {   if(size ==1 ) break;
          SDL_Rect dest = {f.x ,f.y , RECT_SIZE , RECT_SIZE};
          if(gNewFood) SDL_RenderCopy(gRenderer , gFoodTexture , nullptr ,&dest);
          else {
            SDL_SetRenderDrawColor (gRenderer,255,0,0,255);
            SDL_RenderFillRect (gRenderer , &dest);
            size--;
          }
      }
          SDL_Rect dest = {foods[foods.size() - 1].x ,foods[foods.size() - 1].y , RECT_SIZE , RECT_SIZE};
          if(gFoodTexture) SDL_RenderCopy(gRenderer , gNewFood , nullptr ,&dest);
  }
  void bomb ()
  {   int size = (int) foods.size();
      if(!gRenderer) return;
       for(const auto& f: foods)
      {   if(size ==1 ) break;
          SDL_Rect dest = {f.x ,f.y , RECT_SIZE , RECT_SIZE};
          if(gNewFood) SDL_RenderCopy(gRenderer , gFoodTexture , nullptr ,&dest);
          else {
            SDL_SetRenderDrawColor (gRenderer,255,0,0,255);
            SDL_RenderFillRect (gRenderer , &dest);
            size--;
          }
      }
          SDL_Rect dest = {foods[foods.size() - 1].x ,foods[foods.size() - 1].y , RECT_SIZE , RECT_SIZE};
          if(gFoodTexture) SDL_RenderCopy(gRenderer , gBestFood , nullptr ,&dest);


  }
void quitSDL() {
    if (gHeadTexture) SDL_DestroyTexture(gHeadTexture); gHeadTexture = nullptr;
    if (gBodyTexture) SDL_DestroyTexture(gBodyTexture); gBodyTexture = nullptr;
    if (gFoodTexture) SDL_DestroyTexture(gFoodTexture); gFoodTexture = nullptr;
    if (gBackgroundTexture) SDL_DestroyTexture(gBackgroundTexture); gBackgroundTexture = nullptr;
    if (gEat) Mix_FreeChunk(gEat); gEat = nullptr;
    if (gLose) Mix_FreeChunk(gLose); gLose = nullptr;
    if (gMusic) { if(Mix_PlayingMusic() || Mix_PausedMusic()) { Mix_HaltMusic(); } Mix_FreeMusic(gMusic); gMusic = nullptr; }
    if (gFont) TTF_CloseFont(gFont); gFont = nullptr;
    if (gRenderer) SDL_DestroyRenderer(gRenderer); gRenderer = nullptr;
    if (gWindow) SDL_DestroyWindow(gWindow); gWindow = nullptr;
    TTF_Quit(); Mix_CloseAudio(); Mix_Quit(); IMG_Quit(); SDL_Quit();
}
void RenderMenuScreen()
{   menuBackground = loadTexture("img/menu_bg.png",gRenderer);
    SDL_Rect fullScreenRect = { 10, 20, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(gRenderer, menuBackground, nullptr, &fullScreenRect);
    SDL_Color white = {255, 255, 255, 255}, yellow = {255, 255, 0, 255};
    int menuY = SCREEN_HEIGHT / 3, spacing = 60, buttonW = 200, buttonH = 50, buttonX = (SCREEN_WIDTH - buttonW) / 2;
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_Point mousePoint = {mouseX, mouseY};
    startRect = {buttonX, menuY, buttonW, buttonH};
    speedRect = {buttonX, menuY + spacing, buttonW, buttonH};
    highScoreRect = {buttonX, menuY + spacing * 2, buttonW, buttonH};
    quitRect = {buttonX, menuY + spacing * 3, buttonW, buttonH};
    SDL_Color startColor = SDL_PointInRect(&mousePoint, &startRect) ? yellow : white;
    RenderText("Start Game", startRect.x + 30, startRect.y + 10, startColor,gFont,gRenderer);
    SDL_Color speedColor = SDL_PointInRect(&mousePoint, &speedRect) ? yellow : white;
    RenderText("Change Speed", speedRect.x + 20, speedRect.y + 10, speedColor,gFont,gRenderer);
    SDL_Color hsColor = SDL_PointInRect(&mousePoint, &highScoreRect) ? yellow : white;
    RenderText("High Score: " + to_string(highScore), highScoreRect.x + 15, highScoreRect.y + 10, hsColor,gFont,gRenderer);
    SDL_Color quitColor = SDL_PointInRect(&mousePoint, &quitRect) ? yellow : white;
    RenderText("Quit Game", quitRect.x + 40, quitRect.y + 10, quitColor,gFont,gRenderer);
}
void RenderPauseScreen()
{
    SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gRenderer,0,0,0,150);
    SDL_Rect pauseOverlay = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
    SDL_RenderFillRect(gRenderer,&pauseOverlay);
    SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_NONE);
    SDL_Color white = {255, 255, 255, 255}, yellow = {255, 255, 0, 255};
    RenderText("PAUSED", (SCREEN_WIDTH - 100) / 2, SCREEN_HEIGHT / 4, white,gFont,gRenderer);
    int buttonW = 200, buttonH = 50, buttonX = (SCREEN_WIDTH - buttonW) / 2, buttonY = SCREEN_HEIGHT / 2 - buttonH, spacing = 60;
    resumeRect = {buttonX, buttonY, buttonW, buttonH}; menuRect = {buttonX, buttonY + spacing, buttonW, buttonH};
    int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
    SDL_Color resumeColor = SDL_PointInRect(&mousePoint, &resumeRect) ? yellow : white; RenderText("Resume (P/Esc)", resumeRect.x + 10, resumeRect.y + 10, resumeColor,gFont,gRenderer);
    SDL_Color menuColor = SDL_PointInRect(&mousePoint, &menuRect) ? yellow : white; RenderText("Back to Menu", menuRect.x + 25, menuRect.y + 10, menuColor,gFont,gRenderer);

}
void RenderHighScoreScreen() {
    SDL_Color white = {255, 255, 255, 255}, yellow = {255, 255, 0, 255};
    RenderText("HIGH SCORE", (SCREEN_WIDTH - 180) / 2, SCREEN_HEIGHT / 4, white,gFont,gRenderer);
    RenderText(to_string(highScore), (SCREEN_WIDTH - 40) / 2, SCREEN_HEIGHT / 2 - 30, white,gFont,gRenderer);
    int buttonW = 200, buttonH = 50, buttonX = (SCREEN_WIDTH - buttonW) / 2, buttonY = SCREEN_HEIGHT / 2 + 50;
    menuRect = {buttonX, buttonY, buttonW, buttonH};
    int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
    SDL_Color menuColor = SDL_PointInRect(&mousePoint, &menuRect) ? yellow : white;
    RenderText("Back to Menu (Esc)", menuRect.x + 5, menuRect.y + 10, menuColor,gFont,gRenderer);
}
void RenderSpeedScreen() {
    SDL_Color white = {255, 255, 255, 255}, yellow = {255, 255, 0, 255}, green = {0, 255, 0, 255};
    RenderText("SPEED", (SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 5, white,gFont,gRenderer);
    int buttonW = 150, buttonH = 40, buttonX = (SCREEN_WIDTH - buttonW) / 2, buttonY = SCREEN_HEIGHT / 3, spacing = 50;
    slowRect = {buttonX, buttonY + spacing * 0, buttonW, buttonH}; mediumRect = {buttonX, buttonY + spacing * 1, buttonW, buttonH};
    fastRect = {buttonX, buttonY + spacing * 2, buttonW, buttonH}; menuRect = {buttonX, buttonY + spacing * 3, buttonW, buttonH};
    int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
    SDL_Color slowColor = (gameSpeedDelay == 250) ? green : white; if (SDL_PointInRect(&mousePoint, &slowRect)) slowColor = yellow;
    SDL_Color mediumColor = (gameSpeedDelay == 150) ? green : white; if (SDL_PointInRect(&mousePoint, &mediumRect)) mediumColor = yellow;
    SDL_Color fastColor = (gameSpeedDelay == 80) ? green : white; if (SDL_PointInRect(&mousePoint, &fastRect)) fastColor = yellow;
    RenderText("Slow", slowRect.x + 50, slowRect.y + 5, slowColor,gFont,gRenderer); RenderText("Medium", mediumRect.x + 40, mediumRect.y + 5, mediumColor,gFont,gRenderer); RenderText("Fast", fastRect.x + 50, fastRect.y + 5, fastColor,gFont,gRenderer);
    SDL_Color menuColor = SDL_PointInRect(&mousePoint, &menuRect) ? yellow : white; RenderText("Back (Esc)", menuRect.x + 35, menuRect.y + 5, menuColor,gFont,gRenderer);
}
void RenderGameScreen(const Point& current_direction) {
    if (gBackgroundTexture != nullptr) { SDL_RenderCopy(gRenderer, gBackgroundTexture, NULL, NULL); }
    if(score % 5 == 0 && score > 0 ) drawNewFoods();
    else if(score > 0 && score % 3 == 0 ) bomb();
    else drawFoods();
    if (showDouble) {
    Uint32 now = SDL_GetTicks();
    SDL_Color yellow = {255, 255, 0, 255};
    RenderText("X2 DOUBLE!",
               SCREEN_WIDTH/3-80, SCREEN_HEIGHT/3-40,
               yellow, gBigFont, gRenderer);
    if (now - doubleStart > DOUBLE_DURATION) {
        showDouble = false;
    }
}
    drawSnake(current_direction);
    string scoreText = "Score: " + to_string(score); string highScoreText = "High: " + to_string(highScore);
    SDL_Color textColor = {255, 255, 255, 255};
    RenderText(scoreText, 10, 10, textColor,gFont,gRenderer);
    RenderText(highScoreText, SCREEN_WIDTH - 150, 10, textColor,gFont,gRenderer);
}
void HandleMenuInput(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
        if (SDL_PointInRect(&mousePoint, &startRect)) {
            ResetGame(); currentState = PLAYING;
            if (gMusic) { if (Mix_PausedMusic()) { Mix_ResumeMusic(); } else if (!Mix_PlayingMusic()) { Mix_PlayMusic(gMusic, -1); } }
        } else if (SDL_PointInRect(&mousePoint, &speedRect)) { currentState = SPEED_SELECTION; }
        else if (SDL_PointInRect(&mousePoint, &highScoreRect)) { currentState = HIGH_SCORE_DISPLAY; }
        else if (SDL_PointInRect(&mousePoint, &quitRect)) { gameIsRunning = false; }
    } else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
            ResetGame(); currentState = PLAYING;
            if (gMusic) { if (Mix_PausedMusic()) { Mix_ResumeMusic(); } else if (!Mix_PlayingMusic()) { Mix_PlayMusic(gMusic, -1); } }
        } else if (e.key.keysym.sym == SDLK_q) { gameIsRunning = false; }
    }
}

void HandlePlayingInput(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_w: case SDLK_UP: if (direction.y == 0) { next_direction = {0, -RECT_SIZE}; } break;
            case SDLK_s: case SDLK_DOWN: if (direction.y == 0) { next_direction = {0, RECT_SIZE}; } break;
            case SDLK_a: case SDLK_LEFT: if (direction.x == 0) { next_direction = {-RECT_SIZE, 0}; } break;
            case SDLK_d: case SDLK_RIGHT: if (direction.x == 0) { next_direction = {RECT_SIZE, 0}; } break;
            case SDLK_p: case SDLK_ESCAPE:
                currentState = PAUSED;
                if (Mix_PlayingMusic()) { Mix_PauseMusic(); }
                break;
        }
    }
}
void HandlePauseInput(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_p || e.key.keysym.sym == SDLK_ESCAPE)) {
        currentState = PLAYING;
        if (Mix_PausedMusic()) { Mix_ResumeMusic(); }
        lastMoveTime = SDL_GetTicks();
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
        if (SDL_PointInRect(&mousePoint, &resumeRect)) {
            currentState = PLAYING;
            if (Mix_PausedMusic()) { Mix_ResumeMusic(); }
            lastMoveTime = SDL_GetTicks();
        } else if (SDL_PointInRect(&mousePoint, &menuRect)) {
            highScore = max(highScore, score); SaveHighScore(highScore);
            currentState = MENU;
            if (Mix_PlayingMusic() || Mix_PausedMusic()) { Mix_HaltMusic(); }
        }
    }
}
void HandleReturnMenuInput(SDL_Event& e) {

    if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_ESCAPE)) { currentState = MENU; }
    else{
        if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX,mouseY;
        SDL_GetMouseState(&mouseX,&mouseY);
        SDL_Point mousePoint = {mouseX,mouseY};
        if(SDL_PointInRect(&mousePoint,&menuRect)) currentState = MENU;
    }

    }

}
void HandleSpeedInput(SDL_Event& e) {
        if (e.type == SDL_MOUSEBUTTONDOWN) { int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
        if (SDL_PointInRect(&mousePoint, &slowRect)) { gameSpeedDelay = 250; currentState = MENU; }
        else if (SDL_PointInRect(&mousePoint, &mediumRect)) { gameSpeedDelay = 150; currentState = MENU; }
        else if (SDL_PointInRect(&mousePoint, &fastRect)) { gameSpeedDelay = 80; currentState = MENU; }
        else if (SDL_PointInRect(&mousePoint, &menuRect)) { currentState = MENU; } }
}
void CoreGame() {
    gHeadTexture = loadTexture("img/head.png", gRenderer); gBodyTexture = loadTexture("img/body.png", gRenderer);
    gFoodTexture = loadTexture("img/food.png", gRenderer); gBackgroundTexture = loadTexture("img/SnakeGameBackGround.jpg", gRenderer);
    gNewFood = loadTexture("img/newfood.png",gRenderer);
    gBestFood = loadTexture("img/bestfood.png",gRenderer);
    LoadHighScore(highScore);
    SDL_Event e;
    while (gameIsRunning) {
        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { gameIsRunning = false; break; }
            switch (currentState) {
                case MENU: HandleMenuInput(e); break;
                case PLAYING: HandlePlayingInput(e); break;
                case PAUSED: HandlePauseInput(e); break;
                case HIGH_SCORE_DISPLAY: HandleReturnMenuInput(e); break;
                case SPEED_SELECTION: HandleSpeedInput(e); break;
            }
             if (!gameIsRunning) break;
        }
         if (!gameIsRunning) break;

        if (currentState == PLAYING) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastMoveTime >= (Uint32)gameSpeedDelay) {
                lastMoveTime = currentTime;
                direction = next_direction;
                if (body.empty()) {
                     ResetGame();
                     currentState = MENU;
                     continue;
                }
                Point head = body[0];
                Point next_head = {head.x + direction.x, head.y + direction.y};
                bool collisionDetected = false;
                if (next_head.x < 0 || next_head.x >= SCREEN_WIDTH || next_head.y < 0 || next_head.y >= SCREEN_HEIGHT) { collisionDetected = true; }
                if (!collisionDetected) { for (int i = 1; i < body.size(); ++i) { if (next_head == body[i]) { collisionDetected = true; break; } } }
                if (collisionDetected) {
                    if (gLose) play(gLose); if (Mix_PlayingMusic()) Mix_HaltMusic();
                    highScore = max(highScore, score); SaveHighScore(highScore);
                    currentState = MENU;
                } else {
                    body.insert(body.begin(), next_head);

auto it = find(foods.begin(), foods.end(), next_head);
if (it != foods.end()) {
     if ( !foods.empty() && next_head == foods.back() && score % 5 == 0 && score>0) {
        score *= 2;
        showDouble  = true;
        doubleStart = SDL_GetTicks();
    }
    if ( !foods.empty() && next_head == foods.back() && score % 3 == 0 && score>0) {
        if(gLose) play(gLose);
        currentState = MENU;
    }
    else score++;
    if (gEat) play(gEat);
    foods.erase(it);
    int columns = SCREEN_WIDTH / RECT_SIZE;
    int rows    = SCREEN_HEIGHT / RECT_SIZE;
    Point newFood;
    bool ok;
    do {
        ok = true;
        newFood = { (rand() % columns) * RECT_SIZE,
                    (rand() % rows)    * RECT_SIZE };
        if (find(body.begin(), body.end(), newFood) != body.end() ||
            find(foods.begin(), foods.end(), newFood) != foods.end())
            ok = false;
    } while (!ok);
    foods.push_back(newFood);
} else {
    body.pop_back();
}

                }
            }

        if (currentTime - lastShrinkTime >= 8000) {
        if (body.size() > 1) {
            body.pop_back();
        }
        lastShrinkTime = currentTime;
        if (body.size() <= 1) {
            if (gLose) play(gLose);
            highScore = max(highScore, score);
            SaveHighScore(highScore);
            currentState = MENU;
        }
    }
        }

        SDL_RenderClear(gRenderer);
        switch (currentState) {
            case MENU: RenderMenuScreen(); break;
            case PLAYING: RenderGameScreen(direction); break;
            case PAUSED: RenderGameScreen(direction); RenderPauseScreen(); break;
            case HIGH_SCORE_DISPLAY: RenderHighScoreScreen(); break;
            case SPEED_SELECTION: RenderSpeedScreen(); break;
        }

        SDL_RenderPresent(gRenderer);
        //Notice
        int frameTime = SDL_GetTicks() - frameStart;
        int desiredDelay = (currentState == PLAYING) ? gameSpeedDelay : 16;
        if (desiredDelay > frameTime) { SDL_Delay(desiredDelay - frameTime); }

    }

    if (gHeadTexture) SDL_DestroyTexture(gHeadTexture); gHeadTexture = nullptr; if (gBodyTexture) SDL_DestroyTexture(gBodyTexture); gBodyTexture = nullptr;
    if (gFoodTexture) SDL_DestroyTexture(gFoodTexture); gFoodTexture = nullptr; if (gBackgroundTexture) SDL_DestroyTexture(gBackgroundTexture); gBackgroundTexture = nullptr;
}
int main (int argc, char* argv[]) {
    srand(time(0));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { cerr << "SDL_Init Error: " << SDL_GetError() << endl; return 1; }
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG; if (!(IMG_Init(imgFlags) & imgFlags)) { cerr << "IMG_Init Error: " << IMG_GetError() << endl; SDL_Quit(); return 1; }
    if (TTF_Init() == -1) { cerr << "TTF_Init Error: " << TTF_GetError() << endl; IMG_Quit(); SDL_Quit(); return 1; }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { cerr << "Mix_OpenAudio Error: " << Mix_GetError() << endl; TTF_Quit(); IMG_Quit(); SDL_Quit(); return 1; }

    gWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!gWindow) { cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl; quitSDL(); return 1; }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer) { cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl; quitSDL(); return 1; }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    gBigFont = loadFont("font/timesbd.ttf",72);
    gFont = loadFont("font/timesbd.ttf", 36);
    if (!gFont) { cerr << "Failed to load font!" << endl; quitSDL(); return 1; }

    gMusic = loadMusic("audio/RunningAway.mp3");
    gEat = loadSound("audio/eating.wav");
    gLose = loadSound("audio/lose.wav");
    if (!gMusic) cerr << "Warning: Failed to load background music." << endl;
    if (!gEat) cerr << "Warning: Failed to load eat sound." << endl;
    if (!gLose) cerr << "Warning: Failed to load lose sound." << endl;
    LoadHighScore (highScore);
    cout<<highScore;
    CoreGame();
    quitSDL();
    return 0;
}

