#pragma once

#include <CoreMinimal.h>
#include <UObject/ObjectMacros.h>
#include <UObject/Object.h>
#include "Conversion/Messages/BaseMessageConverter.h"
#include "std_msgs/Int32MultiArray.h"
#include "Conversion/Messages/std_msgs/StdMsgsMultiArrayLayoutConverter.h"

#include "StdMsgsInt32MultiArrayConverter.generated.h"


UCLASS()
class ROSINTEGRATION_API UStdMsgsInt32MultiArrayConverter : public UBaseMessageConverter
{
	GENERATED_UCLASS_BODY()

public:
	virtual bool ConvertIncomingMessage(const ROSBridgePublishMsg* message, TSharedPtr<FROSBaseMsg>& BaseMsg);
	virtual bool ConvertOutgoingMessage(TSharedPtr<FROSBaseMsg> BaseMsg, bson_t** message);

	static bool _bson_extract_child_int32_multi_array(bson_t* b, FString key, ROSMessages::std_msgs::Int32MultiArray* fma, bool LogOnErrors = true)
	{
		bool KeyFound = false;

		KeyFound = UStdMsgsMultiArrayLayoutConverter::_bson_extract_child_multi_array_layout(b, key + ".layout", &fma->layout, LogOnErrors); if (!KeyFound) return false;
		fma->data = GetInt32TArrayFromBSON(key + ".data", b, KeyFound, LogOnErrors); if (!KeyFound) return false;

		return true;
	}

	static void _bson_append_child_float_multi_array(bson_t* b, const char* key, const ROSMessages::std_msgs::Int32MultiArray* ima)
	{
		bson_t layout;
		BSON_APPEND_DOCUMENT_BEGIN(b, key, &layout);
		_bson_append_int32_multi_array(&layout, ima);
		bson_append_document_end(b, &layout);
	}

	static void _bson_append_int32_multi_array(bson_t* b, const ROSMessages::std_msgs::Int32MultiArray* ima)
	{
		UStdMsgsMultiArrayLayoutConverter::_bson_append_child_multi_array_layout(b, "layout", &ima->layout);
		_bson_append_int32_tarray(b, "data", ima->data);
	}
};
