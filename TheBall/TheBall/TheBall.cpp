#include <windows.h>
#include <stdlib.h>
#include <tchar.h>


////////////////////////////////
// User types
enum DirectionX { LEFT, RIGHT, NONE_X };
enum DirectionY { UP, DOWN, NONE_Y };

typedef struct ball {
	float X, Y, Radius;
	float SpeedX, SpeedY;
	float BoostX, BoostY;
	DirectionX directionX;
	DirectionY directionY;
} Ball;


////////////////////////////////
// Global variables
//
constexpr auto WND_WIDTH = 1000.0;
constexpr auto WND_HEIGHT = 600.0;
constexpr auto BOOST_LEFT = -0.5;
constexpr auto BOOST_RIGHT = 0.5;
constexpr auto BOOST_UP = -0.5;
constexpr auto BOOST_DOWN = 0.5;
constexpr auto ALLOWED_FAULT = 0.3;
constexpr auto COUNTER_BOOST_PERCENTAGE = 0.25;

static TCHAR szWindowClass[] = _T("TheBall");
static TCHAR szTitle[] = _T("The Ball");
HINSTANCE hInst;
Ball ball;
BOOL wallHitten = FALSE;
int timer = 1;


////////////////////////////////
// Forward declarations of functions included in this code module:
//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitializeBall(HWND);
void DrawBall(HWND);
void RecalculateBallSpeed();
void RecalculateBallPosition();

BOOL LeftHitten();
BOOL RightHitten();
BOOL TopHitten();
BOOL BottomHitten();

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
	wcex.hbrBackground = CreateSolidBrush(RGB(63, 145, 109));
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
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		(int)WND_WIDTH,
		(int)WND_HEIGHT,
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
		SetTimer(hWnd, timer, 1, NULL);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		DrawBall(hWnd);
		break;
	case WM_TIMER:
		RecalculateBallSpeed();
		RecalculateBallPosition();
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			ball.BoostX = BOOST_LEFT;
			ball.directionX = LEFT;
			break;
		case VK_RIGHT:
			ball.BoostX = BOOST_RIGHT;
			ball.directionX = RIGHT;
			break;
		case VK_UP:
			ball.BoostY = BOOST_UP;
			ball.directionY = UP;
			break;
		case VK_DOWN:
			ball.BoostY = BOOST_DOWN;
			ball.directionY = DOWN;
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam) {
		case VK_LEFT:
		case VK_RIGHT:
			ball.BoostX *= (-1) * COUNTER_BOOST_PERCENTAGE;
			ball.directionX = NONE_X;
			break;
		case VK_UP:
		case VK_DOWN:
			ball.BoostY *= (-1) * COUNTER_BOOST_PERCENTAGE;
			ball.directionY = NONE_Y;
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void InitializeBall(HWND hWnd)
{
	ball.X = WND_WIDTH / 2;
	ball.Y = WND_HEIGHT / 2;
	ball.Radius = WND_HEIGHT / 30;
	ball.SpeedX = 0.0;
	ball.SpeedY = 0.0;
	ball.BoostX = 0.0;
	ball.BoostY = 0.0;
	ball.directionX = NONE_X;
	ball.directionY = NONE_Y;
}

void DrawBall(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	Ellipse(hdc, (int)(ball.X - ball.Radius), (int)(ball.Y - ball.Radius), (int)(ball.X + ball.Radius), (int)(ball.Y + ball.Radius));
	EndPaint(hWnd, &ps);
}

void RecalculateBallSpeed()
{
	if (ball.directionX == NONE_X) {
		if (ball.SpeedX > (-1) * ALLOWED_FAULT && ball.SpeedX < ALLOWED_FAULT) {
			ball.SpeedX = 0.0;
			ball.BoostX = 0.0;
		}
	}
	ball.SpeedX += ball.BoostX;

	if (ball.directionY == NONE_Y) {
		if (ball.SpeedY > (-1) * ALLOWED_FAULT && ball.SpeedY < ALLOWED_FAULT) {
			ball.SpeedY = 0.0;
			ball.BoostY = 0.0;
		}
	}
	ball.SpeedY += ball.BoostY;
}

void RecalculateBallPosition()
{
	if (LeftHitten() || RightHitten()) {
		ball.SpeedX *= -1;
		ball.BoostX *= -1;
	}
	if (TopHitten() || BottomHitten()) {
		ball.SpeedY *= -1;
		ball.BoostY *= -1;
	}
	ball.X += ball.SpeedX;
	ball.Y += ball.SpeedY;
}

BOOL LeftHitten()
{
	if (ball.X - ball.Radius <= 0) { return TRUE; }	else { return FALSE; }
}

BOOL RightHitten()
{
	if (ball.X + ball.Radius >= WND_WIDTH) { return TRUE; }	else { return FALSE; }
}

BOOL TopHitten()
{
	if (ball.Y - ball.Radius <= 0) { return TRUE; }	else { return FALSE; }
}

BOOL BottomHitten()
{
	if (ball.Y + ball.Radius >= WND_HEIGHT) { return TRUE; } else { return FALSE; }
}