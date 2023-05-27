#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"
#include "LB_GameInstance.generated.h"


//class FOnlineSessionSearch;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSessionsFound);

UCLASS()
class LOBBY_API ULB_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintAssignable)
		FSessionsFound OnSessionsFound;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

protected:

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
		void CreateSession();

	UFUNCTION(BlueprintCallable)
		void FindSession();

	UFUNCTION(BlueprintCallable)
		void CustomJoinSession(FBlueprintSessionResult SessionBlueprint);

	UFUNCTION(BlueprintCallable)
		FBlueprintSessionResult GetBPSessionResult(int32 Index) const;

private:

	void OnCreateSessionCompleted(FName SessionName, bool bSucceded);
	void OnFindSessionComplete(bool bSucceded);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type SessionType);
};
