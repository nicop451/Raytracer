#pragma once

#include <iostream>
#include <stdint.h>
#include <vector>
#include <cmath>


class RayTracer {
public:
	uint16_t rows;
	uint16_t columns;
	uint16_t samples;

	// ADD
		// Current Camera
		// Object Data
	
	float* pixelColorArray;

	RayTracer(uint16_t _rows, uint16_t _columns, uint16_t _samples);
	~RayTracer();
	//void tracePixel();
	//void Render();
	
};