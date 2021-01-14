#pragma once

#include "ROSBaseMsg.h"
#include "ROSTime.h"

namespace ROSMessages {
	namespace rasm {
		class RASM_GOAL_MSG : public FROSBaseMsg {
		public:
			RASM_GOAL_MSG() {
				_MessageType = "rasm/RASM_GOAL_MSG";
			}

			RASM_GOAL_MSG(float origin_x_meters, float origin_y_meters, float length_meters, float width_meters, float orientation_radians, float timestamp) :
				origin_x_meters(origin_x_meters), origin_y_meters(origin_y_meters), length_meters(length_meters), width_meters(width_meters), orientation_radians(orientation_radians), timestamp(timestamp) {
				_MessageType = "rasm/RASM_GOAL_MSG";
			}

			float origin_x_meters;
			float origin_y_meters;
			float length_meters;
			float width_meters;
			float orientation_radians;
			float timestamp;
		};
	}
}
