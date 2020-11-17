#include "common.h"
#include "bitmap.h"
#include "tasks.h"

int main(void)
{

	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object


	// declare data and memory objects
	struct imageData image;

	try {
		// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}

		// create a context from device
		context = cl::Context(device);

		// build the program
		if (!build_program(&program, &context, "simple_image.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		// read input image
		image.inputImage = read_BMP_RGB_to_RGBA("peppers.bmp", &image.imgWidth, &image.imgHeight);
		image.imageSize = image.imgWidth * image.imgHeight * 4;
		
		//Task1(&program, &context, &device, image);
		
		//Task2(&program, &context, &device, image);

		Task3a(&program, &context, &device, image);

		free(image.inputImage);
	}
	catch (cl::Error e) {
		// call function to handle errors
		handle_error(e);
	}
#ifdef _WIN32
	// wait for a keypress on Windows OS before exiting
	std::cout << "\npress a key to quit...";
	std::cin.ignore();
#endif

	return 0;
}