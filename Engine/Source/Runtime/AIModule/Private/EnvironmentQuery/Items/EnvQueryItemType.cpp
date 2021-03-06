// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AIModulePrivate.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/Items/EnvQueryItemType.h"

UEnvQueryItemType::UEnvQueryItemType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// register in known types 
	if (HasAnyFlags(RF_ClassDefaultObject) && !GetClass()->HasAnyClassFlags(CLASS_Abstract))
	{
		UEnvQueryManager::RegisteredItemTypes.Add(GetClass());
	}
}

void UEnvQueryItemType::FinishDestroy()
{
	// unregister from known types 
	UEnvQueryManager::RegisteredItemTypes.RemoveSingleSwap(GetClass());

	Super::FinishDestroy();
}

void UEnvQueryItemType::AddBlackboardFilters(FBlackboardKeySelector& KeySelector, UObject* FilterOwner) const
{
}

bool UEnvQueryItemType::StoreInBlackboard(FBlackboardKeySelector& KeySelector, UBlackboardComponent* Blackboard, const uint8* RawData) const
{
	return false;
}

FString UEnvQueryItemType::GetDescription(const uint8* RawData) const
{
	return TEXT("item");
}
