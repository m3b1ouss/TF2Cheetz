#include "dx.h"
#include "mathlib.h"
#include <string>

typedef void(__stdcall* transformFunc)(int id, Vector2& screenPos);

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR col) 
{
	D3DRECT rect = {x, y, x + w, x + h};
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, col, 0, 0);
}

void InitFont(int height)
{
	D3DXFONT_DESCW fontDesc {
		.Height = height,
		.Width = 0,
		.Weight = FW_REGULAR,
		.MipLevels = 0,
		.Italic = false,
		.CharSet = DEFAULT_CHARSET,
		.OutputPrecision = OUT_DEFAULT_PRECIS,
		.Quality = DEFAULT_QUALITY,
		.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
		.FaceName = L"DengXian"
	};

	D3DXCreateFontIndirect(pDevice, &fontDesc, &Font);
}

void DrawDXText(const Vector2& screenPos, const std::string text, int height, D3DCOLOR col) {
	RECT rect{};
	Font->DrawTextA(nullptr, text.c_str(), text.length(), &rect, DT_CALCRECT, D3DCOLOR_XRGB(0,0,0));
	int rLength = rect.right - rect.left;
	rect.left = static_cast<LONG>(screenPos.abscissa - rLength / 2.f);
	rect.right = static_cast<LONG>(screenPos.abscissa + rLength / 2.f);
	rect.top = static_cast<LONG>(screenPos.ordinate - height*0.6f);
	rect.bottom = rect.top + rLength;
	Font->DrawTextA(nullptr, text.c_str(), -1, &rect, DT_NOCLIP, col);
}

void DrawLine(int x1, int y1, int x2, int y2, int width, D3DCOLOR col) 
{
	ID3DXLine* Line;
	D3DXCreateLine(pDevice, &Line);
	D3DXVECTOR2 LineVec[2];
	LineVec[0] = D3DXVECTOR2(x1, y1);
	LineVec[1] = D3DXVECTOR2(x2, y2);
	Line->SetWidth(width);
	Line->Draw(LineVec, 2, col);
	Line->Release();
}

void DrawLine(Vector2 src, Vector2 dst, int width, D3DCOLOR col) 
{
	DrawLine(src.abscissa, src.ordinate, dst.abscissa, dst.ordinate, width, col);
}

void DrawHBar(Vector2 pos, int length, int width, D3DCOLOR col) 
{
	Vector2 l, r;
	l.abscissa = r.abscissa = pos.abscissa;
	l.ordinate = pos.ordinate;
	r.ordinate = pos.ordinate + length;
	DrawLine(l, r, width, col);
}

void Draw2DBox(Vector2 top, Vector2 bottom, int width, D3DCOLOR col, Vector2 &bottomLeft, Vector2 &topRight) {
	int height = abs(top.ordinate - bottom.ordinate);
	Vector2 tl, tr, bl, br;
	tl.abscissa = top.abscissa - height / 4;
	tr.abscissa = top.abscissa + height / 4;
	tl.ordinate = tr.ordinate = top.ordinate;

	bl.abscissa = bottom.abscissa - height / 4;
	br.abscissa = bottom.abscissa + height / 4;
	bl.ordinate = br.ordinate = bottom.ordinate;

	bottomLeft = bl;
	topRight = tr;

	DrawLine(tl, tr, width, col);
	DrawLine(bl, br, width, col);
	DrawLine(tl, bl, width, col);
	DrawLine(tr, br, width, col);
}

void DrawCircle(Vector2 center, float radius) 
{
	
}

/*
void DrawGraph(const std::vector<Vector2> *vertices, int vId, int vWidth, D3DCOLOR vCol) {
	
	if (vertices[vId].size() <= 1)
		return;

	for (auto v : vertices[vId]) 
	{
		if (vertices[v][0] != vId) 
		{
			DrawLine(pos1, pos2, vWidth, vCol);
			DrawGraph(vertices, vId, vWidth, vCol);
		}
	}
}
*/