#pragma once
#include "common.h"
#include "bitmap.h"

struct imageData {
	unsigned char* inputImage;
	int imgWidth, imgHeight, imageSize;
};

void Task1(cl::Program* program, cl::Context* context, cl::Device* device, imageData image);

void Task2(cl::Program* program, cl::Context* context, cl::Device* device, imageData image);