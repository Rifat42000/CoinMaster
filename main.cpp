#include <GL/glut.h>  // Include the GLUT library for OpenGL
#include <SOIL/SOIL.h>  // Include the SOIL library for loading textures
#include <AL/al.h>  // Include OpenAL for sound
#include <AL/alc.h>
#include <AL/alut.h>
#include <cstdlib>  // For rand() and srand()
#include <ctime>  // For time()
#include <iostream>  // For input/output
#include <sstream>  // For string streams
#include <cstring>  // For string manipulation

using namespace std;

// Game parameters
int screenWidth = 1920;  // Width of the game window
int screenHeight = 1080;  // Height of the game window
float playerX = 200;  // Player's X position
float playerY = 250;  // Player's Y position
float playerWidth = 80;  // Player's width
float playerHeight = 80;  // Player's height
float playerSpeed = 25;  // Player's movement speed

float coinX, coinY;  // Coin's position
float coinWidth = 150;  // Coin's width
float coinHeight = 150;  // Coin's height
float coinSpeed = 5;  // Coin's falling speed
float speedIncrement = 0.3;  // How much the coin speed increases over time
int speedUpdateInterval = 3000;  // How often the coin speed increases (in milliseconds)

bool isGameOver = false;  // Game over state
bool isPaused = false;  // Pause state
int score = 0;  // Player's score


// Bird variables
float birdX = -200;  // Bird starts off-screen (left)
float birdY = screenHeight - 200; // Near the top of the screen
float birdWidth = 100;
float birdHeight = 100;
float birdSpeed = 4;  // Bird flying speed

// Texture IDs
GLuint backgroundTexture, playerTexture, coinTexture, birdTexture;

// OpenAL variables for sound
ALuint backgroundBuffer, backgroundSource;  // Background music
ALuint coinBuffer, coinSource;  // Sound when collecting a coin
ALuint gameOverBuffer, gameOverSource;  // Sound when game is over

// Function to load textures from files
GLuint loadTexture(const char* filename) {
    GLuint texture = SOIL_load_OGL_texture(
        filename,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA
    );

    if (!texture) {
        cout << "Failed to load texture: " << filename << endl;
        exit(1);
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texture;
}

// Function to draw a rectangle with a texture
void drawTexturedRectangle(float x, float y, float width, float height, GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


// Function to update the bird's position
void updateBird(int value) {
    if (!isGameOver && !isPaused) {
        birdX += birdSpeed; // Move the bird to the right

        if (birdX > screenWidth) {  // Reset when it flies out
            birdX = -200;
            birdY = screenHeight - (rand() % 300 + 100); // Random Y position
        }
    }

    glutPostRedisplay();
    glutTimerFunc(50, updateBird, 0); // Update bird every 50ms
}

// Function to reset the coin's position
void resetCoin() {
    coinX = rand() % (screenWidth - (int)coinWidth) + coinWidth / 2;
    coinY = screenHeight;
}

// Function to initialize OpenAL for sound
void initAudio() {
    alutInit(0, nullptr);

    // Load background music
    backgroundBuffer = alutCreateBufferFromFile("background.wav");
    alGenSources(1, &backgroundSource);
    alSourcei(backgroundSource, AL_BUFFER, backgroundBuffer);
    alSourcei(backgroundSource, AL_LOOPING, AL_TRUE);
    alSourcePlay(backgroundSource);

    // Load coin collection sound
    coinBuffer = alutCreateBufferFromFile("coin.wav");
    alGenSources(1, &coinSource);
    alSourcei(coinSource, AL_BUFFER, coinBuffer);

    // Load game over sound
    gameOverBuffer = alutCreateBufferFromFile("end.wav");
    alGenSources(1, &gameOverSource);
    alSourcei(gameOverSource, AL_BUFFER, gameOverBuffer);
}

// Initialize OpenGL settings
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set the background color to black
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);  // Set the coordinate system

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load textures
    backgroundTexture = loadTexture("background.png");
    playerTexture = loadTexture("player3.png");
    coinTexture = loadTexture("coin.png");
    birdTexture = loadTexture("bird.png");

    resetCoin();  // Reset the coin's position
    srand(time(0));  // Seed the random number generator
    glutTimerFunc(50, updateBird, 0);
    // Initialize audio
    initAudio();
}

// Function to render text on the screen
void renderText(float x, float y, const char* text, void* font) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (isGameOver) {
        // Display game over message
        glColor3f(1.0f, 0.0f, 0.0f);  // Red color for game over text
        std::ostringstream gameOverText;
        gameOverText << "GAME OVER! Score: " << score;
        float gameOverTextWidth = gameOverText.str().length() * 15;
        renderText((screenWidth - gameOverTextWidth) / 2, screenHeight / 2 + 20, gameOverText.str().c_str(), GLUT_BITMAP_HELVETICA_18);

        glColor3f(1.0f, 1.0f, 1.0f);  // White color for other instructions
        const char* replayText = "Press R to Replay";
        float replayTextWidth = strlen(replayText) * 9;
        renderText((screenWidth - replayTextWidth) / 2, screenHeight / 2 - 20, replayText, GLUT_BITMAP_HELVETICA_18);

        const char* exitText = "Press ESC to Exit";
        float exitTextWidth = strlen(exitText) * 9;
        renderText((screenWidth - exitTextWidth) / 2, screenHeight / 2 - 60, exitText, GLUT_BITMAP_HELVETICA_18);
    }
    else {
        // Draw the background, player, and coin
        drawTexturedRectangle(0, 0, screenWidth, screenHeight, backgroundTexture);
        drawTexturedRectangle(playerX, playerY, playerWidth, playerHeight, playerTexture);
        drawTexturedRectangle(coinX, coinY, coinWidth, coinHeight, coinTexture);  // Use rectangle for coin
        drawTexturedRectangle(birdX, birdY, birdWidth, birdHeight, birdTexture);

        // Display the score
        glColor3f(1.0f, 1.0f, 1.0f);
        std::ostringstream scoreText;
        scoreText << "Score: " << score;
        renderText(10, screenHeight - 20, scoreText.str().c_str(), GLUT_BITMAP_HELVETICA_18);
    }

    glutSwapBuffers();
}

// Timer callback for game logic
void update(int value) {
    if (!isGameOver && !isPaused) {
        coinY -= coinSpeed;  // Move the coin down

        // Check if the player catches the coin
        if (coinY <= playerY + playerHeight && coinX + coinWidth >= playerX && coinX <= playerX + playerWidth) {
            score++;
            resetCoin();

            // Play coin collection sound
            alSourcePlay(coinSource);
        }

        // Check if the coin reaches the bottom
        if (coinY <= 0) {
            isGameOver = true;

            // Stop background music
            alSourcePause(backgroundSource);

            // Play game over sound
            alSourcePlay(gameOverSource);

            return;  // Stop further updates
        }

        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
    }
}

// Timer callback to increase coin speed
void updateSpeed(int value) {
    if (!isGameOver && !isPaused) {
        coinSpeed += speedIncrement;
        glutTimerFunc(speedUpdateInterval, updateSpeed, 0);
    }
}

// Keyboard callback function
void handleKeys(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') {
        playerX -= playerSpeed;  // Move player left
        if (playerX < 0) playerX = 0;
    }
    else if (key == 'd' || key == 'D') {
        playerX += playerSpeed;  // Move player right
        if (playerX + playerWidth > screenWidth) playerX = screenWidth - playerWidth;
    }
    else if (key == 'r' || key == 'R') {
        // Restart the game
        isGameOver = false;
        score = 0;
        coinSpeed = 5;
        resetCoin();
        alSourcePlay(backgroundSource);  // Start background music again
        glutTimerFunc(16, update, 0);
        glutTimerFunc(speedUpdateInterval, updateSpeed, 0);
    }
    else if (key == 'p' || key == 'P') {
        // Pause or resume the game
        isPaused = !isPaused;
        if (isPaused) {
            alSourcePause(backgroundSource);
        }
        else {
            alSourcePlay(backgroundSource);
            glutTimerFunc(16, update, 0);
            glutTimerFunc(speedUpdateInterval, updateSpeed, 0);
        }
    }
    else if (key == 27) {  // ESC key to exit
        exit(0);  // Quit the game
    }
}

// Cleanup function for OpenAL resources
void cleanupAudio() {
    alDeleteSources(1, &backgroundSource);
    alDeleteBuffers(1, &backgroundBuffer);
    alDeleteSources(1, &coinSource);
    alDeleteBuffers(1, &coinBuffer);
    alDeleteSources(1, &gameOverSource);
    alDeleteBuffers(1, &gameOverBuffer);

    alutExit();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("coinMaster");

    init();
    atexit(cleanupAudio);  // Register cleanup function

    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeys);
    glutTimerFunc(16, update, 0);
    glutTimerFunc(speedUpdateInterval, updateSpeed, 0);

    glutMainLoop();
    return 0;
}