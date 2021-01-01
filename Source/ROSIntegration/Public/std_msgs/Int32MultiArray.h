#pragma once

#include "ROSBaseMsg.h"

#include "std_msgs/MultiArrayLayout.h"

namespace ROSMessages {
	namespace std_msgs {
		class Int32MultiArray : public FROSBaseMsg {
		public:
			Int32MultiArray() {
				_MessageType = "std_msgs/Int32MultiArray";
			}

			//# specification of data layout
			MultiArrayLayout layout;

			//# array of data
			TArray<int32> data;
		};
	}
}
