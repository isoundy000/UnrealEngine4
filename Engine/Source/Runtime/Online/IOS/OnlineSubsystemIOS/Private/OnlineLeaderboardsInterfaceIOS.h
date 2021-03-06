// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnlineLeaderboardInterface.h"
#include "OnlineSubsystemIOSTypes.h"

class FOnlineLeaderboardsIOS : public IOnlineLeaderboards
{
private:
	class FOnlineIdentityIOS* IdentityInterface;

	class FOnlineFriendsIOS* FriendsInterface;

	NSMutableArray* UnreportedScores;

    bool ReadLeaderboardCompletionDelegate(NSArray* players, FOnlineLeaderboardReadRef& ReadObject);
    
PACKAGE_SCOPE:

	FOnlineLeaderboardsIOS(FOnlineSubsystemIOS* InSubsystem);

public:

	virtual ~FOnlineLeaderboardsIOS();

	// Begin IOnlineLeaderboards interface
	virtual bool ReadLeaderboards(const TArray< TSharedRef<const FUniqueNetId> >& Players, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual bool ReadLeaderboardsForFriends(int32 LocalUserNum, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual void FreeStats(FOnlineLeaderboardRead& ReadObject) override;
	virtual bool WriteLeaderboards(const FName& SessionName, const FUniqueNetId& Player, FOnlineLeaderboardWrite& WriteObject) override;
	virtual bool FlushLeaderboards(const FName& SessionName) override;
	virtual bool WriteOnlinePlayerRatings(const FName& SessionName, int32 LeaderboardId, const TArray<FOnlinePlayerScore>& PlayerScores) override;
	// End IOnlineLeaderboards interface

};

typedef TSharedPtr<FOnlineLeaderboardsIOS, ESPMode::ThreadSafe> FOnlineLeaderboardsIOSPtr;