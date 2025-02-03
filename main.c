#include <stdio.h>
#include "raylib.h"
#include "snake_list.h"
#include "snake_array.h"
#include "screen_shake.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define STATUS_BAR_HEIGHT 40
#define PLAY_AREA_Y STATUS_BAR_HEIGHT


typedef struct
{
    Vector2 position;
    bool active;
} Apple;
typedef struct
{
    bool gameOver;
    Vector2 direction;
    float moveTimer;
    float moveDelay;
    Apple apple;
    int score;
    bool using_array;
} GameState;

void CheckDataStructure(SnakeInterface **snake, bool *using_array)
{
    if (IsKeyPressed(KEY_SPACE))
    {
        if (*using_array)
        {
            *snake = get_snake_list(false);
            *using_array = false;
        }
        else
        {
            *snake = get_snake_array(false);
            *using_array = true;
        }
    }
}
Vector2 GetInput(Vector2 direction)
{
    const int GRID_SIZE = 30;

    if (IsKeyPressed(KEY_D) && direction.x == 0)
    {
        direction = (Vector2){GRID_SIZE, 0};
    }
    if (IsKeyPressed(KEY_A) && direction.x == 0)
    {
        direction = (Vector2){-GRID_SIZE, 0};
    }
    if (IsKeyPressed(KEY_W) && direction.y == 0)
    {
        direction = (Vector2){0, -GRID_SIZE};
    }
    if (IsKeyPressed(KEY_S) && direction.y == 0)
    {
        direction = (Vector2){0, GRID_SIZE};
    }
    return direction;
}
bool CheckEdgeCollision(Position pos)
{
    return (pos.x < 0 || pos.x + 30 >= WINDOW_WIDTH ||
            pos.y < STATUS_BAR_HEIGHT || pos.y +30 >= WINDOW_HEIGHT);
}
bool IsPositionOnSnake(Vector2 pos, SnakeInterface *snake)
{
    // Get snake positions (this will need to be added to your snake interface)
    int length = snake->get_length();
    Position *positions = snake->get_positions();

    for (int i = 0; i < length; i++)
    {
        if (pos.x == positions[i].x && pos.y == positions[i].y)
        {
            return true;
        }
    }
    return false;
}
void SpawnApple(Apple *apple, SnakeInterface *snake)
{
    Vector2 newPos;
    do {
        // Calculate playable area
        int playAreaWidth = WINDOW_WIDTH - 30;  // Subtract segment size to keep within bounds
        int playAreaHeight = WINDOW_HEIGHT - STATUS_BAR_HEIGHT - 30;  // Subtract status bar and segment size

        // Calculate number of possible grid positions
        int maxGridX = playAreaWidth / 30;
        int maxGridY = playAreaHeight / 30;

        // Get random grid coordinates
        int gridX = GetRandomValue(0, maxGridX);
        int gridY = GetRandomValue(0, maxGridY);

        // Convert to pixel coordinates, ensuring alignment with snake movement
        newPos.x = gridX * 30;
        newPos.y = STATUS_BAR_HEIGHT + (gridY * 30);

    } while (IsPositionOnSnake(newPos, snake));

    apple->position = newPos;
    apple->active = true;
}
void ResetGame(GameState *state, SnakeInterface *snake)
{
    state->gameOver = false;
    state->direction = (Vector2){30, 0};
    state->moveTimer = 0;
    state->score = 0;

    snake->init(WINDOW_WIDTH/2, PLAY_AREA_Y + (WINDOW_HEIGHT-PLAY_AREA_Y)/2);
    SpawnApple(&state->apple, snake);
}
bool CheckAppleCollision(Vector2 snakeHead, Apple apple)
{
    // Create rectangular bounds for both snake head and apple
    Rectangle snakeRect = {
            snakeHead.x,
            snakeHead.y,
            30,  // Width matches snake segment size
            30   // Height matches snake segment size
    };

    Rectangle appleRect = {
            apple.position.x,
            apple.position.y,
            30,  // Width matches apple size
            30   // Height matches apple size
    };

    // Check if rectangles overlap
    return CheckCollisionRecs(snakeRect, appleRect);
}
void DrawApple(Apple apple)
{
    if (apple.active)
    {
        // Draw the apple as a red square
        DrawRectangle(apple.position.x, apple.position.y, 30, 30, RED);

        // Optional: Draw a border to make grid alignment visible
        DrawRectangleLinesEx((Rectangle){
                apple.position.x,
                apple.position.y,
                30,
                30
        }, 1, WHITE);
    }
}
void ConstantlyMove(SnakeInterface *snake, GameState *state)
{
    state->moveTimer += GetFrameTime();

    if (state->moveTimer >= state->moveDelay && !state->gameOver)
    {
        state->moveTimer = 0;

        Position head = snake->get_head();
        // Ensure next position stays on grid
        Position nextPos = {
                head.x + state->direction.x,
                head.y + state->direction.y
        };

        // Debug print to verify positions
        printf("Head pos: (%d, %d), Next pos: (%d, %d)\n",
               head.x, head.y, nextPos.x, nextPos.y);

        if (CheckEdgeCollision(nextPos))
        {
            state->gameOver = true;
            TriggerScreenShake(0.5f, 15.0f);
            return;
        }

        // Check apple collision with current position
        Vector2 currentHead = {(float)head.x, (float)head.y};
        if (state->apple.active && CheckAppleCollision(currentHead, state->apple))
        {
            snake->grow();
            state->score++;
            SpawnApple(&state->apple, snake);
        }

        snake->move(state->direction.x, state->direction.y);
    }
}
void DrawDebugGrid(void)
{
    // Draw vertical lines
    for(int x = 0; x < WINDOW_WIDTH; x += 30) {
        DrawLine(x, STATUS_BAR_HEIGHT, x, WINDOW_HEIGHT, (Color){50, 50, 50, 255});
    }
    // Draw horizontal lines
    for(int y = STATUS_BAR_HEIGHT; y < WINDOW_HEIGHT; y += 30) {
        DrawLine(0, y, WINDOW_WIDTH, y, (Color){50, 50, 50, 255});
    }
}
void DrawStatusBar(SnakeInterface* snake, bool using_array, int score)
{
    DrawRectangle(0, 0, WINDOW_WIDTH, STATUS_BAR_HEIGHT, DARKGRAY);

    const char* data_structure = using_array ? "Array Data Structure" : "List Data Structure";
    DrawText(data_structure, 10, 10, 20, WHITE);

    // Update score display
    char scoreText[32];
    sprintf(scoreText, "Score: %d", score);
    int scoreWidth = MeasureText(scoreText, 20);
    DrawText(scoreText, (WINDOW_WIDTH - scoreWidth) / 2, 10, 20, WHITE);

    char lengthText[32];
    sprintf(lengthText, "Length: %d", snake->get_length());
    DrawText(lengthText, WINDOW_WIDTH - MeasureText(lengthText, 20) - 10, 10, 20, WHITE);

    DrawLine(0, STATUS_BAR_HEIGHT, WINDOW_WIDTH, STATUS_BAR_HEIGHT, WHITE);
}
void DrawGameState(SnakeInterface *snake, GameState *state)
{
    BeginDrawing();

    ClearBackground(BLACK);

    if (IsScreenShaking())
    {
        Vector2 offset = GetShakeOffset();
        BeginMode2D((Camera2D){
                .offset = offset,
                .target = (Vector2){0, 0},
                .rotation = 0.0f,
                .zoom = 1.0f
        });
    }

    DrawDebugGrid();
    DrawStatusBar(snake, state->using_array, state->score);
    DrawRectangleLinesEx((Rectangle){0, STATUS_BAR_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT - STATUS_BAR_HEIGHT}, 1, WHITE);

    DrawApple(state->apple);  // Draw apple
    snake->draw();

    if (state->gameOver)
    {
        const char *gameOverText = "GAME OVER!";
        const char *restartText = "Press R to Play Again";

        int gameOverWidth = MeasureText(gameOverText, 40);
        int restartWidth = MeasureText(restartText, 20);

        DrawText(gameOverText,
                 (WINDOW_WIDTH - gameOverWidth) / 2,
                 WINDOW_HEIGHT / 2,
                 40,
                 RED);

        DrawText(restartText,
                 (WINDOW_WIDTH - restartWidth) / 2,
                 WINDOW_HEIGHT / 2 + 50,
                 20,
                 WHITE);
    }

    if (IsScreenShaking())
    {
        EndMode2D();
    }

    EndDrawing();
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake Game");
    SetTargetFPS(120);

    SnakeInterface *snake = get_snake_array(true);

    GameState state = {
            .gameOver = false,
            .direction = {30, 0},
            .moveTimer = 0,
            .moveDelay = 0.2f,
            .using_array = true,
            .score = 0,
            .apple = {0}
    };

    int startX = ((WINDOW_WIDTH / 2) / 30) * 30;  // Round to nearest grid position
    int startY = (((WINDOW_HEIGHT + STATUS_BAR_HEIGHT) / 2) / 30) * 30;
    snake->init(startX, startY);
    SpawnApple(&state.apple, snake);
    InitScreenShake();

    while (!WindowShouldClose())
    {
        if (state.gameOver) {
            if (IsKeyPressed(KEY_R)) {
                ResetGame(&state, snake);
            }
        } else {
            CheckDataStructure(&snake, &state.using_array);
            state.direction = GetInput(state.direction);
        }

        ConstantlyMove(snake, &state);
        UpdateScreenShake();
        DrawGameState(snake, &state);
    }

    snake->cleanup();
    CloseWindow();
    return 0;
}
