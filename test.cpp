# include <GL/glut.h>
# include <stdlib.h>
# include <time.h>

// Game variables
bool playing = false;
bool paused = false;

// Player and coin positions
int playerX, playerY, coinX, coinY;

// Score
int score = 0;

// Images
GLuint background, player, coin;

// Background texture coordinates
GLfloat bgTexCoords[] = { 0.0f, 1.0f,
                         1.0f, 1.0f,
                         1.0f, 0.0f,
                         0.0f, 0.0f };

void init() {
    // Initialize player and coin positions
    srand(time(0));
    playerX = (rand() % 800) + 50;
    playerY = 550;
    coinX = (rand() % 800) + 50;
    coinY = -10;

    // Load images
    background = loadTexture("background.jpg");
    player = loadTexture("player.png");
    coin = loadTexture("coin.png");

    // Initialize score
    score = 0;
}

void display() {
    if (playing && !paused) {
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw background
        glBindTexture(GL_TEXTURE_2D, background);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-400.0f, -300.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(400.0f, -300.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(400.0f, 300.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-400.0f, 300.0f, 0.0f);
        glEnd();

        // Draw player
        glBindTexture(GL_TEXTURE_2D, player);
        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f); // White color for player
        glTexCoord2f(0.0f, 1.0f); glVertex3f(playerX - 20, playerY - 50, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(playerX + 20, playerY - 50, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(playerX + 20, playerY + 50, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(playerX - 20, playerY + 50, 0.0f);
        glEnd();

        // Move coin
        if (coinY < 300) {
            coinY += 1;
            glutTimerFunc(16, updateCoinPosition, NULL);
        }

        // Draw coin
        glBindTexture(GL_TEXTURE_2D, coin);
        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f); // White color for coin
        glTexCoord2f(0.0f, 1.0f); glVertex3f(coinX - 10, coinY - 20, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(coinX + 10, coinY - 20, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(coinX + 10, coinY + 20, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(coinX - 10, coinY + 20, 0.0f);
        glEnd();

        // Check collision with player and coin
        if (playerX > coinX - 30 && playerX < coinX + 30 &&
            playerY > coinY - 50 && playerY < coinY + 30) {
            score += 10;
            printf("Score: %d\n", score);
        }

        // Update player position
        if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
            playerX -= 5;
        }
        else if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
            playerY -= 5;
        }

        // Update display
        glutSwapBuffers();
    }
}

void updateCoinPosition(void* data) {
    coinY += 1;

    // Check if coin is out of screen
    if (coinY > 300) {
        coinX = (rand() % 800) + 50;
        coinY = -10;
        score -= 5;
        printf("Score: %d\n", score);
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (playing && !paused) {
        switch (key) {
        case 'p':
            paused = !paused;
            break;
        default:
            break;
        }
    }
    else if (!playing) {
        switch (key) {
        case 27: // Escape key
            exit(0);
            break;
        case 'm': // Menu key
            playing = true;
            paused = false;
            playerX = (rand() % 800) + 50;
            playerY = 550;
            score = 0;
            break;
        default:
            break;
        }
    }
}

void specialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        playerX -= 5;
        break;
    case GLUT_KEY_RIGHT:
        playerX += 5;
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);

    // Set display mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Set window size and position
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(50, 50);

    // Create window
    glutCreateWindow("Coin Collector");

    // Initialize game variables
    init();

    // Register callback functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);

    // Main loop
    glutMainLoop();

    return 0;
}

// Function to load texture from file
GLuint loadTexture(const char* filename) {
    GLuint texID;
    unsigned char* image = SOIL_load_image(filename, &texWidth, &texHeight, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SOIL_free_image_data(image);
    return texID;
}