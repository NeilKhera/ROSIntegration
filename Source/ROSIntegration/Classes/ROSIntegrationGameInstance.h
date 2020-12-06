#pragma once

#include <CoreMinimal.h>
#include <Engine/GameInstance.h>
#include <Engine/EngineTypes.h>
#include <Runtime/Launch/Resources/Version.h>
#include "ROSIntegrationCore.h"

#include "ROSIntegrationGameInstance.generated.h"

UCLASS()
class ROSINTEGRATION_API UROSIntegrationGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "ROS")
    void SetROSConnect(bool doConnect, FString ROSIP);

	UPROPERTY()
	UROSIntegrationCore* ROSIntegrationCore = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ROS")
	FString ROSBridgeServerHost = "127.0.0.1";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ROS")
	int32 ROSBridgeServerPort = 9090;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ROS")
	bool bConnectToROS = false;

	UPROPERTY(BlueprintReadOnly, Category = "ROS")
	bool bIsConnected = false;

	UPROPERTY(EditAnywhere, Category = "ROS")
	bool bSimulateTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS")
	bool bUseFixedUpdateInterval = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS")
	float FixedUpdateInterval = 0.01666666667;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS")
    bool bCheckHealth = true;

protected:
	void CheckROSBridgeHealth();
	virtual void OnWorldTickStart(UWorld * World, ELevelTick TickType, float DeltaTime);

	FTimerHandle TimerHandle_CheckHealth;
	bool bTimerSet = false;
	bool bReconnect = false;

	UPROPERTY()
	class UTopic* ClockTopic = nullptr;
};

