/*******************************************************************************
* Title : Midterm
* Author: Ryan May
* Date  : 10/21/2020
* Course: CSC 222-301W
* Description: This program is a paddle game that allows a user to control a soccer player sprite. The
*     user uses the up and down arrow keys to move the sprite vertically on the right side of the screen.
*     The program also contains a target goal sprite that moves vertically from the top of screen to bottom
*     of screen on the left side of the game window.  Finally there is a soccer ball sprite that moves diagonally
*     across the game screen, and is initialized to start the game with a random direction, at a random spot on
*     the game screen.  The objective for the user is to hit the target goal with the ball 3 times to win.
*     At the same time, the user only has 3 lives, every time the ball passes the player paddle and hits the right
*     side of the game screen the player loses a life.  The player loses when they lose all 3 lives.  The program
*     detects collisions of sprites by comparing sprites' rectangular coordinates on the game screen to see if they
*     overlap, and the program determines how to manage both sprites when a collision happens.  The collisions are
*     managed in such a way that soccer ball sprite changes its angle when it bounces off target goal, player paddle,
*     top, bottom, and left side of game screen. This is done by randomizing x and y directional values of ball when these collisions
*     take place.  When the ball hits the right side of the screen and a player loses a life, the game screen is reset
*     and the target and player will be moved back to their default positions, while the ball will be given a random
*     starting position on the left side of the screen and a random and direction. If the user wants to quit at any time,
*     they can do so by pressing the escape key, or clicking the x on the game window.  When a game is completed without
*     quitting, a new screen is presented to game window to indicate that the player(user) won or that the player(user) lost.
*     The game also provides background music.
*
* Data Requirements:
*       Input:      Keyboard input of up and down directional keys.
*                   Keyboard input of escape key to quit
*                   Mouse input to click x out of game window
*
*       Graphics:   bombBackground.bmp
*                   soccerPlayer.bmp
*                   soccerNet.bmp
*                   soccerBall.bmp
*                   playerWins.bmp
*                   playerLoses.bmp
*
*       Audio:      theHappySong.wav
*
* Formulas: SEE prologues below
*
* BEGIN main
*    IF(Game is not initialized)
*       Kill the program
*    Call method to create all game images
*    Load background music
*    IF(Game music pointer points to null)
*       Print debug message
*    Play background music
*    WHILE(The game is running)
*       Declare old time integer and call function to get ticks
*       Draw the game screen
*       Update the game
*       Calculate frame time
*       IF(Frame time is less than frame delay constant)
*           Delay
*       END IF
*    END WHILE(End game loop)
*    Print debug
*    Release SDL and its resources
*    Return 0 (C++ needs a return)
* END main
********************************************************************************/
#include <SDL.h>
#include <SDL_mixer.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

//Window
SDL_Window* gameWindow = NULL;

//Renderer
SDL_Renderer* renderer;

//Surfaces
SDL_Surface* tempSurface;

//Textures
SDL_Texture* textureBackground = NULL;
SDL_Texture* textureBall = NULL;
SDL_Texture* texturePlayer = NULL;
SDL_Texture* textureGoal = NULL;
SDL_Texture* textureWin = NULL;
SDL_Texture* textureLose = NULL;

//Rectangles
SDL_Rect rectBackground;    //Background rectangle
SDL_Rect rectBall;          //Soccer ball rectangle
SDL_Rect rectPlayer;        //Soccer player paddle rectangle
SDL_Rect rectGoal;          //Soccer target goal rectangle
SDL_Rect rectPlayerLose;    //Soccer player paddle rectangle
SDL_Rect rectPlayerWin;     //Soccer target goal rectangle

//Music
Mix_Music *GameMusic = NULL;

//Game constants
//Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
//Player dimensions
const int PLAYER_WIDTH = 30;
const int PLAYER_HEIGHT = 100;
//Target goal dimensions
const int GOAL_WIDTH = 50;
const int GOAL_HEIGHT = 150;
//Soccer ball dimensions
const int BALL_WIDTH = 30;
const int BALL_HEIGHT = 30;
//Constant x positions of player and goal
const int PLAYER_X = SCREEN_WIDTH - PLAYER_WIDTH*2;
const int GOAL_X = 0;
//Frame speed and delay
const int FPS = 30;
const int FRAME_DELAY = 1000/FPS;
//Sprite speeds
const int PLAYER_SPEED = 10;
const int ENEMY_SPEED = 7;
const int BALL_MAX_SPEED = 10;

//Game Variables
//Player score and lives remaining
int PlayerScore;
int LivesLeft;

//Directional speeds of soccer ball sprite
int xDirection = 10;
int yDirection = 10;

//Y-directional speed of target goal sprite
int yGoalDir = 10;

//Function Prototypes
bool ProgramIsRunning();
void FillRect(SDL_Rect &rect, int x, int y, int width, int height);
SDL_Surface* loadImage(const char* path);
SDL_Texture* loadTexture(SDL_Surface* surface);
SDL_Surface* setTransparentColor (SDL_Surface* surface, Uint8 r, Uint8 g, Uint8 b);
void CloseShop();SDL_Surface* LoadImage(char* fileName);
void DrawImage(SDL_Surface* image, SDL_Surface* destSurface, int x, int y);
bool ProgramIsRunning();
bool RectsOverlap(SDL_Rect rect1, SDL_Rect rect2);
bool InitSDL();
void ResetGame();
bool InitGame();
void UpdatePlayer();
void UpdateBall();
void RunGame();
void DrawGame();
void CreateGameImages();
void UpdateGoal();


int main(int argc, char *argv[])
{
    //IF(Game is not initialized)
    if(!InitGame())
    {
        //Kill the program
        CloseShop();
    }

    //Call method to create all game images
    CreateGameImages();

    //Load background music
    GameMusic = Mix_LoadMUS("audio/theHappySong.wav");

    //IF(Game music pointer points to null)
    if(GameMusic == NULL)
        //Print debug message
        printf("Game music did not load.");

    //Play background music
    Mix_PlayMusic(GameMusic, -1);

    //WHILE(The game is running)
    while(ProgramIsRunning())
    {
        //Declare old time integer and call function to get ticks
        long int oldTime = SDL_GetTicks();  //We will use this later to see how long it took to update the frame

        //Draw the game screen
        DrawGame();
        //Update the game
        RunGame();

        //Calculate frame time
        int frameTime = SDL_GetTicks() - oldTime;

        //IF(Frame time is less than frame delay constant)
        if(frameTime < FRAME_DELAY)
            //Delay
            SDL_Delay(FRAME_DELAY - frameTime);

    }//end game loop

    //Print debug
    printf("Be quirky enough to dream it.  Be crazy enough to do it.  ~Kef Amaya");
    //Release SDL and its resources
    CloseShop();
    //Return 0
    return 0;
}//END main

//Functions
SDL_Surface* loadImage(const char* path)
{
    //Load image at specified path
    tempSurface = SDL_LoadBMP(path);

    if( tempSurface == NULL )
    {
        printf("Unable to load image at path: %s\n", path);
    }
    return tempSurface;
}//END loadImage

SDL_Texture* loadTexture(SDL_Surface* tempSurface)
{
    //texture
    SDL_Texture* newTexture = NULL;
    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    if( newTexture == NULL )
    {
        printf("Unable to create texture");
    }
    //Get rid of the surface
    SDL_FreeSurface( tempSurface );
    return newTexture;
}//END loadTexture

void FillRect(SDL_Rect &rect, int x, int y, int width, int height)
{
    //Initialize the rectangle
    rect.x = x;         //initial x position of upper left corner
    rect.y = y;         //initial y position of upper left corner
    rect.w = width;     //width of rectangle
    rect.h = height;    //height of rectangle
}//END FillRect

/****************************************************
    Added 2 textures to this function to destroy.
****************************************************/
void CloseShop()
{
    //Destroy all objects
    SDL_DestroyTexture(textureBackground);
    SDL_DestroyTexture(textureBall);
    SDL_DestroyTexture(texturePlayer);
    SDL_DestroyTexture(textureGoal);
    SDL_DestroyTexture(textureWin);
    SDL_DestroyTexture(textureLose);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gameWindow);
    Mix_FreeMusic(GameMusic);
    SDL_Quit(); //Quit the program
}//End Close Shop

SDL_Surface* setTransparentColor (SDL_Surface* surface, Uint8 r, Uint8 g, Uint8 b)
{
    //create the transparent color (pink in this case) and put in colorKey
    Uint32 colorKey;
    colorKey = SDL_MapRGB(surface->format, r, g, b);
    //set the colorKey color as the transparent one in the image
    SDL_SetColorKey(surface, SDL_TRUE, colorKey);
    return surface;
}//End setTransparent Color

/****************************************************
    This function operates the same but now also
        returns false when the escape key is pressed
*****************************************************/
bool ProgramIsRunning()
{
    //Local constants
    //Init constant pointer to point to keyboard's current state
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    //Local variables
    SDL_Event event;
    bool running = true;

    /***************************************************/

    while(SDL_PollEvent(&event))
    {
        //IF(User clicks x on game window)
        if(event.type == SDL_QUIT)
            //Set running to false to end game
            running = false;
        //IF(User presses escape key)
        if(keys[SDL_SCANCODE_ESCAPE])
            //Set running to false to end game
            running = false;
    }

    //Return boolean indicating if program is running or ended
    return running;
}//END ProgramIsRunning

/*********************************************
    Adjusted this method to use only 1 return statement
        Still operates the same way.
*********************************************/
bool RectsOverlap(SDL_Rect rect1, SDL_Rect rect2)
{
    bool overlap = true;

    if(rect1.x >= rect2.x+rect2.w)
        overlap = false;
    if(rect1.y >= rect2.y+rect2.h)
        overlap = false;
    if(rect2.x >= rect1.x+rect1.w)
        overlap = false;
    if(rect2.y >= rect1.y+rect1.h)
        overlap = false;
    return overlap;
}//END RectsOverlap

bool InitSDL()
{
     //Initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL failed to initialize!\n");
        SDL_Quit();
    }
    //Init audio subsystem
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) == -1)
    {
        printf("Open Audio failed to initialize!\n");
        return false;
    }
    return true;
}//END InitSDL

/**********************************************************************************
* Function : ResetGame
* Author   : Ryan May
* Description: This function is called when the ball sprite passes the right side of the game
*       screen. This function resets the game sprites to new positions.  The player and target
*       are reset to default positions halfway down the game screen vertically.  The ball is
*       positioned at a random location on the left side of the game screen. The ball will also
*       be given a random starting direction.  The function does make sure that the ball is not
*       randomly started on the target. The y direction of the target goal will also be randomized.
*
* Parameters: none
* Return    : void
*
* BEGIN ResetGame
*    Create random reset y position for ball
*    Create random reset x pos for ball on left side of screen(NOT on target)
*    Position the player paddle rectangle
*    Position the target goal rectangle
*    Position the ball rectangle
*    Reset x and y direction of ball
*    Give ball 50/50 chance of going left or right
*    Give ball 50/50 chance of going up or down
*    Give goal 50/50 chance of going up or down
* END ResetGame
************************************************************************************/
void ResetGame()
{
    //Create random reset y position for ball
    int yStart = rand()%SCREEN_HEIGHT;

    //Create random reset x pos for ball on left side of screen(NOT on target)
    int xStart = rand()%(SCREEN_WIDTH/2-100) + 100;

    //Position the player paddle rectangle
    rectPlayer.x = PLAYER_X;
    rectPlayer.y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
    rectPlayer.w = PLAYER_WIDTH;
    rectPlayer.h = PLAYER_HEIGHT;

    //Position the target goal rectangle
    rectGoal.x = GOAL_X;
    rectGoal.y = SCREEN_HEIGHT/2 - GOAL_HEIGHT/2;
    rectGoal.w = GOAL_WIDTH;
    rectGoal.h = GOAL_HEIGHT;

    //Position the ball rectangle
    rectBall.x = xStart;
    rectBall.y = yStart;
    rectBall.w = BALL_WIDTH;
    rectBall.h = BALL_HEIGHT;

    //Reset x and y direction of ball
    xDirection = 5;
    yDirection = 5;

    //Give ball 50/50 chance of going left or right
    if(rand()%2 == 0)
        xDirection *= -1;
    //Give ball 50/50 chance of going up or down
    if(rand()%2 == 0)
        yDirection *= -1;
    //Give goal 50/50 chance of going up or down
    if(rand()%2 == 0)
        yGoalDir *= -1;
}//END ResetGame

/***************************************************
    Updated the game variables in this function.
***************************************************/
bool InitGame()
{
    //Init SDL
    if(!InitSDL())
        return false;

    //Initiatialize game variables
    PlayerScore = 0;        //Number of goals a player has scored
    LivesLeft = 3;          //Amount of player lives left before a game over

    return true;
}//END InitGame

/**********************************************************************************
* Function : updatePlayer
* Author   : Ryan May
* Description: This function moves the player paddle sprite and keeps paddle within
*    the borders of a game window.  The function creates a pointer 'keys' to point to the
*    current keyboard state. The player paddle is moved via the up and down directional.
*    keys. When either of those keys are pressed the function updates the player accordingly.
*
* Parameters: none
* Return    : void
*
* BEGIN updatePlayer
*    Declare constant pointer that points to the keyboard's current state
*    IF(Up arrow is pressed)
*       Move player up
*    IF(Down arrow pressed)
*       Move player down
*    IF(Player hits top of the screen)
*       Adjust y position of player to keep on screen
*    IF(Player hits bottom of the screen)
*       Adjust y position of player to keep on screen
* END updatePlayer
************************************************************************************/
void UpdatePlayer()
{
    //Constant pointer that points to the keyboard's current state
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

     //IF(Up arrow is pressed)
    if(keys[SDL_SCANCODE_UP])
        //Move the player up
        rectPlayer.y -= PLAYER_SPEED;

    //IF(Down arrow is pressed)
    if(keys[SDL_SCANCODE_DOWN])
        //Move the player down
        rectPlayer.y += PLAYER_SPEED;

    //IF(Player hits top of the screen)
    if(rectPlayer.y < 0)
        //Adjust y position of player to keep on screen
        rectPlayer.y = 0;

    //IF(Player hits bottom of the screen)
    if(rectPlayer.y > SCREEN_HEIGHT-rectPlayer.h)
        //Adjust y position of player to keep on screen
        rectPlayer.y = SCREEN_HEIGHT-rectPlayer.h;
}//END UpdatePlayer

/**********************************************************************************
* Function : updateBall
* Author   : Ryan May
* Description: This function is moves the soccer ball sprite in a diagonal direction.
*    The function checks to see if the soccer ball sprite collides with other game
*    sprites while it moves through the game screen.  The function also checks to see
*    if sprite is colliding with the boundaries of the game screen.  The ball is designed
*    to bounce off of the player paddle sprite as well as the target goal sprite.  The ball
*    will also bounce off the top, bottom, and left borders of the game screen. When the ball
*    collides with another sprite and bounces, the y directional speed and x directional speed
*    of the ball is changed in a way that makes the ball 'bounce' at a new random angle.
*    When the ball bounces off the target goal sprite, the players score will be incremented.
*    If the soccer ball sprite reaches the right side of the game screen, past the player paddle,
*    the players lives will be decremented, and the sprites/game screen will be reset.
*
* Parameters: none
* Return    : void
*
* BEGIN updateBall
     Update x and y position
     IF(The ball hits the goal)
        Increment player score
        Change y direction of ball
        Send ball back towards player at a random x directional speed
        Change y direction of goal to prevent ball getting stuck in goal
     IF(the ball hits the player)
        Send ball back towards goal at a random speed
        Change y direction of ball
     IF(Ball hits top of screen)
        Send ball in opposite y direction at random y directional speed
     IF(Ball hits bottom of screen)
        Send ball in opposite y direction at random y directional speed
     IF(The ball passes the player paddle on right side of screen)
        Decrement player lives
        Reset game
     IF(Ball hits left side of the screen)
        Set ball x position to 0
        Send ball in opposite x direction at random x directional speed
* END updateBall
************************************************************************************/
void UpdateBall()
{
    //Update x and y position
    rectBall.x += xDirection;
    rectBall.y += yDirection;

    //IF(The ball hits the goal)
    if(RectsOverlap(rectBall, rectGoal))
    {
        //Increment player score
        PlayerScore ++;
        //Change y direction of ball
        yDirection *= -1;
        //Send ball back towards player at a random x directional speed
        xDirection = (rand()%BALL_MAX_SPEED + 5);
        //Change y direction of goal to prevent ball getting stuck in goal
        yGoalDir *= -1;
    }
    //IF(the ball hits the player)
    if(RectsOverlap(rectBall, rectPlayer))
    {
        //Send ball back towards goal at a random speed
        xDirection = (rand()%BALL_MAX_SPEED + 5) * -1;
        //Change y direction of ball
        yDirection *= -1;
    }
    //IF(Ball hits top of screen)
    if(rectBall.y < 0)
    {
        //Send ball in opposite y direction at random y directional speed
        rectBall.y = 0;
        yDirection = rand()%BALL_MAX_SPEED + 5;
    }
    //IF(Ball hits bottom of screen)
    if(rectBall.y > SCREEN_HEIGHT - rectBall.h)
    {
        //Send ball in opposite y direction at random y directional speed
        rectBall.y = SCREEN_HEIGHT - rectBall.h;
        yDirection = (rand()%BALL_MAX_SPEED + 5) * -1;
    }
    //IF(The ball passes the player paddle on right side of screen)
    if(rectBall.x > SCREEN_WIDTH)
    {
        //Decrement player lives
        LivesLeft--;
        //Reset game
        ResetGame();
    }
    //IF(Ball hits left side of the screen)
    if(rectBall.x < 0)
    {
        //Set ball x position to 0
        rectBall.x = 0;
        //Send ball in opposite x direction at random x directional speed
        xDirection = rand()%BALL_MAX_SPEED + 5;
    }
}//END UpdateBall

void RunGame()
{
    UpdatePlayer();     //Update the players's paddle
    UpdateGoal();       //Update the target goal
    UpdateBall();       //Update position of ball
}//END RunGame

/**********************************************************************************
* Function : DrawGame
* Author   : Ryan May
* Description: This function draws/presents the game images on to the game window.
*    The function provides the game window with a proper window title caption depending on the
*    state of the game.  The function determines whether the game is over, or if
*    the game is still going, and then presents the proper game images accordingly.  When the
*    game is over, a new background will be displayed to user, indicating whether they won or
*    lost the game.
*
* Parameters: none
* Return    : void
*
* BEGIN DrawGame
*    Set up a buffer to store window caption in
*    Store the updated score and lives for window caption
*    Print the scores in the game window caption
*    Clear the window
*    IF(Game is not over)
*        Copy the background;
*        Copy ball
*        Copy player paddle
*        Copy target goal
*        Display the game screen with updated position of ball and paddles
*        delay a bit
*    ELSE IF(Player has won)
*        Update game window title to display game results
*        Print the message in game window title
*        Copy the player wins background
*        Present player wins background
*        delay a bit
*    ELSE IF(Player has lost)
*        Update game window title to display game results
*        Print the message in game window title
*        Copy the player loses background
*        Present player loses background
*        delay a bit
*    END IF
* END DrawGame
************************************************************************************/
void DrawGame()
{
    //Set up a buffer to store window caption in
    char scores[64];

    //Store the updated score and lives for window caption
    sprintf(scores, "Goals Scored: %d, Lives Left: %d", PlayerScore, LivesLeft);

    //Print the scores in the game window caption
    SDL_SetWindowTitle(gameWindow, scores);

    //Clear the window
    SDL_RenderClear(renderer);

    //IF(Game is not over)
    if(PlayerScore < 3 && LivesLeft >0)
    {
        //Copy the background;
        SDL_RenderCopy(renderer, textureBackground, NULL, &rectBackground);
        //Copy ball
        SDL_RenderCopy(renderer, textureBall, NULL, &rectBall);
        //Copy player paddle
        SDL_RenderCopy(renderer, texturePlayer, NULL, &rectPlayer);
        //Copy target goal
        SDL_RenderCopy(renderer, textureGoal, NULL, &rectGoal);
        //Display the game screen with updated position of ball and paddles
        SDL_RenderPresent(renderer);
        SDL_Delay(10);  //delay a bit
    }
    //ELSE IF(Player has won)
    else if(PlayerScore >= 3)
    {
        //Update game window title to display game results
        sprintf(scores, "PLAYER WINS");
        //Print the message in game window title
        SDL_SetWindowTitle(gameWindow, scores);
        //Copy the player wins background
        SDL_RenderCopy(renderer, textureWin, NULL, &rectPlayerWin);
        //Present player wins background
        SDL_RenderPresent(renderer);
        SDL_Delay(100);  //delay a bit
    }
    //ELSE IF(Player has lost)
    else if(LivesLeft < 1)
    {
        //Update game window title to display game results
        sprintf(scores, "PLAYER LOSES");
        //Print the message in game window title
        SDL_SetWindowTitle(gameWindow, scores);
        //Copy the player loses background
        SDL_RenderCopy(renderer, textureLose, NULL, &rectPlayerLose);
        //Present player loses background
        SDL_RenderPresent(renderer);
        SDL_Delay(100);  //delay a bit
    }
    //END IF
}//END DrawGame

/***********************************************************
* Function : CreateGameImages
* Author   : Ryan May
* Description: This function operates the same, note that there are 2 extra
*        game images that provide new backgrounds when the player
*        either wins or loses. Line by line documentation is included
*        with each new line. This function creates a game window & a
*        renderer for the window. Bitmap images are loaded into surfaces
*        and textures are then created for each image using said surfaces.
*        Rectangles are then filled for each image, using their
*        dimensions and starting positions.
*
* BEGIN CreateGameImages
*    Init x position of ball between 200-600(So not too close to left/right side)
*    Init y position of ball to any random y position within game screen
*    Create a window
*    Create a renderer for the window
*    Load background
*    Create texture
*    Create a rectangle at position 0, 0 for background
*    Load Ball bitmap
*    Set magenta as transparency color
*    Create texture
*    Create a rectangle at position 0, 0 for Ball
*    Load Player Paddle bitmap
*    Set magenta as transparency color
*    Create texture
*    Initialize rectangle fields for player paddle
*    Create a rectangle halfway down screen on right side for Player
*    Load target goal bitmap
*    Set black as transparency color
*    Create texture
*    Position the target goal
*    Create a rectangle halfway down screen on left side for goal
*    Load player wins background
*    Create texture
*    Create a rectangle at position 0, 0 for player wins background
*    Load player loses background
*    Create texture
*    Create a rectangle at position 0, 0 for player loses background
*    Give ball 50/50 chance of going left or right
*    Give ball 50/50 chance of going up or down
* END CreateGameImages
************************************************************/
void CreateGameImages()
{
   //Local variables
    int xPlayer;        //Player x pos settings
    int yPlayer;        //Player y pos settings
    int wPlayer;        //Player width settings
    int hPlayer;        //Player height settings
    int xGoal;          //Goal x pos settings
    int yGoal;          //Goal y pos settings
    int wGoal;          //Goal width settings
    int hGoal;          //Goal height settings

    //Init x position of ball between 200-600(So not too close to left/right side)
    int xBall = (rand()%(600-200+1))+ 200;
    //Init y position of ball to any random y position within game screen
    int yBall = (rand()%(SCREEN_HEIGHT-BALL_HEIGHT));

    /*********************************************/

    //Create a window
    gameWindow = SDL_CreateWindow(
    "Welcome to Paddle Ball!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    //Create a renderer for the window
    renderer = SDL_CreateRenderer(gameWindow, -1, 0);
    //Load background
    tempSurface = loadImage("graphics/bombBackground.bmp");
    //Create texture
    textureBackground = loadTexture(tempSurface);
    //Create a rectangle at position 0, 0 for background
    FillRect(rectBackground, 0, 0, 800, 600);
    //Load Ball bitmap
    tempSurface = loadImage("graphics/soccerBall.bmp");
    //Set magenta as transparency color
    tempSurface = setTransparentColor(tempSurface, 255, 0, 255);
    //Create texture
    textureBall = loadTexture(tempSurface);
    //Create a rectangle at position 0, 0 for Ball
    FillRect(rectBall, xBall, yBall, BALL_WIDTH, BALL_HEIGHT);

    //Load Player Paddle bitmap
    tempSurface = loadImage("graphics/soccerPlayer.bmp");
    //Set magenta as transparency color
    tempSurface = setTransparentColor(tempSurface, 255, 0, 255);
    //Create texture
    texturePlayer = loadTexture(tempSurface);
    //Initialize rectangle fields for player paddle
    xPlayer = PLAYER_X;
    yPlayer = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
    wPlayer = PLAYER_WIDTH;
    hPlayer = PLAYER_HEIGHT;
    //Create a rectangle halfway down screen on right side for player
    FillRect(rectPlayer, xPlayer, yPlayer, wPlayer, hPlayer);

    //Load target goal bitmap
    tempSurface = loadImage("graphics/soccerNet.bmp");
    //Set black as transparency color
    tempSurface = setTransparentColor(tempSurface, 0, 0, 0);
    //Create texture
    textureGoal = loadTexture(tempSurface);
    //Position the target goal
    xGoal = GOAL_X;
    yGoal = SCREEN_HEIGHT/2 - GOAL_HEIGHT/2;
    wGoal = GOAL_WIDTH;
    hGoal = GOAL_HEIGHT;
    //Create a rectangle halfway down screen on left side for goal
    FillRect(rectGoal, xGoal, yGoal, wGoal, hGoal);

    //Load player wins background
    tempSurface = loadImage("graphics/playerWins.bmp");
    //Create texture
    textureWin = loadTexture(tempSurface);
    //Create a rectangle at position 0, 0 for player wins background
    FillRect(rectPlayerWin, 0, 0, 800, 600);

    //Load player loses background
    tempSurface = loadImage("graphics/playerLoses.bmp");
    //Create texture
    textureLose = loadTexture(tempSurface);
    //Create a rectangle at position 0, 0 for player loses background
    FillRect(rectPlayerLose, 0, 0, 800, 600);

    //Give ball 50/50 chance of going left or right
    if(rand()%2 == 0)
        xDirection *= -1;
    //Give ball 50/50 chance of going up or down
    if(rand()%2 == 0)
        yDirection *= -1;

}//END CreateGameImages

/**********************************************************************************
* Function : updateGoal
* Author   : Ryan May
* Description: This function moves the target goal sprite and keeps sprite within
*    the borders of a game window.  The function moves the goal vertically at a constant speed.
*    The direction of the goal is changed when it hits the top or bottom of the game screen.
*    This keeps the goal sprite within the boundaries of the screen, and allows goal sprite
*    to move continuously back and forth.
*
* Parameters: none
* Return    : void
*
* BEGIN updateGoal
*    Move goal vertically
*    IF(Goal hits top of the screen)
*       Adjust y position of goal to keep on screen
*       Change y direction of goal
*    END IF
*    IF(Goal hits bottom of the screen)
*       Adjust y position of goal to keep on screen
*       Change y direction of goal
*    END IF
* END updateGoal
************************************************************************************/
void UpdateGoal()
{
    //Move goal vertically
    rectGoal.y += yGoalDir;

    //IF(Goal hits top of the screen)
    if(rectGoal.y < 0)
    {
        //Adjust y position of goal to keep on screen
        rectGoal.y = 0;
        //Change y direction of goal
        yGoalDir *= -1;
    }
    //END IF

    //IF(Goal hits bottom of the screen)
    if(rectGoal.y > SCREEN_HEIGHT-rectGoal.h)
    {
        //Adjust y position of goal to keep on screen
        rectGoal.y = SCREEN_HEIGHT-rectGoal.h;
        //Change y direction of goal
        yGoalDir *= -1;
    }
    //END IF
}//END UpdateGoal
