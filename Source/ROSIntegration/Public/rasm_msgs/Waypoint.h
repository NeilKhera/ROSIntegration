#pragma once

#include "ROSBaseMsg.h"
#include "ROSTime.h"

namespace ROSMessages {
	namespace rasm_msgs {
		class Waypoint : public FROSBaseMsg {
		public:
			Waypoint() {
				_MessageType = "rasm_msgs/Waypoint";
			}

			Waypoint(float x, float y, float length, float width, float orientation, FROSTime time) :
				x(x), y(y), length(length), width(width), orientation(orientation), time(time) {
				_MessageType = "rasm_msgs/Waypoint";
			}

			float x;
			float y;
			float length;
			float width;
			float orientation;
			FROSTime time;
		};
	}
}
