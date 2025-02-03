#include "screen_shake.h"

static ScreenShake shake = {0};

void InitScreenShake(void)
{
    shake.duration = 0.0f;
    shake.intensity = 0.0f;
    shake.timer = 0.0f;
    shake.isShaking = false;
    shake.offset = (Vector2){0, 0};
}

void TriggerScreenShake(float duration, float intensity)
{
    shake.duration = duration;
    shake.intensity = intensity;
    shake.timer = 0.0f;
    shake.isShaking = true;
}

void UpdateScreenShake(void)
{
    if (!shake.isShaking) return;

    shake.timer += GetFrameTime();

    if (shake.timer >= shake.duration)
    {
        shake.isShaking = false;
        shake.offset = (Vector2){0, 0};
        return;
    }

    float currentIntensity = shake.intensity * (1.0f - (shake.timer / shake.duration));
    shake.offset.x = GetRandomValue(-currentIntensity, currentIntensity);
    shake.offset.y = GetRandomValue(-currentIntensity, currentIntensity);
}

Vector2 GetShakeOffset(void)
{
    return shake.offset;
}

bool IsScreenShaking(void)
{
    return shake.isShaking;
}