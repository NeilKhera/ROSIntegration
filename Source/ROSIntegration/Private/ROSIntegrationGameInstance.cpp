#include "ROSIntegrationGameInstance.h"

#include "ROSTime.h"
#include "RI/Topic.h"
#include "RI/Service.h"

static void MarkAllROSObjectsAsDisconnected() {
	for (TObjectIterator<UTopic> It; It; ++It) {
		UTopic* Topic = *It;
		Topic->MarkAsDisconnected();  
	}

	for (TObjectIterator<UService> It; It; ++It) {
		UService* Service = *It;
		Service->MarkAsDisconnected();   
	}
}

void UROSIntegrationGameInstance::Init() {
	UE_LOG(LogROS, Display, TEXT("ROS Game Instance - initialize - start"));
	ReceiveInit();
	if (bConnectToROS) {
		EnableROS();
	}
	UE_LOG(LogROS, Display, TEXT("ROS Game Instance - initialize - done"));
}

void UROSIntegrationGameInstance::Shutdown() {
	UE_LOG(LogROS, Display, TEXT("ROS Game Instance - shutdown - start"));
	if (bConnectToROS) {
		DisableROS();
	}
	UE_LOG(LogROS, Display, TEXT("ROS Game Instance - shutdown - done"));
}

void UROSIntegrationGameInstance::BeginDestroy() {
	UE_LOG(LogROS, Display, TEXT("ROS Game Instance - begin destroy - start"));
	Super::BeginDestroy();
	UE_LOG(LogROS, Display, TEXT("ROS Game Instance - begin destroy - done"));
}

void UROSIntegrationGameInstance::SetROSConnect(bool doConnect, FString ROSIP) {
	ROSBridgeServerHost = ROSIP;
	if (doConnect && Validate(ROSIP)) {
		bConnectToROS = true;
		EnableROS();
	} else {
		DisableROS();
	}
}

bool UROSIntegrationGameInstance::Validate(FString ROSIP) {
	const FRegexPattern ipv4(TEXT("^((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])(\\.(?!$)|$)){4}$"));
	FRegexMatcher matcher(ipv4, ROSIP);
	return matcher.FindNext() && ROSIP == matcher.GetCaptureGroup(0);
}

void UROSIntegrationGameInstance::EnableROS() {
	UE_LOG(LogROS, Display, TEXT("UROSIntegrationGameInstance::Init() - connecting to ROS bridge..."));
	FROSTime::SetUseSimTime(false);

	if (ROSIntegrationCore) {
		UROSIntegrationCore* oldRosCore = ROSIntegrationCore;
		ROSIntegrationCore = nullptr;
		oldRosCore->ConditionalBeginDestroy();
	}

	ROSIntegrationCore = NewObject<UROSIntegrationCore>(UROSIntegrationCore::StaticClass()); // ORIGINAL 
	bConnectToROS = ROSIntegrationCore->Init(ROSBridgeServerHost, ROSBridgeServerPort);

	if (!bTimerSet) {
		bTimerSet = true;
		GetTimerManager().SetTimer(TimerHandle_CheckHealth, this, &UROSIntegrationGameInstance::CheckROSBridgeHealth, 1.0f, true, 2.0f);
	}

	if (bConnectToROS) {
		UWorld* CurrentWorld = GetWorld();
		if (CurrentWorld) {
			ROSIntegrationCore->SetWorld(CurrentWorld);
			ROSIntegrationCore->InitSpawnManager();
		} else {
			UE_LOG(LogROS, Display, TEXT("World not available in UROSIntegrationGameInstance::Init()!"));
		}
	} else {
		UE_LOG(LogROS, Error, TEXT("Failed to connect to server %s:%u. Please make sure that your rosbridge is running."), *ROSBridgeServerHost, ROSBridgeServerPort);
	}
}

void UROSIntegrationGameInstance::DisableROS() {
	bConnectToROS = false;
	if (bTimerSet) {
		GetTimerManager().ClearTimer(TimerHandle_CheckHealth);
		bTimerSet = false;
	}

	MarkAllROSObjectsAsDisconnected();

	if (ROSIntegrationCore) {
		UROSIntegrationCore* oldRosCore = ROSIntegrationCore;
		ROSIntegrationCore = nullptr;
		oldRosCore->ConditionalBeginDestroy();
	}
}

void UROSIntegrationGameInstance::CheckROSBridgeHealth() {
	if (!bCheckHealth || (bConnectToROS && ROSIntegrationCore->IsHealthy())) {
		return;
	}
	
	UE_LOG(LogROS, Error, TEXT("Connection to rosbridge %s:%u was interrupted."), *ROSBridgeServerHost, ROSBridgeServerPort);
	SetROSConnect(false, ROSBridgeServerHost);
}