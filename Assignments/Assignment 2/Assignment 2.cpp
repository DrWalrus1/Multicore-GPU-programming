// Assignment 2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "common.h"


int main()
{
    cl::Platform platform;			// device's platform
    cl::Device device;				// device used
    cl::Context context;			// context for the device
    std::vector<cl::Device> contextDevices;	// devices in the context
    cl::Program program;			// OpenCL program object
    cl::Kernel kernel;				// a single kernel object
    cl::CommandQueue queue;			// commandqueue for a context and device

    cl::Buffer choiceBuffer, numBuffer;
    
    try {
        if (!select_one_device(&platform, &device))
        {
            // if no device selected
            quit_program("Device not selected.");
        }

        // create a context from device
        context = cl::Context(device);

        // build the program
        if (!build_program(&program, &context, "kernels.cl"))
        {
            // if OpenCL program build error
            quit_program("OpenCL program build error.");
        }

        task1A(&program, &context, &device);
        task1B(&program, &context, &device);
        
        task2A("plaintext.txt", "ciphertext.txt", "decrypted.txt");
        task2B(&program, &context, &device, "plaintext.txt", "cipher.txt", "decrypted.txt");
        task2C(&program, &context, &device, "plaintext.txt", "customcipher.txt", "customcipherdecrypted.txt");

        quit_program("Program Finished.");
        std::cin.ignore();
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

