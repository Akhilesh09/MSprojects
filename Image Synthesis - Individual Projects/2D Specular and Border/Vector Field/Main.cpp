#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <string>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
//int width, height;
unsigned char *normal_arr;
unsigned char *normal_map;


double *normal_mod;

unsigned char *result;

int width = 300, height = 300, channels1, channels2, channels3;



// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "pixmap."
// Then, "glutDisplayFunc" below will use pixmap to display the pixel colors.
// =============================================================================

double crop(double min, double max, double x) {
	x = (x - min) / (max - min);
	if (x > max)
		x = 1;
	if (x < min)
		x = 0;
	return x;
}
void setPixels()
{
	//load normal map
	normal_map = stbi_load("nm.jpg", &width, &height, &channels3, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			normal_arr[i] = normal_map[i++];
			normal_arr[i] = normal_map[i++];
			normal_arr[i] = normal_map[i++];
		}
	}

	//convert normal map to vector field
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			normal_mod[i] = (double)(2 * normal_arr[i] / 255.0 - 1);
			i++;
			normal_mod[i] = (double)(2 * normal_arr[i] / 255.0 - 1);
			i++;
			normal_mod[i] = (double)(2 * normal_arr[i] / 255.0 - 1);
			i++;
		}
	}

	double L_x[100], L_y[100], lz[100];


	for (int i = 0; i < 100; i++)
	{
		L_x[i] = sin(i + 10);
		L_y[i] = cos(i + 10);
	}


	for (int l = 0; l < 5; l++)
	{
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				
				// Assumed light at (1,1,0)
				double T = 0.5*(normal_mod[i]) + 0.5;
				double S = 2 * normal_mod[i + 2] * (normal_mod[i]);
				double B = 1 - normal_mod[i + 2];

				T = crop(0, 1, T);
				S = crop(0, 1, S);

				B = crop(0.2, 0.5, B);

				if ((int)normal_arr[i + 2] != 1) {
					result[i] = (255 * (T)+255 * (1 - T));
					result[i] = (result[i] * (1 - S) + 255 * S);

					result[i] = (result[i] * (1 - B) + 0 * B);
				}
				else
					result[i] = (255 * 255) / 255;
				i++;

				if ((int)normal_arr[i + 1] != 1) {
					result[i] = (255 * (T)+0 * (1 - T));
					result[i] = (result[i] * (1 - S) + 255 * S);

					result[i] = (result[i] * (1 - B) + 255 * B);
				}
				else
					result[i] = (0 * 255) / 255;
				i++;

				if ((int)normal_arr[i] != 1) {
					result[i] = (0 * (T)+0 * (1 - T));
					result[i] = (result[i] * (1 - S) + 255 * S);

					result[i] = (result[i] * (1 - B) + 0 * B);
				}
				else
					result[i] = (255 * 0) / 255;
			}

		}

		string filename1 = "output/h" + to_string(l) + ".jpg";
		stbi_write_jpg(filename1.c_str(), 300, 300, channels1, result, 100);
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, result);
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
	normal_arr = new unsigned char[300 * 300 * 3];
	normal_mod = new double[300 * 300 * 3];
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
			
