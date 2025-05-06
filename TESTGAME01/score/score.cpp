#include <SDL.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include "score.h"
 using namespace std;
void LoadHighScore (int &highScore)
{
    ifstream file("score/highScore.txt");
    if(file.is_open()) {file >> highScore ;
    file.close();
    }
    else highScore = 0;
}
void SaveHighScore (int &highScore)
{

    ofstream file("score/highScore.txt");
    if(file.is_open())
    {
        file << highScore;
        file.close();
    }
    else cerr << "Error : Not Save High Score" << "highScore.txt" << endl;
}
