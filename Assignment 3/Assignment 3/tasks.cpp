#include "bitmap.h"
#include "tasks.h"


// Flips image left to right, top to bottom and then left to right and top to bottom.
void Task1(cl::Program* program, cl::Context* context, cl::Device* device) {

	cl::Kernel kernel;
	cl::CommandQueue queue;

	unsigned char* inputImage;
	int imgWidth, imgHeight, imageSize;

	unsigned char* outputHFlipImage;

	// read input image
	inputImage = read_BMP_RGB_to_RGBA("lena.bmp", &imgWidth, &imgHeight);

	// allocate memory for output image
	imageSize = imgWidth * imgHeight * 4;

	outputHFlipImage = new unsigned char[imageSize];
	unsigned char* outputVFlipImage = new unsigned char[imageSize];
	unsigned char* outputBothFlipImage = new unsigned char[imageSize];

	cl::ImageFormat imgFormat;
	cl::Image2D inputImgBuffer, outputHorizontalImgBuffer, outputVerticalImgBuffer, outputBothImgBuffer;

	imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);

	// create a kernel
	kernel = cl::Kernel(*program, "flip_images");

	// create command queue
	queue = cl::CommandQueue(*context, *device);

	// create image objects
	inputImgBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)inputImage);
	outputHorizontalImgBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputHFlipImage);
	outputVerticalImgBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputVFlipImage);
	outputBothImgBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputBothFlipImage);

	// set kernel arguments
	kernel.setArg(0, inputImgBuffer);
	kernel.setArg(1, outputHorizontalImgBuffer);
	kernel.setArg(2, outputVerticalImgBuffer);
	kernel.setArg(3, outputBothImgBuffer);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(imgWidth, imgHeight);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = imgWidth;
	region[1] = imgHeight;
	region[2] = 1;

	queue.enqueueReadImage(outputHorizontalImgBuffer, CL_TRUE, origin, region, 0, 0, outputHFlipImage);
	queue.enqueueReadImage(outputVerticalImgBuffer, CL_TRUE, origin, region, 0, 0, outputVFlipImage);
	queue.enqueueReadImage(outputBothImgBuffer, CL_TRUE, origin, region, 0, 0, outputBothFlipImage);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task1a.bmp", outputHFlipImage, imgWidth, imgHeight);
	write_BMP_RGBA_to_RGB("Task1b.bmp", outputVFlipImage, imgWidth, imgHeight);
	write_BMP_RGBA_to_RGB("Task1c.bmp", outputBothFlipImage, imgWidth, imgHeight);

	std::cout << "Done." << std::endl;

	// deallocate memory
	free(inputImage);
	free(outputHFlipImage);
	free(outputVFlipImage);
	free(outputBothFlipImage);
}