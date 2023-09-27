#include "include/raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define JUAN_SPD 200
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 830
#define JUAN_WIDTH 25
#define JUAN_HEIGHT 20
#define VIRUS_WIDTH 32
#define VIRUS_HEIGHT 32
#define NUM_FRAMES 3

typedef struct {
    char name[4];
    int value;
} Score;

Vector2 juan = { SCREEN_WIDTH / 2 - 22, SCREEN_HEIGHT - 80};
Vector2 viruses[5][8];
Vector2 alcodrop[50];
Vector2 miniVirus[50];
int score = 0;
int marginL = 10, marginR = 600;
int direction = 1;
int virusCount = 40;
int juanHP = 3;
//int framesCounter = 0;
int playerCanShoot = 1;
int isInGame = 0;
int isInScores = 0;
int isTypingScore = 0;
int nameCount = 0;
char nameScore[4];
Score scores[5] = { 0 };

void updatePlayer(Sound*, float);
void updateBullets(Sound*, Sound*, float);
void updateEnemies(float);
void updateEnemiesBullets(Sound*, float);
void initGame();
void drawGame(Texture2D*, Texture2D*, Texture2D*);
void drawMainMenu();
void handleMainMenu(Sound*);

int main(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "UNINFECTED");
    SetTargetFPS(60);


    typedef enum { LOGO, TITLE } GameScreen;  //( LOGO, TITLE, GAMING, ENDING )
    GameScreen gameScreen = LOGO;
    int state = 0;
    float alpha = 0.0;
    int framesCounter = 0;
    bool animation = false;
    //framesCounter++;

    Texture2D background_Logo = LoadTexture("assets/textures/plainuninfected.png");
    Texture2D background_Title = LoadTexture("assets/textures/background1.png");
    Texture2D background_Title2 = LoadTexture("assets/textures/background2.png");

// Declaring assets and sounds
    Texture2D juanTexture = LoadTexture("assets/textures/player.png");
    Texture2D virusTexture = LoadTexture("assets/textures/virusr.png");
    Texture2D virusBullet = LoadTexture("assets/textures/VirusBullet.png");
    //Texture2D background = LoadTexture("assets/textures/bg2.png");
    Texture2D button = LoadTexture("assets/textures/playa.png"); // Load button texture

    InitAudioDevice();

    Sound juanSprayFx = LoadSound("assets/sounds/player_shoot.wav");
    Sound virusDieFx = LoadSound("assets/sounds/enemy_explosion.wav");
    Sound scoreFx = LoadSound("assets/sounds/score.wav");
    Sound juanHitFx = LoadSound("assets/sounds/player_explosion.wav");
    Sound uiFx = LoadSound("assets/sounds/ui_interact.wav");
    Sound fxButton = LoadSound("assets/sounds/player_shoot.wav");

    SetSoundVolume(juanSprayFx, 0.2);
    SetSoundVolume(virusDieFx, 0.1);
    SetSoundVolume(juanHitFx, 0.1);
    SetSoundVolume(uiFx, 0.2);
    float frameHeight = (float)button.height/NUM_FRAMES;
    Rectangle sourceRec = { 0, 0, (float)button.width, frameHeight };

    Rectangle btnBounds = { SCREEN_WIDTH/2.0f - button.width/2.0f, SCREEN_HEIGHT/2.0f - button.height/NUM_FRAMES/2.0f, (float)button.width, frameHeight };

    int btnState = 0;               // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
    bool btnAction = false;         // Button action should be activated

    Vector2 mousePoint = { 0.0f, 0.0f };

    initGame();

    while (!WindowShouldClose())
    {
       framesCounter++;

          switch(gameScreen)
            {
                case LOGO:
               {
                  if(state == 0)
                 {
                    if(alpha < 1.0) alpha += 0.05;
                    else state = 1;
                 }
                 else if(state == 1)
                 {
                     alpha = 1.0;
                     if(framesCounter % 160 == 0)
                     {
                         state = 2;
                     }
                 }
                 else if (state == 2)
                 {
                     if(alpha > 0.0) alpha -= 0.05;
                     else
                     {
                         gameScreen = TITLE;
                         state = 0;
                     }
                 }
               } break;
               case TITLE:
               {
                    if(state == 0)
                  {
                      if(alpha < 1.0) alpha += 0.05;
                      else state = 1;
                  }
                  else if(state == 1)
                  {
                      alpha = 1.0;
                      if((framesCounter % 5) == 0) animation = !animation;
                  }
               } break;
            }

        mousePoint = GetMousePosition();
        btnAction = false;

        // Check button state
        if (CheckCollisionPointRec(mousePoint, btnBounds))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnState = 2;
            else btnState = 1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) btnAction = true;
        }
        else btnState = 0;

        if (btnAction)
        {
            PlaySound(fxButton);

            isInGame = 1;
        }
        // Calculate button frame rectangle to draw depending on button state
        sourceRec.y = btnState*frameHeight;
        if (juanHP > 0 && virusCount > 0 && isInGame) {
            if (!playerCanShoot) framesCounter++;

            if (framesCounter == 60) {
                framesCounter = 0;
                playerCanShoot = 1;
            }

            updatePlayer(&juanSprayFx, GetFrameTime());
            updateBullets(&virusDieFx, &scoreFx, GetFrameTime());
            updateEnemies(GetFrameTime());
            updateEnemiesBullets(&juanHitFx, GetFrameTime());
        }
        BeginDrawing();
        ClearBackground(BLANK);

         switch(gameScreen)
            {
               case LOGO:
               {
                   switch(state)
                   {
                       case 0:
                       {
                           DrawTexture(background_Logo, 0, 0, Fade(BLACK, alpha));
                       } break;

                       case 1:
                       {
                           DrawTexture(background_Logo, 0, 0, WHITE);
                       } break;
                       case 2:
                       {
                           DrawTexture(background_Logo, 0, 0, Fade(BLACK, alpha));
                       } break;
                   }
               } break;
               case TITLE:
               {
                   switch(state)
                   {
                       case 0:
                       {
                           DrawTexture(background_Title, 0, 0, Fade(BLACK, alpha));
                       } break;
                       case 1:
                       {
                           if(animation)
                           {
                               DrawTexture(background_Title, 0, 0, WHITE);
                           }
                           else
                           {
                               DrawTexture(background_Title2, 0, 0, WHITE);
                           }
                            DrawText("UNINFECTED", SCREEN_WIDTH / 2 - MeasureText("UNINFECTED", 70) / 2, 30, 70, WHITE);
                            DrawText("JUAN VS. VIRUS", SCREEN_WIDTH / 2 + 170 - MeasureText("JUAN VS. VIRUS", 70) / 2, 95, 30, WHITE);
                            DrawTextureRec(button, sourceRec, (Vector2){ btnBounds.x, btnBounds.y }, WHITE);
                       } break;
                    }
                } break;
            }

        //DrawTexture(background, SCREEN_WIDTH/2 - background.width/2, SCREEN_HEIGHT/2 - background.height/2, WHITE);

        //DrawTextureRec(button, sourceRec, (Vector2){ btnBounds.x, btnBounds.y }, WHITE);// Draw button frame

        if (isInGame) {
            drawGame(&juanTexture, &virusTexture, &virusBullet);
        }else
        drawMainMenu();

        EndDrawing();
    }
    UnloadTexture(button);
    UnloadSound(fxButton);
    UnloadTexture(juanTexture);
    UnloadTexture(virusTexture);
    UnloadTexture(virusBullet);
    UnloadSound(juanSprayFx);
    UnloadSound(virusDieFx);
    UnloadSound(scoreFx);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
// function to store scores
void saveScore() {
    char scoresText[41];

    for (int i = 0; i < 5; ++i) {
        strcat(scoresText, TextFormat("%s%05i\n", scores[i].name, scores[i].value));
    }
    FILE* scoreFile = fopen("scores", "w");
    if (scoreFile == NULL) return;
    fputs(scoresText, scoreFile);
    fclose(scoreFile);
}
// function in typing names for score
void handleTypingScore(Sound* uiFx) {
    int key = GetCharPressed();

    if (key >= 32 && key <= 125 && nameCount < 3) {
        PlaySound(*uiFx);
        nameScore[nameCount] = (char) key;
        nameCount++;
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        PlaySound(*uiFx);
        nameCount--;
        if (nameCount < 0) nameCount = 0;
        nameScore[nameCount] = '\0';
    }
    if (IsKeyPressed(KEY_ENTER) && nameCount == 3) {

        PlaySound(*uiFx);

        int pos = -1;
        for (int i = 0; i < 5; ++i) {
            if (score > scores[i].value) {
                if (pos == -1) pos = i;
                else if (scores[i].value > scores[pos].value) pos = i;
            }
        }
        if (pos != -1) {
            Score temp[5] = { scores[0], scores[1], scores[2], scores[3], scores[4] };
            temp[pos].value = score;
            strcpy(temp[pos].name, nameScore);

            for (int i = pos + 1; i < 5; ++i) {temp[i] = scores[i - 1];}
            for (int i = 0; i < 5; ++i) {scores[i] = temp[i];}
        }
        saveScore();
        score = 0;
        juanHP = 3;
        virusCount = 40;
        nameCount = 0;
        for (int i = 0; i < 3; ++i) {
            nameScore[i] = '\0';
        }
        initGame();
    }
}
// function to sort scores highest to lowest
void sortScores() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4 - i; ++j) {
            if (scores[j].value < scores[j + 1].value) {
                Score temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
}

//  Logic of player
void updatePlayer(Sound* fx, float delta) {
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        juan.x -= JUAN_SPD * delta;
    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        juan.x += JUAN_SPD * delta;
    }

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)|| IsKeyPressed(KEY_UP)) {

        if (playerCanShoot) {
            playerCanShoot = 0;
            for (int i = 0; i < 100; i++) {
                if (alcodrop[i].x == -1 && alcodrop[i].y == -1) {
                    PlaySound(*fx);
                    alcodrop[i] = (Vector2) { juan.x + 22 - 2, juan.y - 2 };
                    break;
                }
            }
        }
    }
    if (juan.x + JUAN_WIDTH > SCREEN_WIDTH) {
        juan.x = SCREEN_WIDTH - JUAN_WIDTH;
    } else if (juan.x < 0) {juan.x = 0;}
}
// function and graphic of player bullet
void updateBullets(Sound* explosionFx, Sound* scoreFx, float delta) {
    for (int i = 0; i < 50; ++i) {
        if (alcodrop[i].x != -1) {
            for (int col = 0; col < 5; ++col) {
                for (int row = 0; row < 8; ++row) {
                    if (viruses[col][row].x != -1 && viruses[col][row].y != 1) {
                        if (CheckCollisionRecs((Rectangle) { alcodrop[i].x, alcodrop[i].y, 6, 6 }, (Rectangle) { viruses[col][row].x, viruses[col][row].y, VIRUS_WIDTH, VIRUS_HEIGHT })) {
                            PlaySound(*explosionFx);
                            viruses[col][row] = (Vector2) { -1, -1 };
                            alcodrop[i] = (Vector2) { -1, -1 };
                            virusCount--;
                            score += 100;
                            if (score % 1000 == 0) PlaySound(*scoreFx);
                        }
                    }
                }
            }
            if (alcodrop[i].y < 0) {alcodrop[i] = (Vector2) { -1, -1 };}
            if (alcodrop[i].x != -1) {alcodrop[i].y -= 400 * delta;}
        }
    }
}
// Logic of enemies
void updateEnemies(float delta) {
    Vector2 aux = { 0, 0 };
    Vector2* first = &aux;
    Vector2* last = &viruses[0][0];

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 8; ++j){
            if (viruses[i][j].x != -1) first = &viruses[i][j];
        }
    }
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 8; ++j) {
            Vector2* virus = &viruses[i][j];
            if (virus->x == -1) continue;

            if (virus->x < first->x) first = virus;
            else if (virus->x > last->x) last = virus;
        }
    }
    // to make the pack of enemies bounce on the edge of screen
    if (first->x <= marginL) direction = 1;
    else if (last->x >= marginR) direction = -1;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 8; ++j) {
            Vector2* virus = &viruses[i][j];
            if (virus->x == -1) continue;
            virus->x += direction * 20 * (40 / virusCount) * delta;
            if (i == 4) {
                if (GetRandomValue(1, 400) == 1) {
                    for (int k = 0; k < 50; ++k) {
                        if (miniVirus[k].x == -1 && miniVirus[k].y == -1) {
                            miniVirus[k] = (Vector2) { virus->x + 22 - 2, virus->y + 2};
                            break;
                        }
                    }
                }
            } else if (viruses[i + 1][j].x == -1) {
                if (GetRandomValue(1, 400) == 1) {
                    for (int k = 0; k < 50; ++k) {
                        if (miniVirus[k].x == -1 && miniVirus[k].y == -1) {
                            miniVirus[k] = (Vector2) { virus->x + 22 - 2, virus->y + 2};
                            break;
                        }
                    }
                }
            }
        }
    }
}
// function and graphics of enemy bullets
void updateEnemiesBullets(Sound* fx, float delta) {
    for (int i = 0; i < 50; ++i) {
        if (miniVirus[i].x != -1 && miniVirus[i].y != -1) {
            Vector2* alcodrop = &miniVirus[i];
            if (CheckCollisionRecs((Rectangle) { miniVirus[i].x, miniVirus[i].y}, (Rectangle) { juan.x, juan.y, JUAN_WIDTH, JUAN_HEIGHT })) {
                PlaySound(*fx);
                juanHP -= 1;
                miniVirus[i] = (Vector2) { -1, -1 };
            }
            if (alcodrop->y > SCREEN_HEIGHT) {*alcodrop = (Vector2) { -1, -1 };}
            if (alcodrop->x != -1) {alcodrop->y += 400 * delta;}
        }
    }
}
// Textures and Graphics
void drawGame(Texture2D* juanTexture, Texture2D* virusTexture, Texture2D* virusBullet) {
    Texture2D background = LoadTexture("assets/textures/bg.png");
    ClearBackground(WHITE);
    DrawTexture(background, SCREEN_WIDTH/2 - background.width/2, SCREEN_HEIGHT/2 - background.height/2, WHITE);

    if (juanHP > 0 && virusCount > 0) {
        DrawTextureV(*juanTexture, juan, WHITE);

        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (viruses[i][j].x != -1) {
                    if (i == 0 && i <= 2) {
                        DrawTextureV(*virusTexture, viruses[i][j], WHITE);
                    }else {
                        DrawTextureV(*virusTexture, viruses[i][j], WHITE);
                    }
                }
            }
        }
        for (int i = 0; i < 50; ++i) {
            if (alcodrop[i].x != -1) {
                DrawRectangle(alcodrop[i].x, alcodrop[i].y, 6, 6, WHITE);
            }
        }
        for (int i = 0; i < 50; ++i) {
            if (miniVirus[i].x != -1) {
                DrawTextureV(*virusBullet, miniVirus[i], WHITE);
            }
        }
        DrawText(TextFormat("SCORE: %05i", score), 10, SCREEN_HEIGHT - 25, 20, RAYWHITE);
    } else {
        isInGame = 0;
        isTypingScore = 1;
    }
    DrawFPS(SCREEN_WIDTH - 90, SCREEN_HEIGHT - 30);
    DrawText(TextFormat("HP: %d", juanHP), 10, SCREEN_HEIGHT - 75, 20, RAYWHITE);
    DrawText(TextFormat("Virus: %d", virusCount), 10, SCREEN_HEIGHT - 50, 20, RAYWHITE);
}
// Function for main menu with textures
// Need modification and functions for buttons
void drawMainMenu() {
    /*if (!isInGame) {
        //DrawText("UNINFECTED", SCREEN_WIDTH / 2 - MeasureText("UNINFECTED", 70) / 2, 30, 70, WHITE);
        //DrawText("JUAN VS. VIRUS", SCREEN_WIDTH / 2 + 170 - MeasureText("JUAN VS. VIRUS", 70) / 2, 95, 30, WHITE);

    }*/
}
void handleMainMenu(Sound* uiFx) {
}
void initGame() {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 8; ++j) {viruses[i][j] = (Vector2) { j * 80 + 20, i * 50 + 50 };}
    }
    for (int i = 0; i < 50; ++i) {alcodrop[i] = (Vector2) { -1, -1 };}

    for (int i = 0; i < 50; ++i) {miniVirus[i] = (Vector2) { -1, -1 };}
}
