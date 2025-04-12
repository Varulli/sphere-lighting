#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#define PI 3.141593f

// Screen resolution
#define width 60
#define height 30

// Key-code macros
#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define KEY_Q 0x51
#define KEY_E 0x45
#define KEY_I 0x49
#define KEY_J 0x4A
#define KEY_K 0x4B
#define KEY_L 0x4C
#define KEY_U 0x55
#define KEY_O 0x4F
#define KEY_0 0x30

#define NUM_KEYS 13

// Displacement of sphere from origin
float xDisp = 0.0f;
float yDisp = 0.0f;
float zDisp = 80.0f;

float dispInc = 0.2f;

const int zProj = 50; // Distance between origin and screen
const float R = 30;	  // Radius of sphere

// Light vector
float xLight = 0;
float yLight = 0;
float zLight = -1;

const float sinLightInc = 0.006283144f;
const float cosLightInc = 0.9999803f;

int rotationCount = 0;
const int rotationLimit = 500;

// Controls frequency of data sampling
const float deltaTheta = 0.01f;
const float deltaPhi = 0.02f;

int keys[NUM_KEYS] = {KEY_W, KEY_A, KEY_S, KEY_D, KEY_Q, KEY_E,
					  KEY_I, KEY_J, KEY_K, KEY_L, KEY_U, KEY_O,
					  KEY_0};

void normalizeLight()
{
	float ooMagLight = 1 / sqrtf(xLight * xLight + yLight * yLight + zLight * zLight);
	xLight *= ooMagLight;
	yLight *= ooMagLight;
	zLight *= ooMagLight;
}

int main()
{
#ifdef _WIN32
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	if (GetConsoleMode(console, &mode))
	{
		SetConsoleMode(console, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
#endif

	// Use alternate buffer
	printf("\x1b[?1049h\x1b[?25l");
	fflush(stdout);

	char proj[height][width];  // Screen
	float zVal[height][width]; // z-value of points

	int choice = 0;

	while (choice != KEY_0)
	{
		// Reset screen and z-values
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
			{
				proj[i][j] = ' ';
				zVal[i][j] = 0;
			}

		// Sample points on sphere by rotating in descrete intervals on domain: 0 < THETA < pi, 0 < PHI < 2pi
		for (float THETA = 0.0; THETA < PI; THETA += deltaTheta)
		{
			float sinTHETA = sinf(THETA), cosTHETA = cosf(THETA);

			for (float PHI = 0.0; PHI < 2 * PI; PHI += deltaPhi)
			{
				float sinPHI = sinf(PHI), cosPHI = cosf(PHI);

				// Calculate coords of sampled point
				float xSphere = R * cosTHETA + xDisp;
				float ySphere = R * cosPHI * sinTHETA + yDisp;
				float zSphere = R * sinPHI * sinTHETA + zDisp;
				float ooz = 1 / zSphere; // Reciprocal of z coord

				// Calculate projected coords
				int xProj = (int)(xSphere * zProj * ooz + width / 2);
				int yProj = (int)(-ySphere * zProj * ooz / 2 + height / 2);

				if (xProj >= 0 && xProj < width && yProj >= 0 && yProj < height) // Check if coords are within bounds of screen
				{
					// Check if sampled point is closer to origin than existing point plotted on same screen coords
					if (ooz > zVal[yProj][xProj])
					{
						zVal[yProj][xProj] = ooz; // Record z-value of plotted point

						// Calculate luminence, i.e., dot product of unit light vector and vector normal of sphere at sampled point
						float L = xLight * cosTHETA + yLight * cosPHI * sinTHETA + zLight * sinPHI * sinTHETA;

						if (L > 0)
						{ // Check for positive luminance, i.e., sphere faces towards light at sampled point
							int indexL = (int)(L * 10);

							proj[yProj][xProj] = ".,-~:;+*#@"[indexL]; // Plotted point is "brighter" if luminence is higher
						}
						else // Negative luminence, i.e., sphere faces away from lightt at sampled point
							proj[yProj][xProj] = ' ';
					}
				}
			}
		}

		char str[height * (width + 1) + 1] = {0};
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				strncat(str, &proj[i][j], 1);
			strcat(str, "\n");
		}
		printf("%s\x1b[H", str);
		fflush(stdout);

		choice = 0;
		while (choice == 0)
		{
			for (int i = 0; i < NUM_KEYS; i++)
				if (0x8000 & GetAsyncKeyState(keys[i]))
				{
					choice = keys[i];
					break;
				}
		}

		float x, y, z;
		switch (choice)
		{
		default:
			break;
		case KEY_0: // exit
			break;
		case KEY_W: // up
			yDisp += dispInc;
			break;
		case KEY_A: // left
			xDisp -= dispInc;
			break;
		case KEY_S: // down
			yDisp -= dispInc;
			break;
		case KEY_D: // right
			xDisp += dispInc;
			break;
		case KEY_Q: // forward
			zDisp += dispInc;
			break;
		case KEY_E: // backward
			zDisp -= dispInc;
			break;
		case KEY_I: // up (light)
			y = yLight * cosLightInc - zLight * sinLightInc;
			z = yLight * sinLightInc + zLight * cosLightInc;
			yLight = y;
			zLight = z;

			if (++rotationCount >= rotationLimit)
				normalizeLight();
			break;
		case KEY_K: // down (light)
			y = yLight * cosLightInc + zLight * sinLightInc;
			z = -yLight * sinLightInc + zLight * cosLightInc;
			yLight = y;
			zLight = z;

			if (++rotationCount >= rotationLimit)
				normalizeLight();
			break;
		case KEY_J: // left (light)
			x = xLight * cosLightInc + zLight * sinLightInc;
			z = -xLight * sinLightInc + zLight * cosLightInc;
			xLight = x;
			zLight = z;

			if (++rotationCount >= rotationLimit)
				normalizeLight();
			break;
		case KEY_L: // right (light)
			x = xLight * cosLightInc - zLight * sinLightInc;
			z = xLight * sinLightInc + zLight * cosLightInc;
			xLight = x;
			zLight = z;

			if (++rotationCount >= rotationLimit)
				normalizeLight();
			break;
		}
	}

	printf("\x1b[?25h\x1b[?1049l");
	fflush(stdout);

	return 0;
}