#include "ROSIntegrationGameInstance.h"
#include "Internationalization/Regex.h"

#include "RI/Topic.h"
#include "RI/Service.h"

#include "ROSTime.h"
#include "rosgraph_msgs/Clock.h"

#include "Misc/App.h"

static void MarkAllROSObjectsAsDisconnected()
{
	for (TObjectIterator<UTopic> It; It; ++It)
	{
		UTopic* Topic = *It;

		Topic->MarkAsDisconnected();  
	}
	for (TObjectIterator<UService> It; It; ++It)
	{
		UService* Service = *It;

		Service->MarkAsDisconnected();   
	}
}

// TODO: Do not init each time ROS is rebooted
void UROSIntegrationGameInstance::Init()
{
	if (bConnectToROS)
	{
		UE_LOG(LogROS, Display, TEXT("UROSIntegrationGameInstance::Init() - connecting to ROS bridge..."));

		FROSTime::SetUseSimTime(false);

		if (ROSIntegrationCore)
		{
			UROSIntegrationCore* oldRosCore = ROSIntegrationCore;
			ROSIntegrationCore = nullptr;
			oldRosCore->ConditionalBeginDestroy();
		}

		ROSIntegrationCore = NewObject<UROSIntegrationCore>(UROSIntegrationCore::StaticClass()); // ORIGINAL 
		bIsConnected = ROSIntegrationCore->Init(ROSBridgeServerHost, ROSBridgeServerPort);

		if (!bTimerSet)
		{
			bTimerSet = true;
			GetTimerManager().SetTimer(TimerHandle_CheckHealth, this, &UROSIntegrationGameInstance::CheckROSBridgeHealth, 1.0f, true, 2.0f);
		}

		if (bIsConnected)
		{
			UWorld* CurrentWorld = GetWorld();
			if (CurrentWorld)
			{
				ROSIntegrationCore->SetWorld(CurrentWorld);
				ROSIntegrationCore->InitSpawnManager();
			}
			else
			{
				UE_LOG(LogROS, Display, TEXT("World not available in UROSIntegrationGameInstance::Init()!"));
			}
		}
		else if (!bReconnect)
		{
			UE_LOG(LogROS, Error, TEXT("Failed to connect to server %s:%u. Please make sure that your rosbridge is running."), *ROSBridgeServerHost, ROSBridgeServerPort);
		}

		if (bSimulateTime)
		{
			FApp::SetFixedDeltaTime(FixedUpdateInterval);
			FApp::SetUseFixedTimeStep(bUseFixedUpdateInterval);

			// tell ROSIntegration to use simulated time
			FROSTime now = FROSTime::Now();
			FROSTime::SetUseSimTime(true);
			FROSTime::SetSimTime(now);

			FWorldDelegates::OnWorldTickStart.AddUObject(this, &UROSIntegrationGameInstance::OnWorldTickStart);

			ClockTopic = NewObject<UTopic>(UTopic::StaticClass()); // ORIGINAL
			ClockTopic->Init(ROSIntegrationCore, FString(TEXT("/clock")), FString(TEXT("rosgraph_msgs/Clock")), 3);
			ClockTopic->Advertise();
		}
	}
}

bool UROSIntegrationGameInstance::Validate(FString ROSIP)
{
	// Regex expression for validating IPv4
	const FRegexPattern ipv4(TEXT("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])"));
	FRegexMatcher matcher(ipv4, ROSIP);

	// Return
	return matcher.FindNext() && ROSIP == matcher.GetCaptureGroup(0);
}

void UROSIntegrationGameInstance::CheckROSBridgeHealth()
{
	if (!bCheckHealth || (bIsConnected && ROSIntegrationCore->IsHealthy()))
	{
		return;
	}
	
	UE_LOG(LogROS, Error, TEXT("Connection to rosbridge %s:%u was interrupted."), *ROSBridgeServerHost, ROSBridgeServerPort);
	SetROSConnect(false, ROSBridgeServerHost);
}

void UROSIntegrationGameInstance::Shutdown()
{
	UE_LOG(LogROS, Display, TEXT("ROS Game Instance - shutdown start"));
	if (bConnectToROS)
	{
		if (bTimerSet) GetTimerManager().ClearTimer(TimerHandle_CheckHealth);

		if (bSimulateTime)
		{
			FWorldDelegates::OnWorldTickStart.RemoveAll(this);
		}

		MarkAllROSObjectsAsDisconnected();

		UE_LOG(LogROS, Display, TEXT("ROS Game Instance - shutdown done"));
	}
}

void UROSIntegrationGameInstance::BeginDestroy()
{
	if (bConnectToROS) 
	{
		UE_LOG(LogROS, Display, TEXT("ROS Game Instance - begin destroy - start"));
	}

	Super::BeginDestroy();

	UE_LOG(LogROS, Display, TEXT("ROS Game Instance - begin destroy - done"));
}

void UROSIntegrationGameInstance::SetROSConnect(bool doConnect, FString ROSIP)
{
	if (doConnect && Validate(ROSIP))
	{
		bConnectToROS = true;
		ROSBridgeServerHost = ROSIP;
		bReconnect = false;

		Init();
	}
	else
	{
		bConnectToROS = false;
		bReconnect = false;
		if (bTimerSet)
		{
			GetTimerManager().ClearTimer(TimerHandle_CheckHealth);
			bTimerSet = false;
		}

		MarkAllROSObjectsAsDisconnected();

		if (ROSIntegrationCore)
		{
			UROSIntegrationCore* oldRosCore = ROSIntegrationCore;
			ROSIntegrationCore = nullptr;
			oldRosCore->ConditionalBeginDestroy();
		}

		bIsConnected = false;
	}
}

void UROSIntegrationGameInstance::OnWorldTickStart(UWorld * World, ELevelTick TickType, float DeltaTime)
{
	if (bSimulateTime && TickType == ELevelTick::LEVELTICK_TimeOnly)
	{
		FApp::SetFixedDeltaTime(FixedUpdateInterval);
		FApp::SetUseFixedTimeStep(bUseFixedUpdateInterval);

		FROSTime now = FROSTime::Now();

		// advance ROS time
		unsigned long seconds = (unsigned long)DeltaTime;
		unsigned long long nanoseconds = (unsigned long long)(DeltaTime * 1000000000ul);
		unsigned long nanoseconds_only = nanoseconds - (seconds * 1000000000ul);

		now._Sec += seconds;
		now._NSec += nanoseconds_only;

		if (now._NSec >= 1000000000ul)
		{
			now._Sec += 1;
			now._NSec -= 1000000000ul;
		}

		// internal update for ROSIntegration
		FROSTime::SetSimTime(now);

		// send /clock topic to let everyone know what time it is...
		TSharedPtr<ROSMessages::rosgraph_msgs::Clock> ClockMessage(new ROSMessages::rosgraph_msgs::Clock(now));
		ClockTopic->Publish(ClockMessage);
	}
}

