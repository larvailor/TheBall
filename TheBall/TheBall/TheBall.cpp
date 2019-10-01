#include "TheBall.h"
#include "resource.h"


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

	switch (message) {
	case WM_CREATE:
		LoadResources();
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
			SetUpLeftHit();
			break;
		case VK_RIGHT:
			SetUpRightHit();
			break;
		case VK_UP:
			SetUpUpHit();
			break;
		case VK_DOWN:
			SetUpDownHit();
			break;
		}
		break;
	case WM_MOUSEWHEEL:
		switch (GET_KEYSTATE_WPARAM(wParam)) {
		case MK_SHIFT:
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) { SetUpRightHit(); } else { SetUpLeftHit(); }
			break;
		default:
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) { SetUpUpHit(); } else { SetUpDownHit(); }
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void LoadResources()
{
	hBmpBall = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
}

void InitializeBall(HWND hWnd)
{
	BITMAP bmp;
	GetObject(hBmpBall, sizeof(BITMAP), (LPSTR)& bmp);
	ball.X = WND_WIDTH / 2;
	ball.Y = WND_HEIGHT / 2;
	ball.Radius = bmp.bmWidth / 2;
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
	BOOL result = DrawBitmap(hdc, ball.X - ball.Radius, ball.Y - ball.Radius, hBmpBall);
	if (result == FALSE) {
		MessageBox(NULL,
			_T("Call to DrawBitmap failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
	}
	EndPaint(hWnd, &ps);
}

BOOL DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap)
{
	HBITMAP hBmp, hBmpOld;
	HDC hMemDC;
	BITMAP bmp;
	POINT ptSize, ptOrg;

	hMemDC = CreateCompatibleDC(hDC); // creating memory context, that is compatible with hDC
	if (hMemDC == NULL) {
		return FALSE;
	}


	hBmpOld = (HBITMAP)SelectObject(hMemDC, hBitmap); // select image into the context	
													  // this function returns ID of the BMP that was loaded into the memory context earlier
	if (!hBmpOld) {
		return FALSE;
	}

	SetMapMode(hMemDC, GetMapMode(hDC)); // synchronizing of the memory context and showing context
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)& bmp);
	
	ptSize.x = bmp.bmWidth;
	ptSize.y = bmp.bmHeight;
	DPtoLP(hDC, &ptSize, 1); // convert units into logical units

	ptOrg.x = 0;
	ptOrg.y = 0;
	DPtoLP(hDC, &ptSize, 1); 

	TransparentBlt(hDC, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, ptSize.x, ptSize.y, RGB(255, 255, 255));

	SelectObject(hMemDC, hBmpOld); // restore memory context
	DeleteObject(hMemDC);
}

void RecalculateBallSpeed()
{
	if (ball.directionX != NONE_X) {
		ball.SpeedX = ball.SpeedX - ball.BoostX;
		if (ball.SpeedX > (-1) * ALLOWED_FAULT && ball.SpeedX < ALLOWED_FAULT) {
			ball.SpeedX = 0.0;
			ball.BoostX = 0.0;
			ball.directionX = NONE_X;
		}
	}

	if (ball.directionY != NONE_Y) {
		ball.SpeedY = ball.SpeedY - ball.BoostY;
		if (ball.SpeedY > (-1) * ALLOWED_FAULT && ball.SpeedY < ALLOWED_FAULT) {
			ball.SpeedY = 0.0;
			ball.BoostY = 0.0;
			ball.directionY = NONE_Y;
		}
	}
}

void RecalculateBallPosition()
{
	switch (ball.directionX) {
	case LEFT:
		ball.X -= ball.SpeedX - ball.BoostX / 2;
		if (LeftHitten()) {	ball.directionX = RIGHT; }
		break;
	case RIGHT:
		ball.X += ball.SpeedX - ball.BoostX / 2;
		if (RightHitten()) { ball.directionX = LEFT; }
		break;
	case NONE_X:
		break;
	}

	switch (ball.directionY) {
	case UP:
		ball.Y -= ball.SpeedY - ball.BoostY / 2;
		if (TopHitten()) { ball.directionY = DOWN; }
		break;
	case DOWN:
		ball.Y += ball.SpeedY - ball.BoostY / 2;
		if (BottomHitten()) { ball.directionY = UP; }
		break;
	case NONE_Y:
		break;
	}
}

void SetUpLeftHit()
{
	ball.directionX = LEFT;
	ball.SpeedX = START_SPEED;
	ball.BoostX = BOOST;
}

void SetUpRightHit()
{
	ball.directionX = RIGHT;
	ball.SpeedX = START_SPEED;
	ball.BoostX = BOOST;
}

void SetUpUpHit()
{
	ball.directionY = UP;
	ball.SpeedY = START_SPEED;
	ball.BoostY = BOOST;
}

void SetUpDownHit()
{
	ball.directionY = DOWN;
	ball.SpeedY = START_SPEED;
	ball.BoostY = BOOST;
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