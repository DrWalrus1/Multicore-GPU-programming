// Multicore and GPU programming.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// functions to handle errors
#include "error.h"

bool select_one_device(cl::Platform* platfm, cl::Device* dev);

int main(void)
{
	std::string deviceSelection;
	cl::Platform platform;
	cl::Device device;
	cl::Context context;					// context for the device
	std::vector<cl::Device> contextDevices;	// devices in the context
	std::string outputString;				// string for output
	unsigned int i;							// counters


	try {
		// select an OpenCL device
		if (!select_one_device(&platform, &device)) {
			// if no device selected
			quit_program("Device not selected.");
		}
		std::cout << std::endl;
		std::cout << "--------------------" << std::endl;

		//Platform Name
		platform.getInfo(CL_PLATFORM_NAME, &outputString);
		std::cout << "Platform Selected: " << outputString << std::endl;

		//Device Type
		cl_device_type type;
		device.getInfo(CL_DEVICE_TYPE, &type);
		if (type == CL_DEVICE_TYPE_CPU)
			std::cout << "\tType: " << "CPU" << std::endl;
		else if (type == CL_DEVICE_TYPE_GPU)
			std::cout << "\tType: " << "GPU" << std::endl;

		//Device Name
		outputString = device.getInfo<CL_DEVICE_NAME>();
		std::cout << "\tName: " << outputString << std::endl;

		//Compute Units
		std::cout << "\tCompute units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;

		//Work Group Size
		std::cout << "\tWork group size: " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;

		//Work Group Size
		std::cout << "\tWork item sizes: " << std::endl;
		
		for (int i = 0; i < device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>().size(); i++) {
			std::cout << "\t\tItem " << i << ": " << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[i] << std::endl;
		}

		//Work Group Size
		std::cout << "\tGlobal memory size: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;

		//Device Extensions
		std::cout << "\tDevice supports cl_khr_icd extension: ";
		if (device.getInfo<CL_DEVICE_EXTENSIONS>().find("cl_khr_icd")) {
			#pragma OPENCL EXTENSION cl_khr_icd : enable
			std::cout << "true" << std::endl;
		} else {
			std::cout << "false" << std::endl;
		}
		context = cl::Context(device);
		// get devices from the context
		contextDevices = context.getInfo<CL_CONTEXT_DEVICES>();

		// open input file stream to .cl file
		std::ifstream programFile("source.cl");

		// check whether file was opened
		if (!programFile.is_open())
		{
			quit_program("File not found.");
		}

		std::string programString(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));

		// create program source from one input string
		cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));
		// create program from source
		cl::Program program(context, source);

		// build the program for the devices in the context
		program.build(contextDevices);

		std::cout << "--------------------" << std::endl;
		std::cout << "Build Status: ";
		if (program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device) == CL_BUILD_ERROR) {
			std::cout << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device) << std::endl;

			return 1;
		}
		else if (program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device) == CL_BUILD_SUCCESS){
			std::cout << "Success" << std::endl;
		}

		// create individual kernels
		cl::Kernel copyKernel(program, "copy");
		cl::Kernel addKernel(program, "add");
		cl::Kernel subKernel(program, "sub");
		cl::Kernel multKernel(program, "mult");
		cl::Kernel divKernel(program, "div");

		std::vector<cl::Kernel> allKernels;		// all kernels

		// create all kernels in the program
		program.createKernels(&allKernels);

		std::cout << "--------------------" << std::endl;
		std::cout << "Kernel names" << std::endl;
		std::cout << "Total kernal count: " << allKernels.size() << std::endl;

		// output kernel name for each index
		for (i = 0; i < allKernels.size(); i++) {
			outputString = allKernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>();
			std::cout << "\tKernel " << i << ": " << outputString << std::endl;
		}
		std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
		std::cout << "--------------------" << std::endl;
		std::cout << "Build Log: " << std::endl;
		std::cout << log << std::endl;
		std::cout << "--------------------" << std::endl;
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

bool select_one_device(cl::Platform* platfm, cl::Device* dev)
{
	std::vector<cl::Platform> allplatforms; // all available platforms
	std::vector<cl::Platform> platforms;	// available platforms that match user choice
	std::vector< std::vector<cl::Device> > platformDevices;	// devices available for each platform
	std::string outputString;				// string for output
	unsigned int i, j;						// counters

	try {
		std::string type;
		//Select type of device
		std::cout << "Please select type of device (CPU/GPU): ";
		std::cin >> type;
		std::cin.ignore();
		if (!(type == "CPU" || type == "GPU")) {
			std::cout << "Incorrect device type selected. Ending..." << std::endl;
			return false;
		}
		
		// get the number of available OpenCL platforms
		cl::Platform::get(&allplatforms);
		std::cout << "Number of OpenCL platforms: " << allplatforms.size() << std::endl;

		// find and store the devices available to each platform
		for (i = 0; i < allplatforms.size(); i++)
		{
			std::vector<cl::Device> devices;		// available devices

			// get all devices available to the platform
			try {
				if (type == "CPU") {
					allplatforms[i].getDevices(CL_DEVICE_TYPE_CPU, &devices);
					platforms.push_back(allplatforms[i]);
				}
				else if (type == "GPU") {
					allplatforms[i].getDevices(CL_DEVICE_TYPE_GPU, &devices);
					platforms.push_back(allplatforms[i]);
				}
			}
			catch (cl::Error e) {
					continue;
				}
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