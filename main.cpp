#include <windows.h>
#include "gomoku.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("GpDoubleBuffer");

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")
class CGdiPlusStarter {
private:
	ULONG_PTR m_gpToken;

public:
	bool m_bSuccess;
	CGdiPlusStarter() {
		GdiplusStartupInput gpsi;
		m_bSuccess = (GdiplusStartup(&m_gpToken, &gpsi, NULL) == Ok);
	}
	~CGdiPlusStarter() {
		GdiplusShutdown(m_gpToken);
	}
};
CGdiPlusStarter g_gps;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	if (g_gps.m_bSuccess == FALSE) {
		MessageBox(NULL, TEXT("GDI+ 라이브러리를 초기화할 수 없습니다."),
			TEXT("알림"), MB_OK);
		return 0;
	}

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

CachedBitmap* pCBit;

Gomoku gomoku;

const int kSquareSize{50};

int mouse_board_x{-1};
int mouse_board_y{-1};

void UpdateScreen() {
	Graphics G(hWndMain);
	RECT crt;
	GetClientRect(hWndMain, &crt);
	Bitmap* pBit = new Bitmap(crt.right, crt.bottom, &G);
	Graphics* memG = new Graphics(pBit);

	//white background
	memG->FillRectangle(&SolidBrush(Color(255, 255, 255)), 0, 0, crt.right, crt.bottom);

	//board orange rectange draw
	memG->FillRectangle(&SolidBrush(Color(231, 187, 95)), 0, 0, (gomoku.board_length() + 1) * kSquareSize, (gomoku.board_length() + 1) * kSquareSize);

	Pen pen(Color(0, 0, 0));
	for (int i = 1; i <= gomoku.board_length(); ++i) {
		memG->DrawLine(&pen, i* kSquareSize, kSquareSize, i * kSquareSize, kSquareSize* gomoku.board_length()+1);
	}
	for (int i = 1; i <= gomoku.board_length(); ++i) {
		memG->DrawLine(&pen, kSquareSize, i * kSquareSize,kSquareSize* gomoku.board_length()+1, i * kSquareSize );
	}


	SolidBrush brush_black(Color(0, 0, 0));
	SolidBrush brush_white(Color(255, 255, 255));

	const auto & board = gomoku.board();
	for (size_t x = 0; x < gomoku.board_length(); x++) {
		for (size_t y = 0; y < gomoku.board_length(); y++) {
			switch (board[x + y * gomoku.board_length()]) {
			case gomoku.black:
				memG->FillEllipse(&brush_black, 
					x * kSquareSize + kSquareSize / 2, y * kSquareSize + kSquareSize / 2, 
					kSquareSize, kSquareSize);
				break;
			case gomoku.white:
				memG->FillEllipse(&brush_white,
					x * kSquareSize + kSquareSize / 2, y * kSquareSize + kSquareSize / 2, 
					kSquareSize, kSquareSize);
				break;
			}
		}
	}

	//mouse preview piece with alpha
	SolidBrush brush_black_alpha(Color(128,0, 0, 0));
	SolidBrush brush_white_alpha(Color(128,255, 255, 255));

	//preview is have to in board
	//and nothing ont preview location
	if (0 <= mouse_board_x && mouse_board_x < gomoku.board_length() &&
		0 <= mouse_board_y && mouse_board_y < gomoku.board_length()
		&& board[mouse_board_x + mouse_board_y* gomoku.board_length()] == gomoku.none ) {

		switch (gomoku.getturn()) {
		case gomoku.black:
			memG->FillEllipse(&brush_black_alpha,
				mouse_board_x * kSquareSize + kSquareSize / 2,
				mouse_board_y * kSquareSize + kSquareSize / 2,
				kSquareSize, kSquareSize);
			break;
		case gomoku.white:
			memG->FillEllipse(&brush_white_alpha,
				mouse_board_x * kSquareSize + kSquareSize / 2,
				mouse_board_y * kSquareSize + kSquareSize / 2,
				kSquareSize, kSquareSize);
			break;
		}
	}

	if (pCBit) {
		delete pCBit;
	}
	pCBit = new CachedBitmap(pBit, &G);
	delete pBit;
	delete memG;
	InvalidateRect(hWndMain, NULL, FALSE);
}

void OnPaint(HDC hdc) {
	Graphics G(hdc);

	if (pCBit == NULL) {
		UpdateScreen();
	}
	G.DrawCachedBitmap(pCBit, 0, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	static int mouse_x;
	static int mouse_y;

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		return 0;

	case WM_LBUTTONDOWN:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);
		mouse_board_x = (mouse_x + kSquareSize / 2) / kSquareSize - 1;
		mouse_board_y = (mouse_y + kSquareSize / 2) / kSquareSize - 1;

		if (!gomoku.PieceOnBoard(mouse_board_x, mouse_board_y))
			//if fail PieceOnBoard
			return 0;

		if (gomoku.CheckWin()) {
			MessageBox(hWnd, L"승리", L"승리", MB_OK);
			
			//reset
			gomoku.reset();
		}
		else {
			gomoku.PassTurn();
		}
		UpdateScreen();
		return 0;

	case WM_MOUSEMOVE:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);

		mouse_board_x = (mouse_x + kSquareSize / 2) / kSquareSize - 1;
		mouse_board_y = (mouse_y + kSquareSize / 2) / kSquareSize - 1;

		UpdateScreen();
		return 0;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			UpdateScreen();
			break;
		case VK_RIGHT:
			UpdateScreen();
			break;
		case VK_UP:
			UpdateScreen();
			break;
		case VK_DOWN:
			UpdateScreen();
			break;
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		if (pCBit) {
			delete pCBit;
		}
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}