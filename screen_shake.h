#ifndef SCREEN_SHAKE_H
#define SCREEN_SHAKE_H

#include <raylib.h>
#include <stdbool.h>

typedef struct {
    float duration;
    float intensity;
    float timer;
    bool isShaking;
    Vector2 offset;
} ScreenShake;

void InitScreenShake(void);
void TriggerScreenShake(float duration, float intensity);
void UpdateScreenShake(void);
Vector2 GetShakeOffset(void);
bool IsScreenShaking(void);

#endif // SCREEN_SHAKE_H