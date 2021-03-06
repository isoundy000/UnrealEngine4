// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameplayTask.h"
#include "AITypes.h"
#include "AITask.generated.h"

class AAIController;

UENUM()
enum class EAITaskPriority : uint8
{
	Lowest = 0,
	Low = 64, //FGameplayTasks::DefaultPriority / 2,
	AutonomousAI = 127, //FGameplayTasks::DefaultPriority,
	High = 192, //(1.5 * FGameplayTasks::DefaultPriority),
	Ultimate = 255,
};

UCLASS(Abstract)
class AIMODULE_API UAITask : public UGameplayTask
{
	GENERATED_BODY()
protected:

	UPROPERTY()
	AAIController* OwnerController;

	virtual void Activate() override;

public:
	UAITask(const FObjectInitializer& ObjectInitializer);

	static AAIController* GetAIControllerForActor(AActor* Actor);
	AAIController* GetAIController() const { return OwnerController; };

	void InitAITask(AAIController& AIOwner, IGameplayTaskOwnerInterface& InTaskOwner, uint8 InPriority);

	/** effectively adds UAIResource_Logic to the set of Claimed resources */
	void RequestAILogicLocking();
};