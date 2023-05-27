#include "LB_GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "Kismet/GameplayStatics.h"

void ULB_GameInstance::Init()
{
	// Forzar subida. 
	Super::Init();

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ULB_GameInstance::OnCreateSessionCompleted);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ULB_GameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ULB_GameInstance::OnJoinSessionComplete);
		}
	}
}

void ULB_GameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings Settings;
		Settings.bAllowJoinInProgress = true;	// Uniser sobre la marcha
		Settings.bIsDedicated = false;			// ¿servidor dedicado?
		Settings.bIsLANMatch = true;			// LAN
		Settings.bShouldAdvertise = true;		// Pueden ver nuestra sesión
		Settings.bUsesPresence = true;			// Que tenga precencia
		Settings.NumPublicConnections = 4;		// Players
		SessionInterface->CreateSession(0, FName("H64"), Settings);
	}
}

void ULB_GameInstance::FindSession()
{
	if (SessionInterface.IsValid())
	{
		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 5;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void ULB_GameInstance::CustomJoinSession(FBlueprintSessionResult SessionBlueprint)
{
	FOnlineSessionSearchResult ResultSearch = SessionBlueprint.OnlineResult;
	if (SessionInterface.IsValid() && ResultSearch.IsValid())
	{
		SessionInterface->JoinSession(0, *ResultSearch.GetSessionIdStr(), ResultSearch);
	}
}

FBlueprintSessionResult ULB_GameInstance::GetBPSessionResult(int32 Index) const
{
	FBlueprintSessionResult SessionResult;
	if (SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
	{
		SessionResult.OnlineResult = SessionSearch->SearchResults[Index];
	}

	return SessionResult;
}

void ULB_GameInstance::OnCreateSessionCompleted(FName SessionName, bool bSucceded)
{
	if (bSucceded)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	}
}

void ULB_GameInstance::OnFindSessionComplete(bool bSucceded)
{
	if (bSucceded && SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
	{
		FOnlineSessionSearchResult CurrentResult = SessionSearch->SearchResults[0];
		CurrentResult.Session.NumOpenPublicConnections;
		OnSessionsFound.Broadcast();
	}
}

void ULB_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type SessionType)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress;
		if (SessionInterface->GetResolvedConnectString(SessionName, JoinAddress))
		{
			UE_LOG(LogTemp, Warning, TEXT("JOIMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"));
			PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}
