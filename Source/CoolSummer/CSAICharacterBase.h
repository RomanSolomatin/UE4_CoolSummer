// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "CSCharacterBase.h"
#include "CSAIControllerBase.h"
#include "CSPlayerCharacterBase.h"
#include "CSUtil.h"
#include "CSAICharacterBase.generated.h"

class UCSWeaponBase;

UCLASS()
class COOLSUMMER_API ACSAICharacterBase : public ACSCharacterBase
{
	GENERATED_BODY()

public:
	ACSAICharacterBase();

	virtual void PossessedBy(AController* InController) override;
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	virtual void UpdateState_Implementation(float DeltaSeconds);
	virtual void UpdateMovementState_Implementation(float DeltaSeconds);
	virtual void UpdateCameraState_Implementation(float DeltaSeconds);
	virtual void UpdateIK_Implementation(float DeltaSeconds);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void UpdateHoldPositionState(float DeltaSeconds);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void UpdatePatrolState(float DeltaSeconds);
	virtual void UpdateHoldPositionState_Implementation(float DeltaSeconds);
	virtual void UpdatePatrolState_Implementation(float DeltaSeconds);

	virtual FRotator CalcShootNoise_Implementation(UCSWeaponBase* Weapon) override;
	virtual FVector CalcShootDirection_Implementation(UCSWeaponBase* Weapon, const FVector& MuzzleLocation);

	virtual void TakeHit_Implementation(ACSProjectileBase* HitProjectile, const FVector& Location, const FVector& Direction, const ECSBodyPart HitPart) override;

	virtual void GetMovementAnimInfo_Implementation(float& OutMoveAnimRatio, float& OutRealSpeed, FVector& OutRealDirLocal, FVector& OutRealDirWorld) override;

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void HearNoise(ACSPlayerCharacterBase* Maker);
	virtual void HearNoise_Implementation(ACSPlayerCharacterBase* Maker);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		bool SetNewTarget(ACSPlayerCharacterBase* Target);
	virtual bool SetNewTarget_Implementation(ACSPlayerCharacterBase* Target);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		bool CheckHoldPositionState(const ECSHoldPositionState Check);
	virtual bool CheckHoldPositionState_Implementation(const ECSHoldPositionState Check);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		bool CheckPatrolState(const ECSPatrolState Check);
	virtual bool CheckPatrolState_Implementation(const ECSPatrolState Check);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetEyeLocation();
	virtual FVector GetEyeLocation_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		bool InHome();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		bool NearlyHome(float Tolerance);
	virtual bool InHome_Implementation();
	virtual bool NearlyHome_Implementation(float Tolerance);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void CallComrades(ACSPlayerCharacterBase* NewTarget);
	virtual void CallComrades_Implementation(ACSPlayerCharacterBase* NewTarget);

	FORCEINLINE ACSPlayerCharacterBase* GetCurrentTarget()
	{
		return CurrentTarget;
	}
	FORCEINLINE int32 GetTemperaturePerAttack()
	{
		return TemperaturePerAttack;
	}

protected:
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		bool SphereTraceToPlayerCharacter(ACSPlayerCharacterBase* Check, const FVector& Start, const FVector& End, const float Radius, TSubclassOf<AActor> IgnoreClass);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		bool IsInWatchSight(ACSPlayerCharacterBase* Check, bool& OutInDistance, bool& OutInDegrees, bool& OutCanSee);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, Meta = (ToolTip = "nullptr if watched nothing."))
		ACSPlayerCharacterBase* Watch();
	virtual bool SphereTraceToPlayerCharacter_Implementation(ACSPlayerCharacterBase* Check, const FVector& Start, const FVector& End, const float Radius, TSubclassOf<AActor> IgnoreClass);
	virtual bool IsInWatchSight_Implementation(ACSPlayerCharacterBase* Check, bool& OutInDistance, bool& OutInDegrees, bool& OutCanSee);
	virtual ACSPlayerCharacterBase* Watch_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		bool CanShootHim(ACSPlayerCharacterBase* He, bool& OutInRange, bool& OutCanChest, bool& OutCanPelvis, bool& OutCanHead, FVector& OutHisChestLocation, FVector& OutHisPelvisLocation, FVector& OutHisHeadLocation, FVector& OutMyMuzzleLocation);
	virtual bool CanShootHim_Implementation(ACSPlayerCharacterBase* He, bool& OutInRange, bool& OutCanChest, bool& OutCanPelvis, bool& OutCanHead, FVector& OutHisChestLocation, FVector& OutHisPelvisLocation, FVector& OutHisHeadLocation, FVector& OutMyMuzzleLocation);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void NextPatrolWaypointIndex();
	virtual void NextPatrolWaypointIndex_Implementation();

protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) ACSAIControllerBase* ACBase;

	UPROPERTY(Category = "CS", BlueprintReadWrite, VisibleAnywhere) ACSPlayerCharacterBase* CurrentTarget;
	//UPROPERTY(Category = "CS", BlueprintReadWrite, VisibleAnywhere) TArray<FCSAgro> Agros;
	UPROPERTY(Category = "CS", BlueprintReadWrite, VisibleAnywhere) FVector CurrentShootLocation;
	
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) FVector HomeLocation;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) FRotator HomeRotation;

	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) float UpdateTimer;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) float DeadTimer;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) float SearchTimer;
	UPROPERTY(Category = "CS", BlueprintReadWrite, VisibleAnywhere) bool bNeedUpdatePatrolTimer;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) float PatrolTimer;

	UPROPERTY(Category = "AI Common Settings", BlueprintReadOnly, EditAnywhere) TEnumAsByte<ECSAIType> AIType;
	UPROPERTY(Category = "AI Common Settings", BlueprintReadOnly, EditAnywhere) float WatchingDistance;
	UPROPERTY(Category = "AI Common Settings", BlueprintReadOnly, EditAnywhere, Meta = (ToolTip = "X = Up, Y = Down, Z = Horizontal")) FVector WatchingDegrees;
	UPROPERTY(Category = "AI Common Settings", BlueprintReadOnly, EditAnywhere) float ShootableRange;
	UPROPERTY(Category = "AI Common Settings", BlueprintReadOnly, EditAnywhere) TArray<ACSAICharacterBase*> Comrades;
	UPROPERTY(Category = "AI Common Settings", BlueprintReadOnly, EditAnywhere) int32 TemperaturePerAttack;
	UPROPERTY(Category = "AI Common Settings", BlueprintReadOnly, EditAnywhere) int32 KillExperience;

	UPROPERTY(Category = "AI Hold Position Settings", BlueprintReadOnly, EditAnywhere) float HomeRadius;

	UPROPERTY(Category = "AI Patrol Settings", BlueprintReadOnly, EditAnywhere) TEnumAsByte<ECSPatrolMode> PatrolMode;
	UPROPERTY(Category = "AI Patrol Settings", BlueprintReadOnly, EditAnywhere) bool bPatrolRoamPositive;
	UPROPERTY(Category = "AI Patrol Settings", BlueprintReadOnly, EditAnywhere) TArray<AActor*> PatrolWaypoints;
	UPROPERTY(Category = "AI Patrol Settings", BlueprintReadOnly, EditAnywhere) int32 PatrolWaypointIndex;
	UPROPERTY(Category = "AI Patrol Settings", BlueprintReadOnly, EditAnywhere) float PatrolWaitTimeInWaypoint;

	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) TEnumAsByte<ECSHoldPositionState> HoldPositionState;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) TEnumAsByte<ECSPatrolState> PatrolState;
};
