// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "CSCharacterBase.h"
#include "Curves/CurveFloat.h"
#include "CSPlayerCharacterBase.generated.h"

class ACSPlayerControllerBase;
class ACSProjectileBase;
class ACSObjectiveMarker;

/**
 * 
 */
UCLASS()
class COOLSUMMER_API ACSPlayerCharacterBase : public ACSCharacterBase
{
	GENERATED_BODY()
	
public:
	ACSPlayerCharacterBase();

	virtual void PossessedBy(AController* InController) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void UpdateState_Implementation(float DeltaSeconds);
	virtual void UpdateMovementState_Implementation(float DeltaSeconds);
	virtual void UpdateCameraState_Implementation(float DeltaSeconds);
	virtual void UpdateIK_Implementation(float DeltaSeconds);

	virtual FRotator CalcShootNoise_Implementation(UCSWeaponBase* Weapon) override;
	virtual FVector CalcShootDirection_Implementation(UCSWeaponBase* Weapon, const FVector& MuzzleLocation) override;

	virtual float GetDamageMultiplier_Implementation(const ECSBodyPart HitPart) override;

	virtual void TakeHit_Implementation(ACSProjectileBase* HitProjectile, const FVector& Location, const FVector& Direction, const ECSBodyPart HitPart) override;
	virtual void TakeExplosion_Implementation(ACSExplosive* Exploded, ACSCharacterBase* InInstigator, const FVector& Direction, const int32 Damage, const float Alpha) override;

	virtual void GetMovementAnimInfo_Implementation(float& OutMoveAnimRatio, float& OutRealSpeed, FVector& OutRealDirLocal, FVector& OutRealDirWorld) override;

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void GenerateNoise(const FVector& Location, const float Radius);
	virtual void GenerateNoise_Implementation(const FVector& Location, const float Radius);

	virtual bool HasAnyAmmoFor_Implementation(UCSWeaponBase* Weapon) override;
	virtual int32 ConsumeAmmoFor_Implementation(UCSWeaponBase* Weapon, const int32 WantToConsume) override;

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		float SetMovementState(const ECSMovementState NewState);
	virtual float SetMovementState_Implementation(const ECSMovementState NewState);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void StartSlideAnim(float Duration);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		float UpdateSlideAnim(float DeltaTime);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void StopSlideAnim();
	virtual void StartSlideAnim_Implementation(float Duration);
	virtual float UpdateSlideAnim_Implementation(float DeltaTime);
	virtual void StopSlideAnim_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void ChangeObjective(ACSObjectiveMarker* NewObjective);
	virtual void ChangeObjective_Implementation(ACSObjectiveMarker* NewObjective);

	FORCEINLINE int32 GetTemperature()
	{
		return Temperature;
	}
	FORCEINLINE void AddTemperature(const int32 Add)
	{
		Temperature += Add;
	}
	FORCEINLINE int32 GetMaxTemperature()
	{
		return MaxTemperature;
	}

	FORCEINLINE void SetWantToJump(const bool Value)
	{
		bWantToJump = Value;
	}
	FORCEINLINE void SetWantToSprint(const bool Value)
	{
		bWantToSprint = Value;
	}
	FORCEINLINE void SetWantToFire(const bool Value)
	{
		bWantToFire = Value;
	}
	FORCEINLINE void SetWantToCrouch(const bool Value)
	{
		bWantToCrouch = Value;
	}
	FORCEINLINE void SetWantToZoom(const bool Value)
	{
		bWantToZoom = Value;
	}
	FORCEINLINE void SetWantToReload(const bool Value)
	{
		bWantToReload = Value;
	}

protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly) ACSPlayerControllerBase* PCBase;
	UPROPERTY(Category = "CS", BlueprintReadOnly) ACSCameraActor* Camera;

	UPROPERTY(Category = "CS", BlueprintReadOnly) ACSObjectiveMarker* CurrentObjective;

	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) int32 Temperature;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) int32 MaxTemperature;

	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bWantToJump;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bWantToSprint;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bWantToFire;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bWantToCrouch;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bWantToZoom;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bWantToReload;

	UPROPERTY(Category = "CS", BlueprintReadOnly) float CrouchInputDelay;

	UPROPERTY(Category = "CS", BlueprintReadOnly) float DesireMoveSpeed;
	UPROPERTY(Category = "CS", BlueprintReadOnly) FVector DesireMoveDirectionLocal;
	UPROPERTY(Category = "CS", BlueprintReadOnly) FVector DesireMoveDirectionWorld;
	UPROPERTY(Category = "CS", BlueprintReadOnly) float LastMoveSpeed;
	UPROPERTY(Category = "CS", BlueprintReadOnly) float RotateSpeed;

	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bInSlideAnim;
	UPROPERTY(Category = "CS", BlueprintReadOnly) float SlideAnimDuration;
	UPROPERTY(Category = "CS", BlueprintReadOnly) float SlideAnimTime;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) UCurveFloat* SlideAnimCurve;

	UPROPERTY(Category = "CS", BlueprintReadOnly) FVector LHandIKLocation;

	UPROPERTY(Category = "CS", BlueprintReadWrite) int32 AmmoCount;
	UPROPERTY(Category = "CS", BlueprintReadOnly) int32 MaxAmmoCount;
};
