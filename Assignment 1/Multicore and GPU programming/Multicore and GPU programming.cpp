// Multicore and GPU programming.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

#include <iostream>
#include <vector>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// functions to handle errors
#include "error.h"

int main(void)
{
    std::string deviceSelection;
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;

    std::vector<cl::Device> cpu_devices;
    std::vector<cl::Device> gpu_devices;

    unsigned int i, j;

    try {
        cl::Platform::get(&platforms);
        std::cout << "Number of OpenCL platforms: " << platforms.size() << std::endl;
        
        for (i = 0; i < platforms.size(); i++) {

            platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);

            for (j = 0; j < devices.size(); j++) {
                
                
                // get and add devices to array of matching type
                cl_device_type type;
                devices[j].getInfo(CL_DEVICE_TYPE, &type);
                if (type == CL_DEVICE_TYPE_CPU) {
                    cpu_devices.push_back(devices[j]);
                    
                } else if (type == CL_DEVICE_TYPE_GPU)
                    gpu_devices.push_back(devices[j]);
            }
        }

        for (i = 0; i < cpu_devices.size(); i++) {
            std::cout << cpu_devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
        }

        for (i = 0; i < gpu_devices.size(); i++) {
            std::cout << gpu_devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
        }
    }
    // catch any OpenCL function errors
    catch (cl::Error e) {
       
        // call function to handle errors
        handle_error(e);
    }

    /*std::cout << "Please select the type of device to use (CPU/GPU): ";
    std::cin >> deviceSelection;

    std::cout << std::endl << "You Chose: " << deviceSelection << std::endl;*/

#ifdef _WIN32
    // wait for a keypress on Windows OS before exiting
    std::cout << "\npress a key to quit...";
    std::cin.ignore();
#endif

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
