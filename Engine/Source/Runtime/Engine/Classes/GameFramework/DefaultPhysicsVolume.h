// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// DefaultPhysicsVolume:  the default physics volume for areas of the level with
// no physics volume specified
//=============================================================================

#pragma once
#include "GameFramework/PhysicsVolume.h"
#include "DefaultPhysicsVolume.generated.h"

/**
* DefaultPhysicsVolume determines the physical effects an actor will experience if they are not inside any user specified PhysicsVolume
*
* @see APhysicsVolume
*/
UCLASS(notplaceable, transient, MinimalAPI)
class ADefaultPhysicsVolume : public APhysicsVolume
{
	GENERATED_UCLASS_BODY()

	// Begin AActor interface
	ENGINE_API virtual void Destroyed() override;
	// End AActor interface
};



