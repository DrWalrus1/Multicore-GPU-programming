#include "common.h"

// allows the user to select a device, displays the available platform and device options
// returns whether selection was successful, the selected device and its platform
bool select_one_device(cl::Platform* platfm, cl::Device* dev)
{
	std::vector<cl::Platform> platforms;	// available platforms
	std::vector< std::vector<cl::Device> > platformDevices;	// devices available for each platform
	std::string outputString;				// string for output
	unsigned int i, j;						// counters

	try {
		// get the number of available OpenCL platforms
		cl::Platform::get(&platforms);
		std::cout << "Number of OpenCL platforms: " << platforms.size() << std::endl;

		// find and store the devices available to each platform
		for (i = 0; i < platforms.size(); i++)
		{
			std::vector<cl::Device> devices;		// available devices

			// get all devices available to the platform
			platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);

			// store available devices for the platform
			platformDevices.push_back(devices);
		}

		// display available platforms and devices
		std::cout << "--------------------" << std::endl;
		std::cout << "Available options:" << std::endl;

		// store options as platform and device indices
		std::vector< std::pair<int, int> > options;
		unsigned int optionCounter = 0;	// option counter

		// for all platforms
		for (i = 0; i < platforms.size(); i++)
		{
			// for all devices per platform
			for (j = 0; j < platformDevices[i].size(); j++)
			{
				// display options
				std::cout << "Option " << optionCounter << ": Platform - ";

				// platform vendor name
				outputString = platforms[i].getInfo<CL_PLATFORM_VENDOR>();
				std::cout << outputString << ", Device - ";

				// device name
				outputString = platformDevices[i][j].getInfo<CL_DEVICE_NAME>();
				std::cout << outputString << std::endl;

				// store option
				options.push_back(std::make_pair(i, j));
				optionCounter++; // increment option counter
			}
		}

		std::cout << "\n--------------------" << std::endl;
		std::cout << "Select a device: ";

		std::string inputString;
		unsigned int selectedOption;	// option that was selected

		std::getline(std::cin, inputString);
		std::istringstream stringStream(inputString);

		// check whether valid option selected
		// check if input was an integer
		if (stringStream >> selectedOption)
		{
			char c;

			// check if there was anything after the integer
			if (!(stringStream >> c))
			{
				// check if valid option range
				if (selectedOption >= 0 && selectedOption < optionCounter)
				{
					// return the platform and device
					int platformNumber = options[selectedOption].first;
					int deviceNumber = options[selectedOption].second;

					*platfm = platforms[platformNumber];
					*dev = platformDevices[platformNumber][deviceNumber];

					return true;
				}
			}
		}
		// if invalid option selected
		std::cout << "\n--------------------" << std::endl;
		std::cout << "Invalid option." << std::endl;
	}
	// catch any OpenCL function errors
	catch (cl::Error e) {
		// call function to handle errors
		handle_error(e);
	}

	return false;
}

// builds program from given filename
bool build_program(cl::Program* prog, const cl::Context* ctx, const std::string filename)
{
	// get devices from the context
	std::vector<cl::Device> contextDevices = ctx->getInfo<CL_CONTEXT_DEVICES>();

	// open input file stream to .cl file
	std::ifstream programFile(filename);

	// check whether file was opened
	if (!programFile.is_open())
	{
		std::cout << "File not found." << std::endl;
		return false;
	}

	// create program string and load contents from the file
	std::string programString(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));

	// create program source from one input string
	cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));
	// create program from source
	*prog = cl::Program(*ctx, source);

	// try to build program
	try {
		// build the program for the devices in the context
		prog->build(contextDevices);

		std::cout << "Program build: Successful" << std::endl;
		std::cout << "--------------------" << std::endl;
	}
	catch (cl::Error e) {
		// if failed to build program
		if (e.err() == CL_BUILD_PROGRAM_FAILURE)
		{
			// output program build log
			std::cout << e.what() << ": Failed to build program." << std::endl;

			// check build status for all all devices in context
			for (unsigned int i = 0; i < contextDevices.size(); i++)
			{
				// get device's program build status and check for error
				// if build error, output build log
				if (prog->getBuildInfo<CL_PROGRAM_BUILD_STATUS>(contextDevices[i]) == CL_BUILD_ERROR)
				{
					// get device name and build log
					std::string outputString = contextDevices[i].getInfo<CL_DEVICE_NAME>();
					std::string build_log = prog->getBuildInfo<CL_PROGRAM_BUILD_LOG>(contextDevices[i]);

					std::cout << "Device - " << outputString << ", build log:" << std::endl;
					std::cout << build_log << "--------------------" << std::endl;
				}
			}

			return false;
		}
		else
		{
			// call function to handle errors
			handle_error(e);
		}
	}

	return true;
}

// function to handle error
void handle_error(cl::Error e)
{
	// output OpenCL function that cause the error and the error code
	std::cout << "Error in: " << e.what() << std::endl;
	std::cout << "Error code: " << e.err() << " (" << lookup_error_code(e.err()) << ")" << std::endl;
}

// function to quit program
void quit_program(const std::string str)
{
	std::cout << str << std::endl;
	std::cout << "Exiting the program..." << std::endl;

#ifdef _WIN32
	// wait for a keypress on Windows OS before exiting
	std::cout << "\npress a key to quit...";
	std::cin.ignore();
#endif

	exit(1);
}

bool SelectNumber(int* UserChoice) {
	std::cout << "Please input a number: ";
	std::cin >> *UserChoice;
	return true;
}

bool SelectNumber(int* UserChoice, int min, int max) {
	std::cout << "Please input a number between 1 and 100: ";
	std::cin >> *UserChoice;
	std::cin.ignore();
	if (*UserChoice >= min && *UserChoice <= max) {
		return true;
	}
	else {
		return false;
	}
}

void task1A(cl::Program* program, cl::Context* context, cl::Device* device) {
	int UserChoice;
	std::vector<cl_int> numbers(512);
	numbers[0] = 3;

	// take user number input
	if (!SelectNumber(&UserChoice, 1, 100)) {
		quit_program("Invalid Number");
	}

	cl::Kernel kernel = cl::Kernel(*program, "add");

	// create command queue
	cl::CommandQueue queue = cl::CommandQueue(*context, *device);

	// Buffers
	cl::Buffer choiceBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &UserChoice);
	cl::Buffer numBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numbers.size(), &numbers[0]);

	// set kernel arguments
	kernel.setArg(0, choiceBuffer);
	kernel.setArg(1, numBuffer);

	// enqueue kernel for execution
	cl::NDRange offset(OFFSET);
	cl::NDRange globalSize(GLOBAL_SIZE);
	cl::NDRange localSize(LOCAL_SIZE);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize, localSize);

	std::cout << "Kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	// enqueue command to read from device to host memory
	queue.enqueueReadBuffer(numBuffer, CL_TRUE, 0, sizeof(cl_int) * numbers.size(), &numbers[0]);

	for (int i = 0; i < GLOBAL_SIZE; i++)
	{
		std::cout << "Item " << i+1 << ": " << numbers[i] << std::endl;
	}
	//FIXME: DOESN'T SEEM TO WORK ON GPU
	//FIXME: At a random point during runtime, the numbers will multiply again (only happens on 8700k,)
}

void task1B(cl::Program* program, cl::Context* context, cl::Device* device) {
	std::vector<int> vec1(32);
	std::vector<int> vec2(16);
	cl::Kernel kernel;

	//Generate random numbers for vec1
	srand((unsigned)time(NULL));
	for (int i = 0; i < vec1.size(); i++) {
		int num = rand() % 20 + 10;
		if (num > 20 || num < 10) {
			i--;
			continue;
		}
		vec1[i] = num;
	}

	//Fill vec2
	for (int i = 0, x = 2, y = -9; i < vec2.size() / 2; i++, x++, y++) {
		vec2[i] = x;
		vec2[i + vec2.size() / 2] = y;
	}

	for (int i = 0; i < vec2.size(); i++) {
		std::cout << i << ": " << vec2[i] << std::endl;
	}

	kernel = cl::Kernel(*program, "task1b");

	// create command queue
	cl::CommandQueue queue = cl::CommandQueue(*context, *device);

	// Buffers
	cl::Buffer vec1Buffer;
	cl::Buffer vec2Buffer;
	

}

void task2A(std::string filename, std::string cypherFilename, std::string decryptFilename) {
	std::string fileContents, cypherContents, decryptContents;
	int shift;

	if (!SelectNumber(&shift)) {
		quit_program("Invalid number picked.");
	}
	

	fileContents = ReadFile(filename);
	WriteToFile(cypherFilename, CeaserShift(fileContents, shift));
	
	cypherContents = ReadFile(cypherFilename);
	WriteToFile(decryptFilename, CeaserShift(cypherContents, shift * -1));

	decryptContents = ReadFile(decryptFilename);
	if (CompareFileContents(fileContents, decryptContents) == true) {
		std::cout << "It matches!" << std::endl;
	}
	else {
		std::cout << "No match!" << std::endl;
	}
}

std::string ReadFile(std::string filename) {
	// open input file stream to .cl file
	std::ifstream file(filename);
	std::string str, fileContents;
	// check whether file was opened
	if (!file.is_open())
	{
		quit_program("File: " + filename + " not found.");
	}

	while (std::getline(file, str)) {
		fileContents += str;
		fileContents += '\n';
	}

	std::transform(fileContents.begin(), fileContents.end(), fileContents.begin(), ::toupper);
	return fileContents;
}

std::string CeaserShift(std::string text, int shift) {
	int Apos = 65, Zpos = 90;
	for (int i = 0; i < text.length(); i++) {
		int remainingShift = shift;
		char current = text[i];
		if (((int)current + shift) > Zpos || ((int)current + shift) < Apos) {
			continue;
		}
		while (remainingShift != 0) {
			if (((int)current + remainingShift) <= Zpos && ((int)current + remainingShift) >= Apos) {
				text[i] = (char)((int)current + remainingShift);
				remainingShift = 0;

			} else if (((int)current + shift) > Zpos) {
				int difference = Zpos - (int)current;
				current = Apos - 1;
				remainingShift = remainingShift - difference;
			//Subtracting
			} else if (((int)current + shift) < Apos) {
				int difference = (int)current - Apos;
				current = Zpos +1;
				remainingShift = remainingShift + difference;
			}
			else {
				text[i] = (char)((int)current + shift);
				remainingShift = 0;
			}
		}

	}

	return text;
}

void WriteToFile(std::string fileName, std::string contents) {
	std::ofstream ofs;
	ofs.open(fileName);
	ofs << contents;
	ofs.close();
}

bool CompareFileContents(std::string fileContents, std::string decryptContents) {
	if (fileContents.compare(decryptContents)) {
		return true;
	}
	else {
		return false;
	}
}

void task2B(cl::Program* program, cl::Context* context, cl::Device* device, std::string filename, std::string cypherFilename, std::string decryptFilename) {
	std::string fileContents, cypherContents, decryptContents;
	int shift;
	int filenameSize = sizeof(filename);
	if (!SelectNumber(&shift)) {
		quit_program("Invalid number picked.");
	}

	fileContents = ReadFile(filename);
	std::vector<char> charArray(fileContents.length());
	std::copy(fileContents.begin(), fileContents.end(), charArray.begin());

	std::string encryptedString = MultiCoreEncrypt(program, context, device, charArray, shift);

	WriteToFile(cypherFilename, encryptedString);

	cypherContents = ReadFile(cypherFilename);
	std::vector<char> cypherArray(cypherContents.length());
	std::copy(cypherContents.begin(), cypherContents.end(), cypherArray.begin());

	std::string decryptedString = MultiCoreEncrypt(program, context, device, charArray, shift * -1);

	if (CompareFileContents(fileContents, decryptedString) == true) {
		std::cout << "It matches!" << std::endl;
	}
	else {
		std::cout << "No match!" << std::endl;
	}

	quit_program("File Encrypted.");
	
}

std::string MultiCoreEncrypt(cl::Program* program, cl::Context* context, cl::Device* device, std::vector<char> charArray, int shift) {
	std::vector<char> charOutput(sizeof(cl_char) * charArray.size());

	cl::Kernel kernel = cl::Kernel(*program, "CeaserShift");

	// create command queue
	cl::CommandQueue queue = cl::CommandQueue(*context, *device);

	// Buffers
	cl::Buffer inputBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * charArray.size(), &charArray[0]);
	cl::Buffer outputBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * charOutput.size(), &charOutput[0]);
	cl::Buffer shiftBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &shift);

	// set kernel arguments
	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, outputBuffer);
	kernel.setArg(2, shiftBuffer);

	// enqueue kernel for execution
	cl::NDRange offset(OFFSET);
	cl::NDRange globalSize(sizeof(cl_char) * charArray.size());
	cl::NDRange localSize(LOCAL_SIZE);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize, localSize);

	std::cout << "Kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	// enqueue command to read from device to host memory
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_char) * charOutput.size(), &charOutput[0]);

	return (std::string(charOutput.begin(), charOutput.end()));
}

void task2C(cl::Program* program, cl::Context* context, cl::Device* device, std::string filename, std::string cypherFilename, std::string decryptFilename) {
	std::string fileContents, cypherContents, decryptContents;
	cl_char fromCharArray[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
	std::vector<cl_char> fromChar(fromCharArray, fromCharArray + sizeof(fromCharArray) / sizeof(cl_char));
	cl_char toCharArray[] = {'G','X','S','Q','F','A','R','O','W','B','L','M','T','H','C','V','P','N','Z','U','I','E','Y','D','K','J'};
	std::vector<cl_char> toChar(toCharArray, toCharArray + sizeof(toCharArray) / sizeof(cl_char));

	fileContents = ReadFile(filename);
	std::vector<char> charArray(fileContents.length());
	std::copy(fileContents.begin(), fileContents.end(), charArray.begin());

	std::vector<char> charOutput(sizeof(cl_char) * charArray.size());

	cl::Kernel kernel = cl::Kernel(*program, "CustomEncrypt");

	// create command queue
	cl::CommandQueue queue = cl::CommandQueue(*context, *device);

	// Buffers
	cl::Buffer fromCharBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * fromChar.size(), &fromChar[0]);
	cl::Buffer toCharBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * toChar.size(), &toChar[0]);
	cl::Buffer charArrayBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * charArray.size(), &charArray[0]);
	cl::Buffer outputBuffer = cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * charOutput.size(), &charOutput[0]);

	// set kernel arguments
	kernel.setArg(0, fromCharBuffer);
	kernel.setArg(1, toCharBuffer);
	kernel.setArg(2, charArrayBuffer);
	kernel.setArg(3, outputBuffer);

	// enqueue kernel for execution
	cl::NDRange offset(0);
	cl::NDRange globalSize(sizeof(cl_char) * charArray.size());
	cl::NDRange localSize(4);

	queue.enqueueNDRangeKernel(kernel, offset, globalSize, localSize);

	std::cout << "Kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	// enqueue command to read from device to host memory
	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_char) * charOutput.size(), &charOutput[0]);

	std::string encrypted(charOutput.begin(), charOutput.end());

	WriteToFile(cypherFilename, encrypted);

	cypherContents = ReadFile(cypherFilename);
	std::vector<char> cypherArray(cypherContents.length());
	std::copy(cypherContents.begin(), cypherContents.end(), cypherArray.begin());

}

// function to lookup and return error code string
const std::string lookup_error_code(cl_int error_code)
{
	// look up error codes as defined in cl.hpp
	switch (error_code) {
	case CL_SUCCESS:
		return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND:
		return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
		return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
		return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
		return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
		return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
		return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
		return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
		return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
		return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_COMPILE_PROGRAM_FAILURE:
		return "CL_COMPILE_PROGRAM_FAILURE";
	case CL_LINKER_NOT_AVAILABLE:
		return "CL_LINKER_NOT_AVAILABLE";
	case CL_LINK_PROGRAM_FAILURE:
		return "CL_LINK_PROGRAM_FAILURE";
	case CL_DEVICE_PARTITION_FAILED:
		return "CL_DEVICE_PARTITION_FAILED";
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
		return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

	case CL_INVALID_VALUE:
		return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
		return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
		return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
		return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
		return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:
		return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
		return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
		return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
		return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
		return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
		return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
		return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
		return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
		return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
		return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
		return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
		return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
		return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
		return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:
		return "CL_INVALID_GLOBAL_WORK_SIZE";
	case CL_INVALID_PROPERTY:
		return "CL_INVALID_PROPERTY";
	case CL_INVALID_IMAGE_DESCRIPTOR:
		return "CL_INVALID_IMAGE_DESCRIPTOR";
	case CL_INVALID_COMPILER_OPTIONS:
		return "CL_INVALID_COMPILER_OPTIONS";
	case CL_INVALID_LINKER_OPTIONS:
		return "CL_INVALID_LINKER_OPTIONS";
	case CL_INVALID_DEVICE_PARTITION_COUNT:
		return "CL_INVALID_DEVICE_PARTITION_COUNT";

		// not defined in MacOS's cl.h
#ifndef __APPLE__
	case CL_INVALID_PIPE_SIZE:
		return "CL_INVALID_PIPE_SIZE";
	case CL_INVALID_DEVICE_QUEUE:
		return "CL_INVALID_DEVICE_QUEUE";
#endif

	default:
		return "Unknown error code";
	}
}