#include "pch.h"
#include "Sprite.h"

#include <cmath>
#include <float.h>

#include "PI.h"

namespace rc {
		
Sprite::Sprite(const float x_position, const float z_position, const uint8_t size) :
	x(x_position),
	z(z_position),
	size(size)
{
}

/** To understand the formula, try to picture it like this. 
   Z ^
	 |        / ray
	 |   I  \/
	 |      /\ C     -
	 |     /  \      | H
	 |    /    \     | 
	 |   O    sprite -
	 |   |--L---|  
   --|-------------------------------------> X

   The ray start in O at angle alpha with the X axis.
   The sprite position is C.

   We want the intersection I and the distances between I and O and I and C.
   The first we will use to scale the sprite (the farthest, the smaller), the second
   is to find the texture slice to draw.

   This is an intentionally stupid algorithm. The best way probably is to intersect the
   segments between the ray origin and any wall hit and between the two endpoints of
   the sprite. But, on a dare, I am trying to not use vector math.
*/
RayHit Sprite::intersection(const Ray& ray) const
{
	const float vertical_difference = z - ray.z;  // H in the drawing.
	float horizontal_difference = x - ray.x;  // L in the drawing.

	// Fudge it to avoid divide by 0 and to address the special case.
	// It should be still "close enough". No, I don't think this is the correct way.
	if (horizontal_difference == 0)
		horizontal_difference = FLT_MIN;

	// Think of the O-I-C triangle. The angle at O is gamma.
	// Beta is the angle in O of the O-C-(below C, side with O to close the triangle).
	const float beta = std::atan(vertical_difference / horizontal_difference);
	const float gamma = ray.alpha_rad - beta;

	const float distance = std::sqrt(horizontal_difference * horizontal_difference + vertical_difference * vertical_difference);

	const float origin_distance = distance * std::cos(gamma);
	float center_distance = distance * std::sin(gamma);

	const float half_span = size / 2;
	RayHit result;

	if ((origin_distance * horizontal_difference) < 0 ||  // Behind the ray. Discovered by trial and error.
		(std::abs(center_distance) > half_span)) // Ouside the segment "covered" by the texture. Half-size each side of the center.
		return result;
	
	// Did actually hit, fit all the data.
	result.distance = std::abs(origin_distance);
	
	// For reasons that I won't even pretend to understand, the texture would be inverted
	// under those conditions. Probably something to do to where the atan changes sign or when
	// it rolls around from -PI/2 to PI/2.
	// This mirrors the texture again. TODO: attempt to figure out the math here.
	if (horizontal_difference < 0)
		center_distance = -center_distance;

	// Texture offsets are non-negative, but the computation uses the sprite center,
	// so re-center the offset to have 0 offset on the side of the texture.
	result.offset = center_distance + half_span;

	// We will never use the exact coordinate (TODO: until you can shoot the sprites, probably).
	
	return result;
}

}