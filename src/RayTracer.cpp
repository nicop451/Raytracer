#include "RayTracer.h"

RayTracer::RayTracer(uint16_t _rows, uint16_t _columns, uint16_t _samples) : rows(_rows), columns(_columns), samples(_samples) {
	pixelColorArray = new float[rows * columns * 3];
}

RayTracer::~RayTracer() {
	delete pixelColorArray;
}

