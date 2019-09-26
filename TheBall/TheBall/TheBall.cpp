#include <windows.h>
#include <stdlib.h>
#include <tchar.h>

typedef struct ball {
	int X;
	int Y;
	int Radius;
	int Speed;
} Ball;

// Global variables
static TCHAR szWindowClass[] = _T("TheBall");

static TCHAR szTitle[] = _T("The Ball");

HINSTANCE hInst;

Ball ball;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void InitializeBall(HWND);

void DrawBall(HWND);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("The Ball"),
			NULL);

		return 1;
	}

	hInst = hInstance;

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 400,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		InitializeBall(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		DrawBall(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void InitializeBall(HWND hWnd)
{
	RECT rect;
	BOOL result = GetWindowRect(hWnd, &rect);
	int wndWidth = rect.right - rect.left;
	int wndHeight = rect.bottom - rect.top;
	ball.X = wndWidth / 2;
	ball.Y = wndHeight / 2;
	ball.Radius = wndHeight / 20;
	ball.Speed = 5;
}

void DrawBall(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	Ellipse(hdc, ball.X - ball.Radius, ball.Y - ball.Radius, ball.X + ball.Radius, ball.Y + ball.Radius);
	EndPaint(hWnd, &ps);
}
