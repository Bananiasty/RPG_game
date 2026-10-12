#pragma once
#include <string>

enum class BodyPart { NONE, TORSO, HEAD, LEFT_ARM, RIGHT_ARM, LEFT_LEG, RIGHT_LEG };

inline const char* get_body_part_name(BodyPart part)
{
	switch (part)
	{
	case BodyPart::HEAD:      return "Head";
	case BodyPart::TORSO:     return "Torso";
	case BodyPart::LEFT_ARM:  return "Left Arm";
	case BodyPart::RIGHT_ARM: return "Right Arm";
	case BodyPart::LEFT_LEG:  return "Left Leg";
	case BodyPart::RIGHT_LEG: return "Right Leg";
	default:                  return "Body";
	}
}
