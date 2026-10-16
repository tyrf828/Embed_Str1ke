#pragma once

void DrawStringMiddle(float x, float y, const TCHAR* str, unsigned int color);

void DrawStringMiddleToHandle(float x, float y, const TCHAR* str, unsigned int color, int handle);

float GetRandomF(float max);

float GetRandomF(float min, float max);

float Clamp01(float value);

bool IsInDotAngle(const float3& forward, const float3& to_target, float angle_degree);
