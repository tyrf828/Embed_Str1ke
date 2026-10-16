#include "Lib.h"

void DrawStringMiddle(float x, float y, const TCHAR* str, unsigned int color)
{
    // 文字列のサイズを取得
    int w = GetDrawStringWidth(str, -1);
    int h = GetFontSize();
    // 文字列の長さの半分をずらす
    DrawStringF(x - (w * 0.5f), y - (h * 0.5f), str, color);
}

void DrawStringMiddleToHandle(float x, float y, const TCHAR* str, unsigned int color, int handle)
{
    // 文字列のサイズを取得
    int w = GetDrawStringWidthToHandle(str, -1, handle);
    int h = GetFontSizeToHandle(handle);
    // 文字列の長さの半分をずらす
    DrawStringFToHandle(x - (w * 0.5f), y - (h * 0.5f), str, color, handle);
}

float GetRandomF(float max)
{
    return GetRandomF(0.0f, max);
}

float GetRandomF(float min, float max)
{
    return min + float(rand()) / (float)((RAND_MAX / (max - min)));
}

float Clamp01(float value)
{
    return std::clamp(value, 0.0f, 1.0f);
}

bool IsInDotAngle(const float3& forward, const float3& to_target, float angle_degree)
{
    float dot_val   = dot(normalize(forward), normalize(to_target));
    float dot_angle = acosf(dot_val) * RadToDeg;
    if(dot_angle <= angle_degree)
    {
        return true;
    }
    return false;
}
