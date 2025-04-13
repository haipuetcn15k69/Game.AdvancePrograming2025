#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "SDL_utils.h"
#include <SDL_mixer.h>
#include "SDL-Mix.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL_ttf.h>
#include "SDL_text.h"
using namespace std;
const int SCREEN_HEIGHT = 800;
const int SCREEN_WIDTH = 600;
const char* WINDOW_TITLE = "SDL Snake Game";
const int RECT_SIZE = 20;
SDL_Texture* gHeadTexture = nullptr;
SDL_Texture* gFoodTexture = nullptr;
SDL_Texture* gBodyTexture = nullptr;
SDL_Texture* gBackgroundTexture = nullptr;
void quitSDL(SDL_Window* window, SDL_Renderer* renderer) {
    if (gHeadTexture != nullptr) {
        SDL_DestroyTexture(gHeadTexture);
        gHeadTexture = nullptr;
    }
    if (gFoodTexture != nullptr) {
        SDL_DestroyTexture(gFoodTexture);
        gFoodTexture = nullptr;
    }
    if (gBodyTexture != nullptr) {
        SDL_DestroyTexture(gBodyTexture);
        gBodyTexture = nullptr;
    }
    if (gBackgroundTexture != nullptr) {
        SDL_DestroyTexture(gBackgroundTexture);
        gBackgroundTexture = nullptr;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    Mix_Quit();


}
struct Point {
    int x, y;
    bool operator==(const Point other) const {
        return x == other.x && y == other.y;
    }
};
vector<Point> body;
Point food;
void drawSnake(SDL_Renderer* renderer, const Point& currentDirection) {
    SDL_Rect destRect;
    destRect.w = RECT_SIZE;
    destRect.h = RECT_SIZE;
    if (!body.empty()) {
        destRect.x = body[0].x;
        destRect.y = body[0].y;

        // <<< THÊM VÀO: Tính toán góc xoay >>>
        double angle = 0.0; // Mặc định là 0 độ (hướng lên)
        if (currentDirection.x > 0) {        // Đi sang phải (RECT_SIZE, 0)
            angle = 90.0;
        } else if (currentDirection.x < 0) { // Đi sang trái (-RECT_SIZE, 0)
            angle = 270.0; // Hoặc -90.0
        } else if (currentDirection.y > 0) { // Đi xuống (0, RECT_SIZE)
            angle = 180.0;
        }
        SDL_RenderCopyEx(renderer, gHeadTexture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
    }

    for (int i = 1; i < body.size(); ++i) {
        destRect.x = body[i].x;
        destRect.y = body[i].y;
        SDL_RenderCopy(renderer, gBodyTexture, NULL, &destRect); // Thân không cần xoay (trừ khi bạn muốn hiệu ứng phức tạp hơn)
    }
}

// Vẽ thức ăn (Giữ nguyên - vẫn vẽ hình chữ nhật đỏ)
// Vẽ thức ăn bằng ảnh quả táo
void drawFood(SDL_Renderer* renderer) {
    // Kiểm tra xem texture thức ăn đã được tải chưa
    if (gFoodTexture == nullptr) {
        // Nếu chưa tải được, vẽ tạm hình chữ nhật đỏ để biết vị trí
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Food texture not loaded, drawing red rectangle instead.");
        SDL_Rect rect;
        rect.x = food.x;
        rect.y = food.y;
        rect.w = RECT_SIZE;
        rect.h = RECT_SIZE;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
        SDL_RenderFillRect(renderer, &rect);
        return; // Thoát khỏi hàm sau khi vẽ hình chữ nhật
    }

    // Nếu texture đã tải, tiến hành vẽ ảnh
    SDL_Rect destRect; // Đổi tên biến để tránh nhầm lẫn nếu cần
    destRect.x = food.x;
    destRect.y = food.y;
    destRect.w = RECT_SIZE; // Kích thước vẽ mong muốn
    destRect.h = RECT_SIZE; // Kích thước vẽ mong muốn

    // Vẽ texture thức ăn vào vị trí và kích thước đã xác định
    SDL_RenderCopy(renderer, gFoodTexture, NULL, &destRect);
}

// --- Logic Game ---
// Tạo thức ăn ở vị trí ngẫu nhiên, không trùng với thân rắn (Giữ nguyên)
void generateFood() {
    bool onBody; // <<< SỬA LỖI >>> Đổi tên biến để rõ nghĩa hơn
    do {
        onBody = false;
        // Tạo tọa độ trên lưới rồi nhân với kích thước ô
        int food_X = rand() % (SCREEN_WIDTH / RECT_SIZE);
        int food_Y = rand() % (SCREEN_HEIGHT / RECT_SIZE);
        food = {food_X * RECT_SIZE, food_Y * RECT_SIZE};

        // Kiểm tra xem thức ăn có nằm trên thân rắn không
        for (const auto& p : body) {
            if (food == p) { // Sử dụng toán tử == đã định nghĩa
                onBody = true;
                break;
            }
        }
    } while (onBody);
}

void CoreGame(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font, Mix_Music* gMusic,Mix_Chunk* gEat, Mix_Chunk *gLose ) {
    // --- Phần tải Texture giữ nguyên ---
    gHeadTexture = loadTexture("img\\head.png", renderer);
    gBodyTexture = loadTexture("img\\body.png", renderer);
    gFoodTexture = loadTexture("img\\food.png", renderer);
    gBackgroundTexture = loadTexture("img\\SnakeGameBackGround.jpg", renderer);

    // Kiểm tra tải Texture giữ nguyên
    if (gHeadTexture == nullptr || gBodyTexture == nullptr) {
         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "Texture Load Error",
                                 "Could not load snake textures (head.jpg or body.jpg). Check file paths.",
                                 window);
    }
     if (gBackgroundTexture == nullptr) {
         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
                                 "Texture Load Warning",
                                 "Could not load background texture (SnakeGameBackGround.jpg).",
                                 window);
    }
    // --- Kết thúc phần Texture ---


    // +++ BẮT ĐẦU CHỈNH SỬA ÂM THANH +++

    // 1. Bắt đầu phát nhạc nền MỘT LẦN trước vòng lặp game.
    //    Kiểm tra xem gMusic có hợp lệ không (tức là đã load thành công trước đó).
    if (gMusic != nullptr) {
        // Mix_PlayMusic trả về 0 nếu thành công, -1 nếu lỗi.
        // Tham số thứ 2 là số lần lặp (-1 = lặp vô hạn).
        if (Mix_PlayMusic(gMusic, -1) == -1) {
            // Ghi lại lỗi nếu không thể bắt đầu phát nhạc
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CoreGame: Could not play music! SDL_mixer Error: %s", Mix_GetError());
            // Game vẫn có thể tiếp tục mà không có nhạc.
        }
    } else {
        // Nếu gMusic là nullptr, có nghĩa là việc tải nhạc đã thất bại TRƯỚC KHI vào CoreGame.
        // Lỗi này đã được báo ở hàm main rồi, ở đây chỉ là không làm gì cả.
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "CoreGame: Music pointer (gMusic) is null, cannot play.");
    }

    body.clear();
    int startX = (SCREEN_WIDTH / RECT_SIZE / 2) * RECT_SIZE;
    int startY = (SCREEN_HEIGHT / RECT_SIZE / 2) * RECT_SIZE;
    body.push_back({startX, startY});
    generateFood();

    Point direction = {RECT_SIZE, 0};
    Point next_direction = direction;

    bool quit = false;
    bool gameOver = false;

    Uint32 lastMoveTime = SDL_GetTicks();
    const Uint32 moveInterval = 150;

    SDL_Event e;
    while (!quit && !gameOver) {
        int currentScore = body.size() - 1;
        string scoreText = "Score: " + to_string(currentScore);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                 switch (e.key.keysym.sym) {
                    case SDLK_w: case SDLK_UP:
                        if (direction.y == 0) { next_direction = {0, -RECT_SIZE}; }
                        break;
                    case SDLK_s: case SDLK_DOWN:
                        if (direction.y == 0) { next_direction = {0, RECT_SIZE}; }
                        break;
                    case SDLK_a: case SDLK_LEFT:
                         if (direction.x == 0) { next_direction = {-RECT_SIZE, 0}; }
                        break;
                    case SDLK_d: case SDLK_RIGHT:
                         if (direction.x == 0) { next_direction = {RECT_SIZE, 0}; }
                        break;
                    case SDLK_ESCAPE:
                        SDL_Color Pause_Color= {255,0,0};
                        SDL_Texture* MenuText = renderText("SPACE to Resume", font, Pause_Color,renderer);
                        renderTexture(MenuText, 140,350,renderer);
                        SDL_RenderPresent(renderer);
                        bool _Space = false;
                        // --- Tạm dừng xử lý âm thanh khi Pause (Tùy chọn) ---
                         if (Mix_PlayingMusic()) { // Kiểm tra nhạc có đang chơi không
                            Mix_PauseMusic();     // Tạm dừng nhạc
                         }
                        while(! _Space) {
                            while (SDL_PollEvent(&e)) {
                                if(e.key.keysym.sym == SDLK_SPACE) {
                                    _Space=true;
                                    break;
                                }
                                // Nếu người dùng Quit trong khi Pause
                                if (e.type == SDL_QUIT) {
                                    quit = true; // Đặt cờ quit
                                    _Space = true; // Thoát vòng lặp Pause
                                    break;
                                }
                            }
                            if (quit) break; // Thoát vòng lặp ngoài nếu đã quit
                        }
                        // --- Tiếp tục phát nhạc khi Resume (Tùy chọn) ---
                         if (Mix_PausedMusic()) { // Kiểm tra nhạc có đang tạm dừng không
                             Mix_ResumeMusic();   // Tiếp tục phát
                         }
                        // --------------------------------------------------
                        SDL_DestroyTexture(MenuText);
                        break;
                }
            }
        }
        if (quit) break; // Thoát vòng lặp chính nếu quit từ Pause
        // --- Kết thúc xử lý Input ---


        // --- Phần cập nhật Game State giữ nguyên ---
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastMoveTime >= moveInterval) {
            lastMoveTime = currentTime;
            direction = next_direction;
            Point head = body[0];
            Point next_head = {head.x + direction.x, head.y + direction.y};

            if (next_head.x < 0 || next_head.x >= SCREEN_WIDTH ||
                next_head.y < 0 || next_head.y >= SCREEN_HEIGHT) {
                gameOver = true;
                continue;
            }

            for (int i = 1; i < body.size(); ++i) {
                if (next_head == body[i]) {
                    gameOver = true;
                    break;
                }
            }
             if (gameOver) continue;

            body.insert(body.begin(), next_head);
            if (next_head == food) {
                play(gEat);
                SDL_Delay(10);
                generateFood();
                // --- Có thể thêm hiệu ứng âm thanh ăn mồi ở đây (dùng Mix_PlayChannel với Mix_Chunk) ---
                // Mix_PlayChannel(-1, gEatSound, 0); // Ví dụ
            } else {
                body.pop_back();
            }
        }
        // --- Kết thúc cập nhật Game State ---


        // --- Phần Vẽ màn hình giữ nguyên ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (gBackgroundTexture != nullptr) {
            SDL_RenderCopy(renderer, gBackgroundTexture, NULL, NULL);
        }

        SDL_Color scoreColor= {255,0,0};
        SDL_Texture* scoreTexture = renderText(scoreText.c_str(), font, scoreColor, renderer);
        renderTexture(scoreTexture, 10, 10, renderer);

        drawFood(renderer);
        drawSnake(renderer, direction);

        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(scoreTexture);
        // --- Kết thúc Vẽ màn hình ---

    } // Kết thúc vòng lặp game chính


    // +++ BẮT ĐẦU CHỈNH SỬA ÂM THANH (Khi Kết Thúc Game) +++

    // 2. Dừng nhạc khi game kết thúc (do quit hoặc gameOver).
    //    Việc này giúp dừng nhạc ngay lập tức thay vì chờ đến khi giải phóng.
    Mix_HaltMusic();

    // 3. KHÔNG giải phóng bộ nhớ nhạc (Mix_FreeMusic) ở đây.
    //    Việc giải phóng nên được thực hiện ở hàm dọn dẹp chính (quitSDL).

    // +++ KẾT THÚC CHỈNH SỬA ÂM THANH (Khi Kết Thúc Game) +++


    // --- Phần xử lý Game Over giữ nguyên ---
    if (gameOver) {
         // --- Có thể thêm hiệu ứng âm thanh thua cuộc ở đây ---
         // Mix_PlayChannel(-1, gGameOverSound, 0); // Ví dụ
        play(gLose);
        SDL_Color color = {255, 0, 0, 255}; // Alpha nên là 255
         SDL_Texture* _Text = renderText("Game Over", font, color,renderer);
         renderTexture(_Text, 180,375,renderer);
         SDL_RenderPresent(renderer);
         SDL_Delay(1000);
         SDL_DestroyTexture(_Text);
          if (gEat != nullptr) Mix_FreeChunk( gEat);
          if (gLose != nullptr) Mix_FreeChunk( gLose);
    }
    // --- Kết thúc Game Over ---
}
int main (int argc, char* argv[]) {
    srand(time(0)); // Khởi tạo bộ sinh số ngẫu nhiên MỘT LẦN DUY NHẤT
    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer = createRenderer(window,SCREEN_WIDTH, SCREEN_HEIGHT);
    TTF_Font* font = loadFont("font\\timesbd.ttf", 50);

    Mix_Music *gMusic = loadMusic("audio\\RunningAway.mp3");
    play(gMusic);
    Mix_Chunk *gEat = loadSound("audio\\eating.wav");
    Mix_Chunk *gLose = loadSound("audio\\lose.wav");
 //   play (gEat);
    CoreGame(renderer, window,font,gMusic,gEat,gLose); // Chạy vòng lặp game
    quitSDL(window, renderer); // Dọn dẹp tài nguyên SDL

    return 0;
}


