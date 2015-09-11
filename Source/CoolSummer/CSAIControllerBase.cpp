// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSAICharacterBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CSAIControllerBase.h"

ACSAIControllerBase::ACSAIControllerBase(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
}

void ACSAIControllerBase::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	AICharacterBase = Cast<ACSAICharacterBase>(InPawn);
	if (nullptr == AICharacterBase)
	{
		UE_LOG(CSLog, Warning, TEXT("AI Character Invalid!"));
	}
}

void ACSAIControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (nullptr != AICharacterBase)
	{
		if (nullptr != AICharacterBase->GetCurrentTarget())
		{
			const FVector Dest = AICharacterBase->GetCurrentTarget()->GetActorLocation();
			const FVector Src = AICharacterBase->GetActorLocation();
			SetControlRotation((Dest - Src).Rotation());
		}
		else
		{
			SetControlRotation(AICharacterBase->GetActorRotation());
		}
	}
}

bool ACSAIControllerBase::MoveIdle_Implementation()
{
	return GetMoveStatus() == EPathFollowingStatus::Idle;
}
