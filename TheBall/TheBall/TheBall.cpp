#include <windows.h>
#include <stdlib.h>
#include <tchar.h>


////////////////////////////////
// User types
enum Direction { LEFT, RIGHT, UP, DOWN, NONE };

typedef struct ball {
	float X, Y, Radius;
	float Speed, Boost;
} Ball;


////////////////////////////////
// Global variables
//
constexpr auto WND_WIDTH = 1000.0;
constexpr auto WND_HEIGHT = 600.0;
constexpr auto BOOST = 0.5;
constexpr auto COUNTER_BOOST = -0.5;

static TCHAR szWindowClass[] = _T("TheBall");
static TCHAR szTitle[] = _T("The Ball");
HINSTANCE hInst;
Ball ball;
Direction direction = NONE;
BOOL wallHitten = FALSE;
int timer = 1;


////////////////////////////////
// Forward declarations of functions included in this code module:
//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitializeBall(HWND);
void DrawBall(HWND);
void RecalculateBallPosition(Direction);


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
		direction = NONE;
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
		if (ball.Speed + ball.Boost >= 0) { 
			ball.Speed = ball.Speed + ball.Boost;
		} 
		else {
			ball.Speed = 0.0;
			ball.Boost = 0.0;
			direction = NONE;
		}
		RecalculateBallPosition(direction);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_KEYDOWN:
		ball.Boost = BOOST;
		switch (wParam)
		{
		case VK_LEFT:
			direction = LEFT;
			break;
		case VK_RIGHT:
			direction = RIGHT;
			break;
		case VK_UP:
			direction = UP;
			break;
		case VK_DOWN:
			direction = DOWN;
			break;
		}
		break;
	case WM_KEYUP:
		ball.Boost = COUNTER_BOOST;
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
	ball.Speed = 0.0;
	ball.Boost = 0.0;
}

void DrawBall(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	Ellipse(hdc, (int)(ball.X - ball.Radius), (int)(ball.Y - ball.Radius), (int)(ball.X + ball.Radius), (int)(ball.Y + ball.Radius));
	EndPaint(hWnd, &ps);
}

void RecalculateBallPosition(Direction direction)
{
	switch (direction)
	{
	case LEFT:
		ball.X = ball.X - ball.Speed;
		break;
	case RIGHT:
		ball.X = ball.X + ball.Speed;
		break;
	case UP:
		ball.Y = ball.Y - ball.Speed;
		break;
	case DOWN:
		ball.Y = ball.Y + ball.Speed;
		break;
	case NONE:
		break;
	}
}
