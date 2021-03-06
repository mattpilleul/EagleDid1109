/*******************************************************************************************
*
*   raylib game - Dr. Turtle & Mr. Gamera
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This game has been created using raylib 1.6 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <math.h>        // Used for sinf()

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define MAX_ENEMIES 10

typedef enum { TITLE = 0, GAMEPLAY, ENDING, WIN, CREDITS } GameScreen;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const int screenWidth = 1280;
const int screenHeight = 720;
    
Texture2D sky;
Texture2D mountains;
Texture2D sea;
Texture2D turtle;
Texture2D gamera;
Texture2D shark;
Texture2D orca;
Texture2D swhale;
Texture2D fish;
Texture2D gframe;
Texture2D ttower;

Font font;

Sound eat;
Sound die;
Sound growl;
Sound explode;

Music music;

// Define scrolling variables
int backScrolling = 0;
int seaScrolling = 0;

// Define current screen
GameScreen currentScreen = 0;

// Define player variables
int playerRail = 1;
Rectangle playerBounds;
bool gameraMode = false;

// Define enemies variables
Rectangle enemyBounds[MAX_ENEMIES];
int enemyRail[MAX_ENEMIES];
int enemyType[MAX_ENEMIES];
bool enemyActive[MAX_ENEMIES];
float enemySpeed = 10;

// Define Twin towers variables
Rectangle ttowerBounds;
bool ttowerActive;
float ttowerSpeed = 10;

// Define additional game variables
int score = 0;
float distance = 0.0f;
int hiscore = 0;
float hidistance = 0.0f;
int foodBar = 0;
int framesCounter = 0;

float timeCounter = 0;
    
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void);     // Update and Draw one frame

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    
    // Init window
    InitWindow(screenWidth, screenHeight, "Who Did 9/11 ?");
    
    // Initialize audio device
    InitAudioDevice();      
    
    // Load game resources: textures
    sky = LoadTexture("resources/sky.png");
    mountains = LoadTexture("resources/mountains.png");
    sea = LoadTexture("resources/sea.png");
    turtle = LoadTexture("resources/eagle.png");
    gamera = LoadTexture("resources/henric.png");
    shark = LoadTexture("resources/rafale.png");
    orca = LoadTexture("resources/drone.png");
    swhale = LoadTexture("resources/boeing777.png");
    fish = LoadTexture("resources/worm.png");
    gframe = LoadTexture("resources/gframe.png");
    ttower = LoadTexture("resources/tours.png");
    
    // Load game resources: fonts
    font = LoadFont("resources/komika.png");
    
    // Load game resources: sounds
    eat = LoadSound("resources/son_bouche_manger.wav");
    die = LoadSound("resources/AIE.wav");
    growl = LoadSound("resources/whatttt.wav");
    explode = LoadSound("resources/bruit_explosion.wav");
    
    SetSoundVolume(eat, 10);
    SetSoundVolume(explode, 5);
    SetSoundVolume(growl, 10);
    // Load music stream and start playing music
    music = LoadMusicStream("resources/speeding.ogg");
    PlayMusicStream(music);
    
    playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
    
    // Init enemies variables
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        // Define enemy type (all same probability)
        //enemyType[i] = GetRandomValue(0, 3);
    
        // Probability system for enemies type
        int enemyProb = GetRandomValue(0, 100);
        
        if (enemyProb < 30) enemyType[i] = 0;
        else if (enemyProb < 60) enemyType[i] = 1;
        else if (enemyProb < 90) enemyType[i] = 2;
        else enemyType[i] = 3;

        // define enemy rail
        enemyRail[i] = GetRandomValue(0, 4);

        // Make sure not two consecutive enemies in the same row
        if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);
        
        enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
        enemyActive[i] = false;
    }

    ttowerBounds = (Rectangle){ screenWidth + 14, 120 + 90, 100, screenHeight };
    ttowerActive = false;
    
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
    // Unload textures
    UnloadTexture(sky);
    UnloadTexture(mountains);
    UnloadTexture(sea);
    UnloadTexture(gframe);
    UnloadTexture(turtle);
    UnloadTexture(shark);
    UnloadTexture(orca);
    UnloadTexture(swhale);
    UnloadTexture(fish);
    UnloadTexture(gamera);
    UnloadTexture(ttower);
    
    // Unload font texture
    UnloadFont(font);
    
    // Unload sounds
    UnloadSound(eat);
    UnloadSound(die);
    UnloadSound(growl);
    UnloadSound(explode);
    
    UnloadMusicStream(music);   // Unload music
    CloseAudioDevice();         // Close audio device
    
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    UpdateMusicStream(music);   // Refill music stream buffers (if required)
        
    framesCounter++;

    timeCounter += 0.01;

    // Game screens management
    switch (currentScreen)
    {
        case TITLE:
        {
            // Sea scrolling
            seaScrolling -= 2;
            if (seaScrolling <= -screenWidth) seaScrolling = 0;
        
            // Press enter to change to gameplay screen
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = GAMEPLAY;
                framesCounter = 0;
            }
            
        } break;
        case GAMEPLAY:
        {
            // Background scrolling logic
            backScrolling--;
            if (backScrolling <= -screenWidth) backScrolling = 0; 
            
            // Sea scrolling logic
            seaScrolling -= (enemySpeed - 2);
            if (seaScrolling <= -screenWidth) seaScrolling = 0; 
        
            // Player movement logic
            if (IsKeyPressed(KEY_DOWN)) playerRail++;
            else if (IsKeyPressed(KEY_UP)) playerRail--;
            
            // Check player not out of rails
            if (playerRail > 4) playerRail = 4;
            else if (playerRail < 0) playerRail = 0;
        
            // Update player bounds
            playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
            
            // Enemies activation logic (every 40 frames)        
            if (framesCounter > 40)
            {
                for (int i = 0; i < MAX_ENEMIES && distance < 1105; i++)
                {
                    if (enemyActive[i] == false)
                    {
                        enemyActive[i] = true;
                        i = MAX_ENEMIES;
                    }
                }
                
                if (distance == 1109.0f)
                    ttowerActive = true;

                framesCounter = 0;
            }
            
            // Enemies logic
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemyActive[i])
                {
                    enemyBounds[i].x -= enemySpeed;
                }
                
                // Check enemies out of screen
                if (enemyBounds[i].x <= 0 - 128)
                {
                    enemyActive[i] = false;
                    enemyType[i] = GetRandomValue(0, 3);
                    enemyRail[i] = GetRandomValue(0, 4);
                    
                    // Make sure not two consecutive enemies in the same row
                    if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);
                    
                    enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                }
            }
            
            if (ttowerActive)
                ttowerBounds.x -= ttowerSpeed;

            if (!gameraMode) enemySpeed += 0.005;
            
            // Check collision player vs enemies
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemyActive[i])
                {
                    if (CheckCollisionRecs(playerBounds, enemyBounds[i]))
                    {
                        if (enemyType[i] < 3)   // Bad enemies
                        {
                            if (gameraMode)
                            {
                                if (enemyType[i] == 0) score += 50;
                                else if (enemyType[i] == 1) score += 150;
                                else if (enemyType[i] == 2) score += 300;
                                
                                foodBar += 15;
                            
                                enemyActive[i] = false;
                                
                                // After enemy deactivation, reset enemy parameters to be reused
                                enemyType[i] = GetRandomValue(0, 3);
                                enemyRail[i] = GetRandomValue(0, 4);
                                
                                // Make sure not two consecutive enemies in the same row
                                if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);
                                
                                enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                                
                                PlaySound(eat);
                            }
                            else
                            {
                                // Player die logic
                                PlaySound(die);
                            
                                currentScreen = WIN;
                                framesCounter = 0;
                                
                                // Save hiscore and hidistance for next game
                                if (score > hiscore) hiscore = score;
                                if (distance > hidistance) hidistance = distance;
                            }
                        }
                        else    // Sweet fish
                        {
                            enemyActive[i] = false;
                            enemyType[i] = GetRandomValue(0, 3);
                            enemyRail[i] = GetRandomValue(0, 4);
                            
                            // Make sure not two consecutive enemies in the same row
                            if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);
                            
                            enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                            
                            if (!gameraMode) foodBar += 80;
                            else foodBar += 25;
                            
                            score += 10;
                            
                            if (foodBar == 400)
                            {
                                gameraMode = true;
                                
                                PlaySound(growl);
                            }
                            
                            PlaySound(eat);
                        }
                    }
                }
            }

            if (ttowerActive) {
                if (CheckCollisionRecs(playerBounds, ttowerBounds)) {
                    PlaySound(explode);
                    currentScreen = WIN;
                    framesCounter = 0;

                    // Save hiscore and hidistance for next game
                    if (score > hiscore) hiscore = score;
                    if (distance > hidistance) hidistance = distance;
                }
            }
            
            // Gamera mode logic
            if (gameraMode)
            {
                foodBar--;
                
                if (foodBar <= 0) 
                {
                    gameraMode = false;
                    enemySpeed -= 2;
                    if (enemySpeed < 10) enemySpeed = 10;
                }
            }
    
            // Update distance counter
            if (distance < 1109)
                distance += 0.5f;
        
        } break;
        case ENDING:
        {
            // Press enter to play again
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = GAMEPLAY;
                
                // Reset player
                playerRail = 1;
                playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
                gameraMode = false;
                
                // Reset enemies data
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    int enemyProb = GetRandomValue(0, 100);
                    
                    if (enemyProb < 30) enemyType[i] = 0;
                    else if (enemyProb < 60) enemyType[i] = 1;
                    else if (enemyProb < 90) enemyType[i] = 2;
                    else enemyType[i] = 3;
                    
                    //enemyType[i] = GetRandomValue(0, 3);
                    enemyRail[i] = GetRandomValue(0, 4);

                    // Make sure not two consecutive enemies in the same row
                    if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);
                    
                    enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                    enemyActive[i] = false;
                }
                
                ttowerBounds = (Rectangle){ screenWidth + 14, 120 + 90, 100, screenHeight };
                ttowerActive = false;

                enemySpeed = 10;
                
                // Reset game variables
                score = 0;
                distance = 0.0;
                foodBar = 0;
                framesCounter = 0;
            }
            if (IsKeyPressed(KEY_C)) {
                currentScreen = CREDITS;
            }
  
        } break;
        case WIN:
        {
            // Press enter to play again
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = GAMEPLAY;
                
                // Reset player
                playerRail = 1;
                playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
                gameraMode = false;
                
                // Reset enemies data
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    int enemyProb = GetRandomValue(0, 100);
                    
                    if (enemyProb < 30) enemyType[i] = 0;
                    else if (enemyProb < 60) enemyType[i] = 1;
                    else if (enemyProb < 90) enemyType[i] = 2;
                    else enemyType[i] = 3;
                    
                    //enemyType[i] = GetRandomValue(0, 3);
                    enemyRail[i] = GetRandomValue(0, 4);

                    // Make sure not two consecutive enemies in the same row
                    if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);
                    
                    enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                    enemyActive[i] = false;
                }
                
                ttowerBounds = (Rectangle){ screenWidth + 14, 120 + 90, 100, screenHeight };
                ttowerActive = false;

                enemySpeed = 10;
                
                // Reset game variables
                score = 0;
                distance = 0.0;
                foodBar = 0;
                framesCounter = 0;
            }
            if (IsKeyPressed(KEY_C)) {
                currentScreen = CREDITS;
            }
  
        } break;
        case CREDITS:
        {
            if (IsKeyPressed(KEY_T)) {
                currentScreen = TITLE;

                playerRail = 1;
                playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
                gameraMode = false;
                
                // Reset enemies data
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    int enemyProb = GetRandomValue(0, 100);
                    
                    if (enemyProb < 30) enemyType[i] = 0;
                    else if (enemyProb < 60) enemyType[i] = 1;
                    else if (enemyProb < 90) enemyType[i] = 2;
                    else enemyType[i] = 3;
                    
                    //enemyType[i] = GetRandomValue(0, 3);
                    enemyRail[i] = GetRandomValue(0, 4);

                    // Make sure not two consecutive enemies in the same row
                    if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);
                    
                    enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                    enemyActive[i] = false;
                }
                
                ttowerBounds = (Rectangle){ screenWidth + 14, 120 + 90, 100, screenHeight };
                ttowerActive = false;

                enemySpeed = 10;
                
                // Reset game variables
                score = 0;
                distance = 0.0;
                foodBar = 0;
                framesCounter = 0;
            }
        } break;
        default: break;
    }
    //----------------------------------------------------------------------------------
    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    
        ClearBackground(RAYWHITE);
        
        // Draw background (common to all screens)
        DrawTexture(sky, 0, 0, WHITE);
        
        DrawTexture(mountains, backScrolling, 0, WHITE);
        DrawTexture(mountains, screenWidth + backScrolling, 0, WHITE);
        
        if (!gameraMode)
        {
            DrawTexture(sea, seaScrolling, 0, BEIGE);
            DrawTexture(sea, screenWidth + seaScrolling, 0, BEIGE);
        }
        else
        {
            DrawTexture(sea, seaScrolling, 0, BEIGE);
            DrawTexture(sea, screenWidth + seaScrolling, 0, BEIGE);
        }
        
        switch (currentScreen)
        {
            case TITLE:
            {
                // Draw title
                DrawTextEx(font, "WHO DID 9/11", (Vector2){ screenWidth/2 - 300, 220 }, 100, 1, RED);
                
                // Draw blinking text
                if ((framesCounter/30) % 2) DrawTextEx(font, "PRESS ENTER", (Vector2){ screenWidth/2 - 150, 480 }, font.baseSize, 1, WHITE);
            
            } break;
            case GAMEPLAY:
            {
                // Draw water lines
                for (int i = 0; i < 5; i++) DrawRectangle(0, i*120 + 120, screenWidth, 110, Fade(SKYBLUE, 0.1f));
                
                // Draw player
                if (!gameraMode) DrawTexture(turtle, playerBounds.x - 14, playerBounds.y - 14, WHITE);
                else DrawTexture(gamera, playerBounds.x - 64, playerBounds.y - 64, WHITE);
                
                // Draw player bounding box
                //if (!gameraMode) DrawRectangleRec(playerBounds, Fade(GREEN, 0.4f));
                //else DrawRectangleRec(playerBounds, Fade(ORANGE, 0.4f));
                
                // Draw enemies
                if (distance < 1109.0f) {
                    for (int i = 0; i < MAX_ENEMIES; i++)
                    {
                        if (enemyActive[i]) 
                        {
                            // Draw enemies
                            switch(enemyType[i])
                            {
                                case 0: DrawTexture(shark, enemyBounds[i].x - 14, enemyBounds[i].y - 14, WHITE); break;
                                case 1: DrawTexture(orca, enemyBounds[i].x - 14, enemyBounds[i].y - 14, WHITE); break;
                                case 2: DrawTexture(swhale, enemyBounds[i].x - 14, enemyBounds[i].y - 14, WHITE); break;
                                case 3: DrawTexture(fish, enemyBounds[i].x - 14, enemyBounds[i].y - 14, WHITE); break;
                                default: break;
                            }

                            // Draw enemies bounding boxes
                            /*
                            switch(enemyType[i])
                            {
                                case 0: DrawRectangleRec(enemyBounds[i], Fade(RED, 0.5f)); break;
                                case 1: DrawRectangleRec(enemyBounds[i], Fade(RED, 0.5f)); break;
                                case 2: DrawRectangleRec(enemyBounds[i], Fade(RED, 0.5f)); break;
                                case 3: DrawRectangleRec(enemyBounds[i], Fade(GREEN, 0.5f)); break;
                                default: break;
                            }
                            */
                        }
                    }
                }
                else
                    DrawTexture(ttower, ttowerBounds.x - 14, ttowerBounds.y - 14, WHITE);
                
                // Draw gameplay interface
                DrawRectangle(20, 20, 400, 40, Fade(GRAY, 0.4f));
                DrawRectangle(20, 20, foodBar, 40, ORANGE);
                DrawRectangleLines(20, 20, 400, 40, BLACK);
                
                DrawTextEx(font, TextFormat("SCORE: %04i", score), (Vector2){ screenWidth - 300, 20 }, font.baseSize, -2, ORANGE);
                DrawTextEx(font, TextFormat("DISTANCE: %04i", (int)distance), (Vector2){ 550, 20 }, font.baseSize, -2, ORANGE);
                
                if (gameraMode)
                {
                    DrawText("HENRIC MODE", 60, 22, 40, GRAY);
                    DrawTexture(gframe, 0, 0, Fade(WHITE, 0.5f));
                }
        
            } break;
            case ENDING:
            {
                // Draw a transparent black rectangle that covers all screen
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.4f));
            
                DrawTextEx(font, "GAME OVER", (Vector2){ 300, 160 }, font.baseSize*3, -2, MAROON);
                
                DrawTextEx(font, TextFormat("SCORE: %04i", score), (Vector2){ 680, 350 }, font.baseSize, -2, GOLD);
                DrawTextEx(font, TextFormat("DISTANCE: %04i", (int)distance), (Vector2){ 290, 350 }, font.baseSize, -2, GOLD);
                DrawTextEx(font, TextFormat("HISCORE: %04i", hiscore), (Vector2){ 665, 400 }, font.baseSize, -2, ORANGE);
                DrawTextEx(font, TextFormat("HIDISTANCE: %04i", (int)hidistance), (Vector2){ 270, 400 }, font.baseSize, -2, ORANGE);
                
                // Draw blinking text
                if ((framesCounter/30) % 2) DrawTextEx(font, "PRESS ENTER to REPLAY", (Vector2){ screenWidth/2 - 250, 520 }, font.baseSize, -2, LIGHTGRAY);
                DrawTextEx(font, "PRESS C to show CREDITS", (Vector2){ screenWidth/2 - 250, 580 }, font.baseSize, -2, GRAY);
                
            } break;
            case WIN:
            {
                // Draw a transparent black rectangle that covers all screen
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.4f));
                if (gameraMode)
                    DrawTextEx(font, "HENRIC DID 9/11", (Vector2){ 200, 160 }, font.baseSize*3, -2, MAROON);
                else
                    DrawTextEx(font, "EAGLE DID 9/11", (Vector2){ 220, 160 }, font.baseSize*3, -2, MAROON);
                
                DrawTextEx(font, TextFormat("SCORE: %04i", score), (Vector2){ 680, 350 }, font.baseSize, -2, GOLD);
                DrawTextEx(font, TextFormat("DISTANCE: %04i", (int)distance), (Vector2){ 290, 350 }, font.baseSize, -2, GOLD);
                DrawTextEx(font, TextFormat("HISCORE: %04i", hiscore), (Vector2){ 665, 400 }, font.baseSize, -2, ORANGE);
                DrawTextEx(font, TextFormat("HIDISTANCE: %04i", (int)hidistance), (Vector2){ 270, 400 }, font.baseSize, -2, ORANGE);
                
                // Draw blinking text
                if ((framesCounter/30) % 2) DrawTextEx(font, "PRESS ENTER to REPLAY", (Vector2){ screenWidth/2 - 250, 520 }, font.baseSize, -2, LIGHTGRAY);
                DrawTextEx(font, "PRESS C to show CREDITS", (Vector2){ screenWidth/2 - 250, 580 }, font.baseSize, -2, GRAY);
            } break;
            case CREDITS:
            {
                DrawTextEx(font, "TEAM:", (Vector2){ screenWidth/2 - 50, 120 }, font.baseSize, -2, ORANGE);
                DrawTextEx(font, "THIBAULT BARBE", (Vector2){ screenWidth/2 - 150, 200 }, font.baseSize, -2, ORANGE);
                DrawTextEx(font, "BAPTISTE PAUTONNIER", (Vector2){ screenWidth/2 - 150, 250 }, font.baseSize, -2, ORANGE);
                DrawTextEx(font, "MATTHIEU PILLEUL", (Vector2){ screenWidth/2 - 150, 300 }, font.baseSize, -2, ORANGE);
                DrawTextEx(font, "CLEMENT BUTET", (Vector2){ screenWidth/2 - 150, 350 }, font.baseSize, -2, ORANGE);
                DrawTextEx(font, "ANTOINE BOUSSION", (Vector2){ screenWidth/2 - 150, 400 }, font.baseSize, -2, ORANGE);
                if ((framesCounter/30) % 2) DrawTextEx(font, "PRESS T to go back to TITLE", (Vector2){ screenWidth/2 - 250, 520 }, font.baseSize, -2, LIGHTGRAY);

            } break;
            default: break;
        }

    EndDrawing();
    //----------------------------------------------------------------------------------
}
