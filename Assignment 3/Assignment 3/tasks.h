#pragma once
#include "common.h"
#include "bitmap.h"

struct imageData {
	unsigned char* inputImage;
	int imgWidth, imgHeight, imageSize;
};

struct Lookup {
	int previousPixels;
};

void Task1(cl::Program* program, cl::Context* context, cl::Device* device, imageData image);

void Task2(cl::Program* program, cl::Context* context, cl::Device* device, imageData image);

void Task3a(cl::Program* program, cl::Context* context, cl::Device* device, imageData image);

void Task3b(cl::Program* program, cl::Context* context, cl::Device* device, imageData image);

void Task3c(cl::Program* program, cl::Context* context, cl::Device* device, imageData image);

void WriteProfileDataToFile(const char* filename, const char* columnName, std::vector<int> dataArray);

void Task4(cl::Program* program, cl::Context* context, cl::Device* device, imageData image);