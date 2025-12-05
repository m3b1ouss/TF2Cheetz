#pragma once
#include <string>
#include "mathlib.h"
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR col);

void InitFont(int height);

void DrawDXText(const Vector2& screenPos, const std::string text, int height, D3DCOLOR col);

void DrawLine(int x1, int y1, int x2, int y2, int width, D3DCOLOR col);

void DrawLine(Vector2 src, Vector2 dst, int width, D3DCOLOR col);

void DrawHBar(Vector2 pos, int length, int width, D3DCOLOR col);

void Draw2DBox(Vector2 top, Vector2 bottom, int width, D3DCOLOR col, Vector2& bottomLeft, Vector2& topRight);

void DrawCircle(Vector3 center, float radius);
//void DrawGraph(const std::vector<int>* vertices, int vId, int vWidth, D3DCOLOR vCol, const void (*transformFunc)(int, Vector2&));