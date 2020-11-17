__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | 
      CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST; 

// 3x3 Blurring filter (Naïve Approach)
__constant float GaussianFilter[49] = {
	//-3	   -2		 -1			0		 1		   2		 3
	0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036, //-3
	0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363, //-2
	0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446, //-1
	0.002291, 0.023226, 0.092651, 0.146768, 0.092651, 0.023226, 0.002291, // 0
	0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446, // 1
	0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363, // 2
	0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036  // 3
};

__constant float TwoPassGaussianFilter[7] = {
	0.00598, 0.060626, 0.241843, 0.383103, 0.241843, 0.060626, 0.00598
};

__kernel void simple_image(	read_only image2d_t src_image,
							write_only image2d_t dst_image) {

   /* Get pixel coordinate */
   int2 coord = (int2)(get_global_id(0), get_global_id(1));

   /* Read pixel value */
   float4 pixel = read_imagef(src_image, sampler, coord);
   /* Write new pixel value to output */
   write_imagef(dst_image, coord, pixel);
}

__kernel void flip_images(read_only image2d_t src_image,
	write_only image2d_t flip_horizontal,
	write_only image2d_t flip_vertical,
	write_only image2d_t flip_both) {
	
	int imgWidth = get_image_width(src_image);
	int imgHeight = get_image_height(src_image);

	
	/* Get pixel coordinate */
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	int2 flip_horizontal_coord = (int2) (imgWidth - 1 - coord.x, coord.y);
	int2 flip_vertical_coord = (int2) (coord.x, imgHeight - 1 - coord.y);
	int2 flip_both_coord = (int2) (flip_horizontal_coord.x, flip_vertical_coord.y);

	/* Read pixel value */
	float4 pixel = read_imagef(src_image, sampler, coord);
	
	/* Write pixel value to outputs */
	write_imagef(flip_horizontal, flip_horizontal_coord, pixel);
	write_imagef(flip_vertical, flip_vertical_coord, pixel);
	write_imagef(flip_both, flip_both_coord, pixel);
}

__kernel void black_and_white(read_only image2d_t src_image,
	write_only image2d_t dst_image) {

	/* Get pixel coordinate */
	int2 coord = (int2)(get_global_id(0), get_global_id(1));

	/* Read pixel value */
	float4 pixel = read_imagef(src_image, sampler, coord);
	float luminance = (0.299 * pixel.x) + (0.587 * pixel.y) + (0.114 * pixel.z);
	pixel.xyz = luminance;
	/* Write new pixel value to output */
	write_imagef(dst_image, coord, pixel);
}

__kernel void gaussian_blur(read_only image2d_t src_image,
	write_only image2d_t dst_image) {

	/* Get work-item’s row and column position */
	int column = get_global_id(0);
	int row = get_global_id(1);

	/* Accumulated pixel value */
	float4 sum = (float4)(0.0);

	/* Filter's current index */
	int filter_index = 0;

	int2 coord;
	float4 pixel;

	/* Iterate over the rows */
	for (int i = -3; i <= 3; i++) {
		coord.y = row + i;

		/* Iterate over the columns */
		for (int j = -3; j <= 3; j++) {
			coord.x = column + j;

			/* Read value pixel from the image */
			pixel = read_imagef(src_image, sampler, coord);
			/* Acculumate weighted sum */
			sum.xyz += pixel.xyz * GaussianFilter[filter_index++];
		}
	}

	/* Write new pixel value to output */
	coord = (int2)(column, row);
	write_imagef(dst_image, coord, sum);
}

__kernel void two_pass_gaussian(read_only image2d_t src_image,
	write_only image2d_t dst_image) {

	/* Get work-item’s row and column position */
	int column = get_global_id(0);
	int row = get_global_id(1);

	/* Accumulated pixel value */
	float4 sum = (float4)(0.0);

	/* Filter's current index */
	int filter_index = 0;

	int2 coord;
	float4 pixel;

	/* Horizontal pass */
	coord.x = column;
	for (int i = -3; i <= 3; i++) {
		coord.y = row + i;

		/* Read value pixel from the image */
		pixel = read_imagef(src_image, sampler, coord);

		/* Acculumate weighted sum */
		sum.xyz += pixel.xyz * TwoPassGaussianFilter[filter_index++];
	}

	/* Vertical pass */
	coord.y = row;
	for (int i = -3; i <= 3; i++) {
		coord.x = column + i;

		/* Read value pixel from the image */
		pixel = read_imagef(src_image, sampler, coord);

		/* Acculumate weighted sum */
		sum.xyz += pixel.xyz * TwoPassGaussianFilter[filter_index++];
	}

	/* Write new pixel value to output */
	coord = (int2)(column, row);
	write_imagef(dst_image, coord, sum);
}