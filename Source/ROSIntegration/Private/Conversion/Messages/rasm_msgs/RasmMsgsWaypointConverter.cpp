#include "Conversion/Messages/rasm_msgs/RasmMsgsWaypointConverter.h"


URasmMsgsWaypointConverter::URasmMsgsWaypointConverter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	_MessageType = "rasm_msgs/Waypoint";
}

bool URasmMsgsWaypointConverter::ConvertIncomingMessage(const ROSBridgePublishMsg* message, TSharedPtr<FROSBaseMsg>& BaseMsg) {
	bool KeyFound = false;

	float X = (float)GetDoubleFromBSON(TEXT("msg.x"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Y = (float)GetDoubleFromBSON(TEXT("msg.y"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Length = (float)GetDoubleFromBSON(TEXT("msg.length"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Width = (float)GetDoubleFromBSON(TEXT("msg.width"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Orientation = (float)GetDoubleFromBSON(TEXT("msg.orientation"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;

	int32 Sec = GetInt32FromBSON(TEXT("msg.stamp.secs"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	int32 NSec = GetInt32FromBSON(TEXT("msg.stamp.nsecs"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;

	BaseMsg = TSharedPtr<FROSBaseMsg>(new ROSMessages::rasm_msgs::Waypoint(X, Y, Length, Width, Orientation, FROSTime(Sec, NSec)));
	return true;
}

bool URasmMsgsWaypointConverter::ConvertOutgoingMessage(TSharedPtr<FROSBaseMsg> BaseMsg, bson_t** message) {
	auto WaypointMessage = StaticCastSharedPtr<ROSMessages::rasm_msgs::Waypoint>(BaseMsg);
	*message = BCON_NEW(
		"x", BCON_DOUBLE(WaypointMessage->x),
		"y", BCON_DOUBLE(WaypointMessage->y),
		"length", BCON_DOUBLE(WaypointMessage->length),
		"width", BCON_DOUBLE(WaypointMessage->width),
		"orientation", BCON_DOUBLE(WaypointMessage->orientation),
		"stamp", "{",
		"secs", BCON_INT32(WaypointMessage->time._Sec),
		"nsecs", BCON_INT32(WaypointMessage->time._NSec),
		"}"
	);
	return true;
}
