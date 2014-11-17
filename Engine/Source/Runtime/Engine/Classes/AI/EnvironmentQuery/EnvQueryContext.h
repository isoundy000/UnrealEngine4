// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "EnvQueryContext.generated.h"

UCLASS(Abstract)
class ENGINE_API UEnvQueryContext : public UObject
{
	GENERATED_UCLASS_BODY()

	virtual void ProvideContext(struct FEnvQueryInstance& QueryInstance, struct FEnvQueryContextData& ContextData) const;
};
