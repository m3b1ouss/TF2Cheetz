#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

bool GetDXDevice(void** pTable, size_t size);

extern LPDIRECT3DDEVICE9 pDevice;
extern ID3DXFont* Font;