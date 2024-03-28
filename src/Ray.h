#pragma once

class Ray {
public:
	// Origin
	// Direction/Normal
	// Cast function () { This will be called in raytracer for certain amount of bounces
	// trace and return color type
	// set this rays origin to intersection point and calculate reflection angle and set that to this rays new orientation
	// Possible dont do it this way^ you could have seperate instances of rays in the raytracer class and spawn new ones when there is a bounce, this will allow more than one ray to spawn after a bounce.
};