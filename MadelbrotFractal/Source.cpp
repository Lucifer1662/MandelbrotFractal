#include  <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <vector>
#include <iostream>
#include <ctime>
#include <amp.h> 
using namespace concurrency;
using std::vector;
using glm::vec3;
using std::cout;
using std::endl;

GLFWwindow* window;
size_t width, height;
#define maxIteration 1000

void OpenWindow(size_t _width, size_t _height, const char* windowName) {
	width = _width;
	height = _height;
	glfwInit();
	window = glfwCreateWindow(width, height, windowName, NULL, NULL);
	glfwMakeContextCurrent(window);

	glewInit();
}


#define At(x,y) width * y + x
vec3 palet[maxIteration];

void GetPixels(double atx, double aty, double scale,vector<vec3>& pixels) {
	long start = std::clock();


	
	double minx = (-2.5-atx)/scale;
	double maxx = (1-atx)/scale;
	double miny = (-1 - aty)/scale;
	double maxy = (1-aty)/scale;
	
	

	/*
	size_t i = 0;
	int m = 0;
	for (double py = 0; py < height; py++)	
	{
		for (double px = 0; px < width; px++)
		{			

			double x0 = (((px/width)*(maxx-minx)) + minx);
			double y0 = (((py/height) * (maxy-miny)) + miny);
			
			double x = 0;
			double y = 0;
			size_t iteration = 0;
			double xtemp = 0;
			while (x*x + y*y < 4 && iteration < maxIteration) {
				xtemp = x*x - y*y + x0;
				y = 2 * x*y + y0;
				x = xtemp;
				iteration++;				
			}	
			pixels[i] = palet[iteration];
			i++;
		}		
	}*/


	


	array_view<vec3, 2> pixelArrayView(width, height, pixels);
	array_view<vec3, 1> paletArrayView(maxIteration, palet);

	accelerator device = accelerator(accelerator::default_accelerator);
	accelerator_view view = device.default_view;
	double w = width;
	double h = height;
	double xmax_min = maxx - minx;
	double ymax_min = maxy - miny;

	parallel_for_each(view, pixelArrayView.extent, [=](index<2> idx) restrict(amp)
	{
		
		double x0 = (((idx[1] / w) * xmax_min) + minx);
		double y0 = (((idx[0] / h) * ymax_min) + miny);	

		double x = 0;
		double y = 0;
		double xx = 0;
		double yy = 0;
		size_t iteration = 0;
		double xtemp = 0;
		while ( xx + yy < 4 && iteration < maxIteration) {
			xtemp = xx - yy + x0;
			y = 2 * x*y + y0;
			x = xtemp;
			iteration++;
			xx = x*x;
			yy = y*y;
		}
		pixelArrayView[idx].x =  paletArrayView[iteration].x;
		pixelArrayView[idx].y = paletArrayView[iteration].y;
		pixelArrayView[idx].z = paletArrayView[iteration].z;
	});
	view.wait();

	
	long end = std::clock();
	
	cout << end - start << endl;



/*
	For each pixel(Px, Py) on the screen, do:
	{
		x0 = scaled x coordinate of pixel(scaled to lie in the Mandelbrot X scale(-2.5, 1))
			y0 = scaled y coordinate of pixel(scaled to lie in the Mandelbrot Y scale(-1, 1))
			x = 0.0
			y = 0.0
			iteration = 0
			max_iteration = 1000
			while (x*x + y*y < 2 * 2  AND  iteration < max_iteration) {
				xtemp = x*x - y*y + x0
					y = 2 * x*y + y0
					x = xtemp
					iteration = iteration + 1
			}
		color = palette[iteration]
			plot(Px, Py, color)
	}
	*/

}

void StartWindowLoop() {
	
	for (size_t i = 0; i < maxIteration; i++)
	{
		palet[i] = vec3((size_t)rand(), (size_t)rand(), (size_t)rand());
		palet[i] = glm::normalize(palet[i]);
	}


	double scale = 1;
	double x = 0, y = 0;
	double speed = 0.7f;
	glClearColor(0, 1, 0, 0);

	vector<vec3> pixels;
	pixels.resize(width*height);
	GetPixels(0, 0, scale, pixels);

	glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);

	glfwSwapBuffers(window);

	glClear(GL_COLOR_BUFFER_BIT);



	while (true)
	{	
		glfwPollEvents();
		


		if (glfwGetMouseButton(window, 0)) {
	/*		glfwGetCursorPos(window, &x, &y);

			x /= width;
			x = (x * 3.5) - 2.5;


			y /= height;
			y = (y * 2) - 1;



			glDrawPixels(width, height, GL_RGB, GL_FLOAT, &GetPixels(-x*scale, y*scale, scale)[0]);

			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT);
			*/
		}


		if (glfwGetKey(window, GLFW_KEY_A)) {
			x -= speed/scale;
			GetPixels(-x*scale, y*scale, scale, pixels);
			glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);

			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT);
		}

		if (glfwGetKey(window, GLFW_KEY_D)) {
			x += speed/scale;
			GetPixels(-x*scale, y*scale, scale, pixels);
			glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);

			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT);
		}



		if (glfwGetKey(window, GLFW_KEY_W)) {
			y -= speed / scale;
			GetPixels(-x*scale, y*scale, scale, pixels);
			glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);

			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT);
		}

		if (glfwGetKey(window, GLFW_KEY_S)) {
			y += speed / scale;
			GetPixels(-x*scale, y*scale, scale, pixels);
			glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);

			glfwSwapBuffers(window);
		}



		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			scale *= 1.4f;
			GetPixels(-x*scale, y*scale, scale, pixels);
			glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);

			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT);
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
			scale /= 1.4f;
			GetPixels(-x*scale, y*scale, scale, pixels);
			glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);

			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT);
		}

			
	

	

	}
}



void main() {
	OpenWindow(600, 600, "Mandelbrot Fractal");
	StartWindowLoop();
	
}

