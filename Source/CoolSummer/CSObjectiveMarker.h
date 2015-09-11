// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "CSCharacterBase.h"
#include "CSObjectiveMarker.generated.h"

class ACSPlayerCharacterBase;

UCLASS()
class COOLSUMMER_API ACSObjectiveMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSObjectiveMarker();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void ReceiveActorBeginOverlap(AActor* Other) override;

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void Start(ACSPlayerCharacterBase* InPlayerCharacter);
	virtual void Start_Implementation(ACSPlayerCharacterBase* InPlayerCharacter);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		bool CheckObjectiveCompleted();
	virtual bool CheckObjectiveCompleted_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void Finished();
	virtual void Finished_Implementation();

	FORCEINLINE int32 GetID()
	{
		return ID;
	}

protected:
	FORCEINLINE bool KillCheck()
	{
		bool Result = true;
		for (ACSCharacterBase* C : NeedToKill)
		{
			if (nullptr != C)
			{
				if (!C->IsDead())
				{
					Result = false;
					break;
				}
			}
		}
		return Result;
	}

protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly) ACSPlayerCharacterBase* PlayerCharacter;

	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) USphereComponent* SphereComponent;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) UBillboardComponent* SpriteComponent;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) UWidgetComponent* WidgetComponent;

	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) bool bFinalObjective;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) int32 ID;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FText Description;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FText Hint;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere, Meta = (ToolTip = "If True, NOT generate any Check. Just finish when Player Character Overlap.")) bool bSimpleTouchObjective;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) TArray<ACSCharacterBase*> NeedToKill;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) ACSObjectiveMarker* Next;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) USoundBase* StartSound;

	static UTexture2D* SpriteTexture;
};
