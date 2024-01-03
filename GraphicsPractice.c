#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <math.h>

// Screen resolution
#define width 100
#define height 50

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
int xDisp = 0;
int yDisp = 0;
int zDisp = 80;

const int zProj = 50; // Distance between origin and screen
const float R = 30; // Radius of sphere

// Light vector
float xLight = 0;
float yLight = 1;
float zLight = -1;

// Controls frequency of data sampling
const float deltaTheta = (float)0.01;
const float deltaPhi = (float)0.01;

int keys[NUM_KEYS] = { KEY_W, KEY_A, KEY_S, KEY_D, KEY_Q, KEY_E,
				 KEY_I, KEY_J, KEY_K, KEY_L, KEY_U, KEY_O,
				 KEY_0 };

int main() {
	char proj[height][width]; // Screen
	float zVal[height][width]; // z-value of points

	int choice = 0;
	
	INPUT virtualEnter;
	virtualEnter.type = INPUT_KEYBOARD;
	virtualEnter.ki.wScan = 0;
	virtualEnter.ki.time = 0;
	virtualEnter.ki.dwExtraInfo = 0;
	virtualEnter.ki.wVk = 0x0D;
	virtualEnter.ki.dwFlags = 0;

	while (choice != KEY_0)
	{
		// Reset screen and z-values
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
			{
				proj[i][j] = ' ';
				zVal[i][j] = 0;
			}
		
		float ooMagLight = 1 / sqrtf(xLight * xLight + yLight * yLight + zLight * zLight); // Reciprocal of magnitude of light vector

		// Sample points on sphere by rotating in descrete intervals on domain: 0 < THETA < pi, 0 < PHI < 2pi
		for (float THETA = 0.0; THETA < 3.14; THETA += deltaTheta)
		{
			float sinTHETA = sinf(THETA), cosTHETA = cosf(THETA);

			for (float PHI = 0.0; PHI < 6.28; PHI += deltaPhi)
			{
				float sinPHI = sinf(PHI), cosPHI = cosf(PHI);

				// Calculate coords of sampled point
				float xSphere = R * cosTHETA + xDisp;
				float ySphere = R * cosPHI * sinTHETA + yDisp;
				float zSphere = R * sinPHI * sinTHETA + zDisp;
				float ooz = 1 / zSphere; // Reciprocal of z coord

				//Calculate projected coords
				int xProj = (int)(xSphere * zProj * ooz + width / 2);
				int yProj = (int)(-ySphere * zProj * ooz / 2 + height / 2);

				if (xProj >= 0 && xProj < width && yProj >= 0 && yProj < height) // Check if coords are within bounds of screen
				{
					// Check if sampled point is closer to origin than existing point plotted on same screen coords
					if (ooz > zVal[yProj][xProj])
					{
						zVal[yProj][xProj] = ooz; // Record z-value of plotted point

						// Calculate luminence, i.e., dot product of unit light vector and vector normal of sphere at sampled point
						float L = ooMagLight * (xLight * cosTHETA + yLight * cosPHI * sinTHETA + zLight * sinPHI * sinTHETA);

						if (L > 0) { // Check for positive luminance, i.e., sphere faces towards light at sampled point
							int indexL = (int)(L * 10);

							proj[yProj][xProj] = ".,-~:;+*#@"[indexL]; // Plotted point is "brighter" if luminence is higher
						}
						else // Negative luminence, i.e., sphere faces away from lightt at sampled point
							proj[yProj][xProj] = ' ';
					}
				}
			}
		}

		printf("\x1b[H");
		char str[height * (width + 1) + 1] = { 0 };
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				strncat(str, &proj[i][j], 1);
			strcat(str, "\n");
		}
		printf("%s", str);
		
		while (choice == 0)
		{
			for (int i = 0; i < NUM_KEYS; i++)
				if (0x8000 & GetAsyncKeyState(keys[i]))
				{
					choice = keys[i];
					break;
				}
		}

		switch (choice)
		{
			default:
				break;
			case KEY_0: // exit
				break;
			case KEY_W: // up
				yDisp += 5;
				break;
			case KEY_A: // left
				xDisp -= 5;
				break;
			case KEY_S: // down
				yDisp -= 5;
				break;
			case KEY_D: // right
				xDisp += 5;
				break;
			case KEY_Q: // forward
				zDisp += 5;
				break;
			case KEY_E: // backward
				zDisp -= 5;
				break;
			case KEY_I: // up (light)
				if (yLight < 1)
					yLight += (float)0.2;
				break;
			case KEY_J: // left (light)
				if (xLight > -1)
					xLight -= (float)0.2;
				break;
			case KEY_K: // down (light)
				if (yLight > -1)
					yLight -= (float)0.2;
				break;
			case KEY_L: // right (light)
				if (xLight < 1)
					xLight += (float)0.2;
				break;
			case KEY_U: // forward (light)
				if (zLight < 1)
					zLight += (float)0.2;
				break;
			case KEY_O: // backward (light)
				if (zLight > -1)
					zLight -= (float)0.2;
				break;
		}

		choice = 0;
		Sleep(50);
	}

	return 0;
}