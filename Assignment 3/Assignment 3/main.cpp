
#include "common.h"
#include "bitmap.h"

int main(void)
{

	// declare data and memory objects
	unsigned char* inputImage;
	unsigned char* outputImage;
	int imgWidth, imgHeight, imageSize;


	// read input image
	inputImage = read_BMP_RGB_to_RGBA("peppers.bmp", &imgWidth, &imgHeight);

	// allocate memory for output image
	imageSize = imgWidth * imgHeight * 4;

	for (int i = 0; i < 4; i++) {
		std::cout << (int)inputImage[i] << " ";
	}
	std::cout << std::endl;

	quit_program("Finished.");

	return 0;
}