#include "Conversion/Messages/std_msgs/StdMsgsInt32MultiArrayConverter.h"

UStdMsgsInt32MultiArrayConverter::UStdMsgsInt32MultiArrayConverter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	_MessageType = "std_msgs/Int32MultiArray";
}

bool UStdMsgsInt32MultiArrayConverter::ConvertIncomingMessage(const ROSBridgePublishMsg* message, TSharedPtr<FROSBaseMsg>& BaseMsg)
{
	auto p = new ROSMessages::std_msgs::Int32MultiArray;
	BaseMsg = TSharedPtr<FROSBaseMsg>(p);
	return _bson_extract_child_int32_multi_array(message->full_msg_bson_, "msg", p);
}

bool UStdMsgsInt32MultiArrayConverter::ConvertOutgoingMessage(TSharedPtr<FROSBaseMsg> BaseMsg, bson_t** message)
{
	auto ima = StaticCastSharedPtr<ROSMessages::std_msgs::Int32MultiArray>(BaseMsg);

	*message = new bson_t;
	bson_init(*message);
	_bson_append_int32_multi_array(*message, ima.Get());

	return true;
}
