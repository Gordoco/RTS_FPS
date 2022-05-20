// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online.h"
#include "RTS_FPSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API URTS_FPSGameInstance : public UGameInstance
{
	GENERATED_BODY()

	URTS_FPSGameInstance(const FObjectInitializer& ObjectInitializer);

	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

	/**
*	Function fired when a session create request has completed
*
*	@param SessionName the name of the session this callback is for
*	@param bWasSuccessful true if the async action completed without error, false if there was an error
*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Function fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
*	Find an online session
*
*	@param UserId user that initiated the request
*	@param bIsLAN are we searching LAN matches
*	@param bIsPresence are we searching presence sessions
*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	/**
*	Delegate fired when a session search query has completed
*
*	@param bWasSuccessful true if the async action completed without error, false if there was an error
*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
*	Joins a session via a search result
*
*	@param SessionName name of session
*	@param SearchResult Session to join
*
*	@return bool true if successful, false otherwise
*/
	bool RTS_FPSJoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/**
*	Delegate fired when a session join request has completed
*
*	@param SessionName the name of the session this callback is for
*	@param bWasSuccessful true if the async action completed without error, false if there was an error
*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	/**
*	Delegate fired when a destroying an online session has completed
*
*	@param SessionName the name of the session this callback is for
*	@param bWasSuccessful true if the async action completed without error, false if there was an error
*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

public:
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void StartOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void FindOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void JoinOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void DestroySessionAndLeaveGame();

	UFUNCTION(BlueprintImplementableEvent, Category = "TESTING")
		void JoinToServer();
};
