#include <SDL.h>
#include <iostream>
#include <vector>
using namespace std;
const int SCREEN_HEIGHT=800;
const int SCREEN_WIDTH=600;
const string WINDOW_TITLE="SDL.setup";
void logSDLError(ostream& os,const string& msg,bool fatal)
{
    os << msg << "Error:" << SDL_GetError() << endl;
    if(fatal) {
    SDL_Quit();
    exit(1);
    }

}
void initSDL(SDL_Window* &window,SDL_Renderer* &renderer)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(cout,"SDL_Init",true);
    window = SDL_CreateWindow(WINDOW_TITLE.c_str(),SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED , SCREEN_WIDTH ,SCREEN_HEIGHT , SDL_WINDOW_SHOWN);
    if(window== nullptr ) logSDLError(cout,"CreateWindow",true);
    renderer = SDL_CreateRenderer (window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(renderer == nullptr) logSDLError(cout,"CreateRenderer",true);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"linear");
    SDL_RenderSetLogicalSize (renderer,SCREEN_WIDTH,SCREEN_HEIGHT);
}
void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
 {
 SDL_DestroyRenderer(renderer);
 SDL_DestroyWindow(window);
 SDL_Quit();
 }
 void waitUntilKeyPressed()
 {
    SDL_Event e;
 while (true) {
 if ( SDL_WaitEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
 return;
        SDL_Delay(100);
    }
 }
 struct Point{
     int x,y ;
 };
  vector <Point> body;
 void draw(SDL_Renderer* renderer)
 {

     const int RECT_SIZE=10;
     SDL_Rect rect;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
     rect.w=RECT_SIZE;
     rect.h=RECT_SIZE;
     for(auto p : body)
     {rect.x=p.x;
     rect.y=p.y;
     SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
     SDL_RenderFillRect(renderer, &rect);
     SDL_RenderPresent(renderer);
     }
 }
void render(SDL_Renderer* renderer,int x,int y)
 {

     const int RECT_SIZE=10;
     SDL_Rect rect;
     rect.x=x;
     rect.y=y;
     rect.w=RECT_SIZE;
     rect.h=RECT_SIZE;
     SDL_SetRenderDrawColor(renderer,255,0,0,0);
      SDL_RenderFillRect(renderer, &rect);
     SDL_RenderPresent(renderer);
 }
 Point randomFood()
{
    Point food;
    bool isFood=false;
    while(!isFood)
    {
         food.x = rand() % (SCREEN_WIDTH /10);
        food.y = rand() % (SCREEN_HEIGHT /10);
       isFood=true;
    }
    return food;
}
void Food(SDL_Renderer* renderer)
{   Point foody=randomFood();
    render(renderer,foody.x,foody.y);

}
 void move(SDL_Renderer* renderer,SDL_Window* window)
 {     int countMove=0,step=1;
      int y=SCREEN_HEIGHT/2;
      int x=SCREEN_WIDTH/2;
     body.push_back({x,y});
     Point head = body [0];
    Uint32 lastTime = SDL_GetTicks();
    const Uint32 delayTime = 100;
     SDL_Event e;
      Point food=randomFood();
     while(true)
     {   render(renderer,food.x,food.y);
         if(SDL_WaitEvent(&e)==0) SDL_Delay(100);
         else if(e.type == SDL_QUIT) break;
         else if ( e.type == SDL_KEYDOWN)
         {

             switch (e.key.keysym.sym)
             {
                 case SDLK_w : head.y-=10;
                 break;
                 case SDLK_a :  head.x-=10;
                 break;
                 case SDLK_s : head.y+=10;
                 break;
                 case SDLK_d : head.x+=10;
                 break;
             }
         }
       // render(renderer,food.x,food.y);
         countMove++;
         body.insert(body.begin(),head);
         if (abs(head.x - food.x) < 8 && abs(head.y - food.y) < 8) {
                food = randomFood();
                step++;
            } else if(countMove>step){
                body.pop_back();
            }
            //cout<<head.x<<" "<<SCREEN_WIDTH<<endl;
            if(head.x>SCREEN_WIDTH||head.y>SCREEN_HEIGHT||head.x<0||head.y<0) {quitSDL(window,renderer);}
            draw(renderer);

     }
 }
int main(int argc, char* argv[]) {

     SDL_Window* window;
      SDL_Renderer* renderer;
      initSDL(window,renderer);
       //waitUntilKeyPressed();
       bool quit=false;
           //Food(renderer);
           move(renderer,window);

   //  quitSDL(window,renderer);

    return 0;
}

