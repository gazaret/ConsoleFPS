#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>

#include <stdio.h>
#include <Windows.h>

using namespace std;

int const SCREEN_WIDTH = 120;
int const SCREEN_HEIGHT = 40;
int const SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

int const MAP_WIDTH = 16;
int const MAP_HEIGHT = 16;
int const MAP_SIZE = MAP_WIDTH * MAP_HEIGHT;

float MAX_DEPTH = 16.0f;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerAngle = 0.0f;

float fFOV = 3.14159f / 4.0f;

wstring generateMap() {
	wstring map;

	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	return map;
}

int main() {
	wchar_t* p_screen = new wchar_t[SCREEN_SIZE];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(hConsole);

	DWORD dwBytesWritten = 0;

	wstring map = generateMap();

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (1) {
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		// Управление
		if (GetAsyncKeyState((unsigned short)'A') || GetAsyncKeyState(VK_LEFT)) {
			fPlayerAngle -= (0.1f) * fElapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'D') || GetAsyncKeyState(VK_RIGHT)) {
			fPlayerAngle += (0.1f) * fElapsedTime;
		}

		for (int x = 0; x < SCREEN_WIDTH; x++) {
			float fRayAngle = (fPlayerAngle - fFOV / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * fFOV;

			float fDistanceToWall = 0.0f;

			bool bHitWall = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			// пробрасываем лучи
			while (!bHitWall && fDistanceToWall < MAX_DEPTH) {
				fDistanceToWall += 0.1f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);


				// луч вышел за максимальные границы карты
				if (nTestX < 0 || nTestX > MAP_WIDTH || nTestY < 0 || nTestY > MAP_HEIGHT) {
					bHitWall = true;
					fDistanceToWall = MAX_DEPTH;
				}
				else {
					// луч встретил стену
					if (map.c_str()[nTestX * MAP_WIDTH + nTestY] == '#') {
						bHitWall = true;
					}
				}
			}

			int nCeiling = (float)(SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / ((float)fDistanceToWall);
			int nFloor = SCREEN_HEIGHT - nCeiling;

			short nShade = ' ';

			// Отрисовка стены с помощью ASCII
			if (fDistanceToWall <= MAX_DEPTH / 4.0f)	 nShade = 0x2588; // Близкое расстояние
			else if (fDistanceToWall < MAX_DEPTH / 3.0f) nShade = 0x2593;
			else if (fDistanceToWall < MAX_DEPTH / 2.0f) nShade = 0x2592;
			else if (fDistanceToWall < MAX_DEPTH)		 nShade = 0x2591;
			else										 nShade = ' ';    // Далекое расстояние

			for (int y = 0; y < SCREEN_HEIGHT; y++) {
				if (y <= nCeiling) {
					// Потолок
					p_screen[y * SCREEN_WIDTH + x] = ' ';
				}
				else if (y > nCeiling && y <= nFloor) {
					// Стена
					p_screen[y * SCREEN_WIDTH + x] = nShade;
				}
				else {
					// Пол
					p_screen[y * SCREEN_WIDTH + x] = ' ';
				}
			}
		}

		// Отображаем карту
		for (int nx = 0; nx < MAP_WIDTH; nx++)
			for (int ny = 0; ny < MAP_WIDTH; ny++)
			{
				p_screen[(ny + 1) * SCREEN_WIDTH + nx] = map[ny * MAP_WIDTH + nx];
			}
		p_screen[((int)fPlayerX + 1) * SCREEN_WIDTH + (int)fPlayerY] = 'P';

		p_screen[SCREEN_SIZE - 1] = '\0';

		WriteConsoleOutputCharacter(hConsole, p_screen, SCREEN_SIZE, { 0,0 }, &dwBytesWritten);
	}

	return 0;
}
