#include "Conversion/Messages/rasm/RasmGoalMsgConverter.h"


URasmGoalMsgConverter::URasmGoalMsgConverter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	_MessageType = "rasm/RASM_GOAL_MSG";
}

bool URasmGoalMsgConverter::ConvertIncomingMessage(const ROSBridgePublishMsg* message, TSharedPtr<FROSBaseMsg>& BaseMsg) {
	bool KeyFound = false;

	float X = (float)GetDoubleFromBSON(TEXT("msg.origin_x_meters"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Y = (float)GetDoubleFromBSON(TEXT("msg.origin_y_meters"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Length = (float)GetDoubleFromBSON(TEXT("msg.length_meters"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Width = (float)GetDoubleFromBSON(TEXT("msg.width_meters"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Orientation = (float)GetDoubleFromBSON(TEXT("msg.orientation_radians"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;
	float Time = (float)GetDoubleFromBSON(TEXT("msg.timestamp"), message->full_msg_bson_, KeyFound); if (!KeyFound) return false;

	BaseMsg = TSharedPtr<FROSBaseMsg>(new ROSMessages::rasm::RASM_GOAL_MSG(X, Y, Length, Width, Orientation, Time));
	return true;
}

bool URasmGoalMsgConverter::ConvertOutgoingMessage(TSharedPtr<FROSBaseMsg> BaseMsg, bson_t** message) {
	auto GoalMessage = StaticCastSharedPtr<ROSMessages::rasm::RASM_GOAL_MSG>(BaseMsg);
	*message = BCON_NEW(
		"origin_x_meters", BCON_DOUBLE(GoalMessage->origin_x_meters),
		"origin_y_meters", BCON_DOUBLE(GoalMessage->origin_y_meters),
		"length_meters", BCON_DOUBLE(GoalMessage->length_meters),
		"width_meters", BCON_DOUBLE(GoalMessage->width_meters),
		"orientation_radians", BCON_DOUBLE(GoalMessage->orientation_radians),
		"timestamp", BCON_DOUBLE(GoalMessage->timestamp)
	);
	return true;
}
