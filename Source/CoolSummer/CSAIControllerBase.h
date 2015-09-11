// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "AIController.h"
#include "CSPlayerCharacterBase.h"
#include "CSAIControllerBase.generated.h"

class ACSAICharacterBase;

UENUM(BlueprintType)
enum class ECSAIType
{
	HoldPosition, Patrol
};

UENUM(BlueprintType)
enum class ECSHoldPositionState
{
	Watch, Chase, Search, Attack, BackHome
};

UENUM(BlueprintType)
enum class ECSPatrolState
{
	Patrol, Chase, Attack
};

UENUM(BlueprintType)
enum class ECSPatrolMode
{
	Roam, Loop
};

USTRUCT()
struct FCSAgro
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(Category = "CS", EditAnywhere, BlueprintReadWrite) ACSPlayerCharacterBase* Target;
	UPROPERTY(Category = "CS", EditAnywhere, BlueprintReadWrite) int32 Value;
};

/**
 * 
 */
UCLASS()
class COOLSUMMER_API ACSAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:
	ACSAIControllerBase(const FObjectInitializer& Initializer);

	virtual void Possess(APawn* InPawn) override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		bool MoveIdle();
	virtual bool MoveIdle_Implementation();
	
protected:
	UPROPERTY(Category = "CS", VisibleAnywhere, BlueprintReadOnly) ACSAICharacterBase* AICharacterBase;
};
