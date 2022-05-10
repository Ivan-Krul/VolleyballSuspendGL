#include <windows.h>
#include <gl/gl.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

#pragma comment(lib, "opengl32.lib")

#define WINDOW_X 1200
#define WINDOW_Y 700
#define WINDOW_RELATION (float(WINDOW_X)/float(WINDOW_Y))

#define GAME_GRAVITY_BALL 0.002f
#define GAME_GRAVITY_PLAYER 0.004f
#define GAME_NET_HEIGTH -0.2f
#define GAME_BALL_BOUNCINESS 0.07f

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

struct TBall {
	float x;
	float y;
	float dx;
	float dy;
	float r;

	void Init(float x, float y, float dx, float dy, float r) {
		this->x = x;
		this->y = y;
		this->dx = dx;
		this->dy = dy;
		this->r = r;
	}
};

TBall ball;
TBall player[2];
COORD check = { 0,0 };

bool isCross(float x1, float y1, float r, float x2, float y2) {
	return pow(x1 - x2, 2) + pow(y1 - y2, 2) < r * r;
}

void Mirror(TBall& tball, float x, float y,float speed) {
	float tballVec = atan2(tball.dx, tball.dy);
	float crossVec = atan2(tball.x-x, tball.y-y);

	float resVec = speed==0 ? M_PI - tballVec + crossVec * 2.0f:crossVec;
	float Speed = speed == 0 ? sqrt(pow(tball.dx, 2) + pow(tball.dy, 2)) : speed;

	tball.dx = sin(resVec) * Speed;
	tball.dy = cos(resVec) * Speed;
}

void Reflect(float& da, float& a, bool cond, float wall) {
	if (!cond) return;
	da *= -0.85f;
	a = wall;
}

void PlayerMove(TBall& player, char left, char rigth, char jump, float wl1, float wl2) {
	static float speed = 0.05f;

	if (GetAsyncKeyState(left)) player.x -= speed;
	else if (GetAsyncKeyState(rigth)) player.x += speed;

	if (player.x - player.r < wl1) player.x = wl1 + player.r;
	if (player.x + player.r > wl2) player.x = wl2 - player.r;

	if (GetAsyncKeyState(jump) && (player.y < -0.99f + player.r))
		player.dy = speed * 1.4f;
	player.y += player.dy;
	player.dy -= GAME_GRAVITY_PLAYER;
	if (player.y - player.r < -1) {
		player.dy = 0;
		player.y = -1 + player.r;
	}

	if (isCross(player.x, player.y, player.r + ball.r, ball.x, ball.y)) {
		Mirror(ball, player.x, player.y, GAME_BALL_BOUNCINESS);
		ball.dy += 0.01f;
	}
}

void TBallMove(TBall& tball) {
	tball.x += tball.dx;
	tball.y += tball.dy;

	Reflect(tball.dy, tball.y, (tball.y < tball.r - 1), tball.r - 1);
	Reflect(tball.dy, tball.y, (tball.y > 1 - tball.r), 1 - tball.r);
	tball.dy -= GAME_GRAVITY_BALL;

	Reflect(tball.dx, tball.x, (tball.x < tball.r - WINDOW_RELATION), tball.r - WINDOW_RELATION);
	Reflect(tball.dx, tball.x, (tball.x > WINDOW_RELATION - tball.r), WINDOW_RELATION - tball.r);

	if (tball.y < GAME_NET_HEIGTH) {
		if(tball.x>0)
			Reflect(tball.dx, tball.x, (tball.x < +tball.r), +tball.r);
		else
			Reflect(tball.dx, tball.x, (tball.x > -tball.r), -tball.r);
	}
	else {
		if (isCross(tball.x, tball.y, tball.r, 0, GAME_NET_HEIGTH))
			Mirror(tball, 0, GAME_NET_HEIGTH,0);
	}
}

void GameInit() {
	ball.Init(0.1f,0.5f,0.0f,0.0f,0.2f);
	player[0].Init(-1, 0, 0.0f, 0.0f, 0.2f);
	player[1].Init(1, 0, 0.0f, 0.0f, 0.2f);
}

bool IsWork() {
	return !(ball.y - ball.r < -0.95);
}

void line(double x1, double y1, double x2, double y2) { glVertex2f(x1, y1);glVertex2f(x2, y2); }

void ShowNumber(int num) {

	glLineWidth(3);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	if (num == '0' || num == '4' || num == '5' || num == '6' || num == '8' || num == '9' || num == 'L' || num == 'V' ) line(0.15, 0.85, 0.15, 0.5);
	if (num == '0' || num == '2' || num == '6' || num == '8' || num == 'L') line(0.15, 0.5, 0.15, 0.15);
	if (num == '0' || num == '2' || num == '3' || num == '5' || num == '6' || num == '7' || num == '8' || num == '9') line(0.15, 0.85, 0.85, 0.85);
	if (num == '2' || num == '3' || num == '4' || num == '5' || num == '6' || num == '8' || num == '9' || num == ':') line(0.15, 0.5, 0.85, 0.5);
	if (num == '0' || num == '2' || num == '3' || num == '5' || num == '6' || num == '8' || num == '9' || num == 'L' || num == 'V' || num == ':') line(0.15, 0.15, 0.85, 0.15);
	if (num == '0' || num == '1' || num == '2' ||  num == '3' || num == '4' || num == '7' || num == '8' || num == '9' || num == 'V') line(0.85, 0.5, 0.85, 0.85);
	if (num == '0' || num == '1' || num == '3' || num == '4' || num == '5' || num == '6' || num == '7' || num == '8' || num == '9' || num == 'V') line(0.85, 0.5, 0.85, 0.15);
	glEnd();
}

void DrawCircle(int poly) {
	float a = 2.0f * M_PI / float(poly);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0,0);
	for (int i = 0;i <= poly;i++) {
		float x = cos(i * a);
		float y = sin(i * a);
		glVertex2f(x,y);
	}

	glEnd();
}

void DrawTBall(TBall tball) {
	glPushMatrix();
	glTranslatef(tball.x, tball.y,0.0f);
	glScalef(tball.r, tball.r,0.0f);

	DrawCircle(20);

	glPopMatrix();
}

void DrawSquare(float x,float y,float dx,float dy) {
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(x, y);
	glVertex2f(x + dx, y);
	glVertex2f(x + dx, y + dy);
	glVertex2f(x, y + dy);

	glEnd();
}

void DrawBG() {
	glColor3ub(69, 197, 230);
	DrawSquare(-WINDOW_RELATION,0.0f,WINDOW_RELATION*2.0f,1.0f);
	glColor3ub(23, 29, 209);
	DrawSquare(-WINDOW_RELATION, -0.4f,WINDOW_RELATION * 2.0f, 0.4f);
	glColor3ub(214, 203, 101);
	DrawSquare(-WINDOW_RELATION, -1.0f,WINDOW_RELATION*2.0f, 0.6f);
}

void Paint() {
	DrawBG();

	glColor3ub(75, 103, 163);
	DrawTBall(ball);

	glColor3ub(21, 207, 89);
	DrawTBall(player[0]);

	glColor3ub(109, 22, 222);
	DrawTBall(player[1]);

	glColor3ub(79, 19, 54);
	DrawSquare(-0.01, GAME_NET_HEIGTH, 0.02, -(1 - GAME_NET_HEIGTH));

	std::string str = std::to_string(check.X) + ':' + std::to_string(check.Y);
	for (int i=0;i<str.size();i++) {
		glPushMatrix();
		glTranslatef(0 - (i * 0.1), 0.8f, 0.0f);
		glScalef(0.1f, 0.2f, 0.0f);
		ShowNumber(str[i]);
		glEnd();
		glPopMatrix();
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;
	float theta = 0.0f;

	/* register window class */
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0,
		L"GLSample",
		L"VolleyballSuspendGL",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_X,
		WINDOW_Y,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);

	/* enable OpenGL for the window */
	EnableOpenGL(hwnd, &hDC, &hRC);

	glScalef(1.0/WINDOW_RELATION, 1.0f, 0.0f);
	GameInit();

	/* program main loop */
	while (!bQuit)
	{
		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/* OpenGL animation code goes here */

			if (GetAsyncKeyState(VK_F4)) { GameInit(); check = { 0,0 }; Sleep(500); }

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);


			if (IsWork()) {
				TBallMove(ball);
				PlayerMove(player[0], 'A', 'D', 'W', -WINDOW_RELATION, 0);
				PlayerMove(player[1], VK_LEFT, VK_RIGHT, VK_UP, 0, WINDOW_RELATION);
			}
			else {
				if (ball.x < 0.0f) check.X++;
				else check.Y++;

				GameInit();

				Sleep(1000);
			}
			Paint();

			SwapBuffers(hDC);

			theta += 1.0f;
			Sleep(1);
		}
	}

	/* shutdown OpenGL */
	DisableOpenGL(hwnd, hDC, hRC);

	/* destroy the window explicitly */
	DestroyWindow(hwnd);

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}

