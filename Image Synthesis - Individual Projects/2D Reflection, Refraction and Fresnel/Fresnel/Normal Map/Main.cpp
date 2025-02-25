﻿#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include </home/user/Desktop/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include </home/user/Desktop/stb_image_write.h>
#include </home/user/Desktop/stb_include.h>
#include <string>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
//int width, height;
unsigned char *env_img;
unsigned char *fore_img;

unsigned char *normal_img;

unsigned char *env_arr;
unsigned char *fore_arr;

unsigned char *result;

unsigned char *normal_arr;
unsigned char *normal_map;


float *normal_mod;

int width = 300, height = 300, channels1, channels2, channels3;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "pixmap."
// Then, "glutDisplayFunc" below will use pixmap to display the pixel colors.
// =============================================================================
float magnitude(float a, float b, float c)
{
	return pow(pow(a, 2) + pow(b, 2) + pow(c, 2), 0.5);
}


float crop(float min, float max, float x) {
	x = (x - min) / (max - min);
	if (x > 1)
		x = 1;
	if (x < 0)
		x = 0;
	return -2*pow(x,3)+3*pow(x,2);
}
void setPixels()
{

	stbi_set_flip_vertically_on_load(true);
	
	//load normal map
	normal_img = stbi_load("NormalMap2.png", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			normal_arr[i] = normal_img[i++];
			normal_arr[i] = normal_img[i++];
			normal_arr[i] = normal_img[i++];
		}
	}

	//convert normal map to vector field
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			normal_mod[i] = (float)(2*(int)normal_arr[i] / 255.0-1);
			i++;
			normal_mod[i] = (float)(2 * (int)normal_arr[i] / 255.0 - 1);
			i++;
			normal_mod[i] = (float)(2 * (int)normal_arr[i] / 255.0 - 1);
			i++;
		}
	}

	//load background image
	env_img = stbi_load("pic2.jpg", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			env_arr[i] = env_img[i++];
			env_arr[i] = env_img[i++];
			env_arr[i] = env_img[i++];
		}
	}

	//load foreground image
	fore_img = stbi_load("env.jpg", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			fore_arr[i] = fore_img[i++];
			fore_arr[i] = fore_img[i++];
			fore_arr[i] = fore_img[i++];
		}
	}

	int d = 24;
	float eta2 = 1.5;
	float eta1 = 1;
	float eta = eta2 / eta1;
	float a = log2(eta);
	float F0 = 0.1;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;

			float T = 0.5 * (normal_mod[i] + normal_mod[i + 1]) + 0.5;
			float B = 1 - normal_mod[i + 2];
			float a = log2(eta);

			int refl_indx = abs(((2 * normal_mod[i] * normal_mod[i + 2] * d) / (-1 + 2 * normal_mod[i + 2] * normal_mod[i + 2])) + x);
			int refl_indy = abs(((2 * normal_mod[i + 1] * normal_mod[i + 2] * d) / (-1 + 2 * normal_mod[i + 2] * normal_mod[i + 2])) + y);
			refl_indx = refl_indx % 300;
			refl_indy = refl_indy % 300;
			int refl_ind = (refl_indy * width + refl_indx) * 3;


			float delx = (-normal_mod[i] * d * a) / (a - 1 - (normal_mod[i + 2] * a));
			float dely = (-normal_mod[i + 1] * d * a) / (a - 1 - (normal_mod[i + 2] * a));
			int refr_indx = x + delx;
			int refr_indy = y + dely;
			refr_indx = refr_indx % 300;
			refr_indy = refr_indy % 300;
			int refr_ind = (refr_indy * width + refr_indx) * 3;

			float cosi = normal_mod[i + 2];
			float alpha = 1 - cosi;

			float alpha1 = 0.2;
			float alpha2 = 0.25;
			float alpha3 = 0.9;

			float F;

			if (alpha < alpha1)
			{
				F = F0 * (1 - (alpha / alpha1));
			}
			else
			{
				F = (alpha - alpha2) / (alpha3 - alpha2);
				if (F < 0) F = 0;
				if (F > 1) F = 1;
			}
			

			result[i] = fore_arr[refr_ind++] * (1 - F) + env_arr[refl_ind++] * F;
			i++;

			result[i] = fore_arr[refr_ind++] * (1 - F) + env_arr[refl_ind++] * F;
			i++;

			result[i] = fore_arr[refr_ind++] * (1 - F) + env_arr[refl_ind++] * F;
		}
	}
}


// =============================================================================
// OpenGL Display and Mouse Processing Functions.
//
// You can read up on OpenGL and modify these functions, as well as the commands
// in main(), to perform more sophisticated display or GUI behavior. This code
// will service the bare minimum display needs for most assignments.
// =============================================================================
static void windowResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (w / 2), 0, (h / 2), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
static void windowDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE,result);
	glFlush();
}
static void processMouse(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
		exit(0);               // Exit on mouse click.
}
static void init(void)
{
	glClearColor(1, 1, 1, 1); // Set background color.
}

// =============================================================================
// main() Program Entry
// =============================================================================
int main(int argc, char *argv[])
{

	//initialize the global variables
	width = 300;
	height = 300;
	env_arr = new unsigned char[300 * 300 * 3];
	fore_arr = new unsigned char[300 * 300 * 3];
	normal_arr = new unsigned char[300 * 300 * 3];
	normal_mod = new float[300 * 300 * 3];
	result = new unsigned char[300 * 300 * 3];

	setPixels();

	// OpenGL Commands:
	// Once "glutMainLoop" is executed, the program loops indefiniteL_y to all
	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Homework Zero");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; //This line never gets reached. We use it because "main" is T_ype int.
}