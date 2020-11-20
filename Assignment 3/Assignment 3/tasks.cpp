#include "tasks.h"


cl::Kernel kernel;
cl::CommandQueue queue;
cl::ImageFormat imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);

#define OFFSET 0
#define GLOBAL_SIZE 16
#define LOCAL_SIZE 4

#ifdef PROFILE
#define ITERATIONS 1000
#endif // PROFILE


#define OUTPUT_SIZE (GLOBAL_SIZE + 1) * 4

// Flips image left to right, top to bottom and then left to right and top to bottom.
void Task1(cl::Program* program, cl::Context* context, cl::Device* device, imageData image) {


	unsigned char* outputHFlipImage = new unsigned char[image.imageSize];
	unsigned char* outputVFlipImage = new unsigned char[image.imageSize];
	unsigned char* outputBothFlipImage = new unsigned char[image.imageSize];

	cl::Image2D inputImgBuffer, outputHorizontalImgBuffer, outputVerticalImgBuffer, outputBothImgBuffer;

	// create a kernel
	kernel = cl::Kernel(*program, "flip_images");

	// create command queue
	queue = cl::CommandQueue(*context, *device);

	// create image objects
	inputImgBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)image.inputImage);
	outputHorizontalImgBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputHFlipImage);
	outputVerticalImgBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputVFlipImage);
	outputBothImgBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputBothFlipImage);

	// set kernel arguments
	kernel.setArg(0, inputImgBuffer);
	kernel.setArg(1, outputHorizontalImgBuffer);
	kernel.setArg(2, outputVerticalImgBuffer);
	kernel.setArg(3, outputBothImgBuffer);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(image.imgWidth, image.imgHeight);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Flip images kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = image.imgWidth;
	region[1] = image.imgHeight;
	region[2] = 1;

	queue.enqueueReadImage(outputHorizontalImgBuffer, CL_TRUE, origin, region, 0, 0, outputHFlipImage);
	queue.enqueueReadImage(outputVerticalImgBuffer, CL_TRUE, origin, region, 0, 0, outputVFlipImage);
	queue.enqueueReadImage(outputBothImgBuffer, CL_TRUE, origin, region, 0, 0, outputBothFlipImage);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task1a.bmp", outputHFlipImage, image.imgWidth, image.imgHeight);
	write_BMP_RGBA_to_RGB("Task1b.bmp", outputVFlipImage, image.imgWidth, image.imgHeight);
	write_BMP_RGBA_to_RGB("Task1c.bmp", outputBothFlipImage, image.imgWidth, image.imgHeight);

	std::cout << "Done." << std::endl;

	// deallocate memory
	delete[] outputHFlipImage;
	delete[] outputVFlipImage;
	delete[] outputBothFlipImage;
}

void Task2(cl::Program* program, cl::Context* context, cl::Device* device, imageData image) {

	unsigned char* outputImage = new unsigned char[image.imageSize];
	
	// create a kernel
	kernel = cl::Kernel(*program, "black_and_white");

	// create command queue
	queue = cl::CommandQueue(*context, *device);

	cl::Image2D inputBuffer, outputBuffer;

	inputBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)image.inputImage);
	outputBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputImage);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, outputBuffer);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(image.imgWidth, image.imgHeight);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Luminance kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = image.imgWidth;
	region[1] = image.imgHeight;
	region[2] = 1;

	queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task2.bmp", outputImage, image.imgWidth, image.imgHeight);

	std::cout << "Done." << std::endl;

	delete[] outputImage;
}

void Task3a(cl::Program* program, cl::Context* context, cl::Device* device, imageData image) {

	unsigned char* outputImage = new unsigned char[image.imageSize];

	// create a kernel
	kernel = cl::Kernel(*program, "gaussian_blur");

#ifdef PROFILE
	// declare events
	std::vector<int> attemptData;
	cl::Event profileEvent;
	cl_ulong timeStart, timeEnd, timeTotal;
	queue = cl::CommandQueue(*context, *device, CL_QUEUE_PROFILING_ENABLE);
#else
	// create command queue
	queue = cl::CommandQueue(*context, *device);
#endif // PROFILE

	cl::Image2D inputBuffer, outputBuffer;

	inputBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)image.inputImage);
	outputBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputImage);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, outputBuffer);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(image.imgWidth, image.imgHeight);

	std::cout << "Enqueuing Naive Gaussian blur kernel..." << std::endl;
	std::cout << "--------------------" << std::endl;

	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = image.imgWidth;
	region[1] = image.imgHeight;
	region[2] = 1;

#ifdef PROFILE
	timeTotal = 0;
	for (int i = 0; i < ITERATIONS; i++) {
		queue.enqueueNDRangeKernel(kernel, offset, globalSize, cl::NullRange, NULL, &profileEvent);

		queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);
		
		queue.finish();
		timeStart = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		timeEnd = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();

		attemptData.push_back(timeEnd - timeStart);
		timeTotal += timeEnd - timeStart;
	}
	printf("Average time = %lu\n", timeTotal / ITERATIONS);
	WriteProfileDataToFile("Task3a.csv", __FUNCTION__, attemptData);
#else
	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

#endif // PROFILE

	// output results to image file
	write_BMP_RGBA_to_RGB("Task3a.bmp", outputImage, image.imgWidth, image.imgHeight);

	std::cout << "Done." << std::endl;

	delete[] outputImage;

}

void Task3b(cl::Program* program, cl::Context* context, cl::Device* device, imageData image) {

	unsigned char* outputImage = new unsigned char[image.imageSize];

#ifdef PROFILE
	// declare events
	std::vector<int> attemptData;
	cl::Event profileEvent;
	cl_ulong timeStart, timeEnd, timeTotal;
	queue = cl::CommandQueue(*context, *device, CL_QUEUE_PROFILING_ENABLE);
#else
	// create command queue
	queue = cl::CommandQueue(*context, *device);
#endif // PROFILE

	// create a kernel
	kernel = cl::Kernel(*program, "two_pass_gaussian");

	cl::Image2D inputBuffer, outputBuffer;

	inputBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)image.inputImage);
	outputBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputImage);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, outputBuffer);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(image.imgWidth, image.imgHeight);

	std::cout << "Enqueuing Two pass Gaussian blur kernel..." << std::endl;
	std::cout << "--------------------" << std::endl;

	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = image.imgWidth;
	region[1] = image.imgHeight;
	region[2] = 1;


#ifdef PROFILE
	timeTotal = 0;
	for (int i = 0; i < ITERATIONS; i++) {
		queue.enqueueNDRangeKernel(kernel, offset, globalSize, cl::NullRange, NULL, &profileEvent);

		queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

		queue.finish();
		timeStart = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		timeEnd = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();

		attemptData.push_back(timeEnd - timeStart);
		timeTotal += timeEnd - timeStart;
	}
	printf("Average time = %lu\n", timeTotal / ITERATIONS);
	WriteProfileDataToFile("Task3b.csv", __FUNCTION__, attemptData);
#else
	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

#endif // PROFILE

	// output results to image file
	write_BMP_RGBA_to_RGB("Task3b.bmp", outputImage, image.imgWidth, image.imgHeight);

	std::cout << "Done." << std::endl;

	delete[] outputImage;

}

void Task3c(cl::Program* program, cl::Context* context, cl::Device* device, imageData image) {

	unsigned char* outputImage = new unsigned char[image.imageSize];

	// create a kernel
	//kernel = cl::Kernel(*program, "simple_image");
	kernel = cl::Kernel(*program, "test_Task3c");

	// create command queue
	queue = cl::CommandQueue(*context, *device);

	cl::Buffer inputBuffer;
	cl::Image1D outputBuffer;

	inputBuffer = cl::Buffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, image.imageSize, (void*)image.inputImage);

	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = image.imgWidth;
	region[1] = image.imgHeight;
	region[2] = 1;

	outputBuffer = cl::Image1D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, (void*)outputImage);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, outputBuffer);
	
	// enqueue kernel for execution
	cl::NDRange offset(0/*, 0*/);
	cl::NDRange globalSize(image.imageSize/*image.imgWidth, image.imgHeight*/);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	//std::cout << "Naive Gaussian blur kernel enqueued." << std::endl;
	//std::cout << "--------------------" << std::endl;

	//queue.enqueueReadImage(test2, CL_TRUE, origin, region, 0, 0, outputImage);

	//// output results to image file
	//write_BMP_RGBA_to_RGB("Task3c.bmp", outputImage, image.imgWidth, image.imgHeight);

	//std::cout << "Done." << std::endl;

	//delete[] outputImage;
}

void WriteProfileDataToFile(const char* filename, const char* columnName, std::vector<int> dataArray) {
	std::vector<std::string> ColumnNames;
	std::vector<std::vector<int>> ColumnsData;
	int ColumnCount = 0;
	
	/*	Attempt | Task3a | Task3b | Task3c... */
	std::fstream DataFile;
	DataFile.open(filename, std::fstream::in | std::fstream::out);

	if (!DataFile.is_open()) {
		std::cout << "File: " << filename << " not found. Creating new file." << std::endl;
		DataFile.open(filename, std::fstream::out);
	}
	ColumnNames.push_back("Attempt");
	ColumnNames.push_back(columnName);
	DataFile.clear();
	DataFile.seekg(0, std::ios::beg);
	std::cout << "Writing new data to file!" << std::endl;
	//Add new Column names
	for (int i = 0; i < ColumnNames.size() - 1; i++) {
		DataFile << ColumnNames[i] << ",";
	}
	DataFile << ColumnNames[ColumnNames.size() - 1] << "\n";

	// Write rows
	for (int i = 0; i < dataArray.size(); i++) {
		DataFile << i+1 << "," << dataArray[i] << "\n";
	}
}

void Task4(cl::Program* program, cl::Context* context, cl::Device* device, imageData image) {
	unsigned char* outputImage = new unsigned char[image.imageSize];
	double luminance;
	std::cout << "Please enter luminance threshold value (double): ";
	std::cin >> luminance;
	std::cin.ignore();
	std::cout << "Your luminance threshold is: " << luminance << std::endl;

	kernel = cl::Kernel(*program, "luminance_threshold");

	queue = cl::CommandQueue(*context, *device);

	cl::Image2D inputBuffer, outputBuffer;

	inputBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)image.inputImage);
	outputBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputImage);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, outputBuffer);
	kernel.setArg(2, luminance);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(image.imgWidth, image.imgHeight);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Luminance threshold kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = image.imgWidth;
	region[1] = image.imgHeight;
	region[2] = 1;

	queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task4a.bmp", outputImage, image.imgWidth, image.imgHeight);

	std::cout << "Luminance threshold kernel complete." << std::endl;
	std::cout << "--------------------" << std::endl;

	//----------------- Task4b --------------------

	kernel = cl::Kernel(*program, "horizontal_pass");

	image.inputImage = read_BMP_RGB_to_RGBA("Task4a.bmp", &image.imgWidth, &image.imgHeight);

	inputBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)image.inputImage);
	outputBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputImage);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, outputBuffer);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Horizontal pass kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task4b.bmp", outputImage, image.imgWidth, image.imgHeight);

	std::cout << "Horizontal pass kernel compete." << std::endl;
	std::cout << "--------------------" << std::endl;

	//----------------- Task4c --------------------

	kernel = cl::Kernel(*program, "vertical_pass");

	image.inputImage = read_BMP_RGB_to_RGBA("Task4b.bmp", &image.imgWidth, &image.imgHeight);

	inputBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)image.inputImage);
	outputBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputImage);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, outputBuffer);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Vertical pass kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task4c.bmp", outputImage, image.imgWidth, image.imgHeight);

	//----------------- Task4d --------------------
	kernel = cl::Kernel(*program, "combine_images");

	image.inputImage = read_BMP_RGB_to_RGBA("peppers.bmp", &image.imgWidth, &image.imgHeight);
	unsigned char* modifiedImage = read_BMP_RGB_to_RGBA("Task4c.bmp", &image.imgWidth, &image.imgHeight);

	cl::Image2D modifiedImageBuffer;

	inputBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)image.inputImage);
	modifiedImageBuffer = cl::Image2D(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)modifiedImage);
	outputBuffer = cl::Image2D(*context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, image.imgWidth, image.imgHeight, 0, (void*)outputImage);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, modifiedImageBuffer);
	kernel.setArg(2, outputBuffer);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Combine images kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	queue.enqueueReadImage(outputBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task4d.bmp", outputImage, image.imgWidth, image.imgHeight);

	std::cout << "Combine images kernel complete." << std::endl;
	std::cout << "--------------------" << std::endl;
}