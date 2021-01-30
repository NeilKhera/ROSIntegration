#pragma once

#include <CoreMinimal.h>
#include <Engine/GameInstance.h>
#include <Engine/EngineTypes.h>
#include <Runtime/Launch/Resources/Version.h>

#include "ROSIntegrationCore.h"
#include "Internationalization/Regex.h"

#include "ROSIntegrationGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FStartTime {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test") int32 year;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test") int32 month;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test") int32 day;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test") int32 hour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test") int32 minute;

	FStartTime() {
		year = 1970;
		month = 1;
		day = 1;
		hour = 0;
		minute = 0;
	}
};

UCLASS()
class ROSINTEGRATION_API UROSIntegrationGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UROSIntegrationCore* ROSIntegrationCore = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS")
	FStartTime start_time;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ROS")
	FString ROSBridgeServerHost = "127.0.0.1";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ROS")
	int32 ROSBridgeServerPort = 9090;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ROS")
	bool bConnectToROS = false;

	UPROPERTY(BlueprintReadOnly, Category = "ROS")
	bool bIsConnected = false;

<<<<<<< HEAD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS")
	bool bSimulateTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS")
	bool bUseFixedUpdateInterval = false;

=======
>>>>>>> 3e650fb (Lots of cleanup)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS")
	bool bSimulateTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS")
    bool bCheckHealth = true;

protected:
	bool bTimerSet = false;
	bool bReconnect = false;

	FTimerHandle TimerHandle_CheckHealth;

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "ROS")
	void SetROSConnect(bool doConnect, FString ROSIP);

protected:
	bool Validate(FString ROSIP);

	void EnableROS();
	void DisableROS();

	void CheckROSBridgeHealth();

};