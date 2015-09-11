// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "GameFramework/Character.h"
#include "CSCameraActor.h"
#include "CSAnimNodeAcceptable.h"
#include "CSGameInstance.h"
#include "CSCharacterBase.generated.h"

class UCSWeaponBase;
class ACSProjectileBase;
class ACSShellBase;
class ACSExplosive;

class ACSCharacterBase;

UENUM(BlueprintType)
enum class ECSMovementState
{
	Idle, Prone, Crouch, Walk, Run, Sprint, Count
};

USTRUCT(BlueprintType)
struct FCSMovementInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(Category = "CS", BlueprintReadWrite, Meta = (Tooltip = "Desire Movement Speed in this State.")) float MoveSpeed;
	UPROPERTY(Category = "CS", BlueprintReadWrite, Meta = (Tooltip = "Character's Rotation Speed in this State.")) float RotateSpeed;
};

UENUM(BlueprintType)
enum class ECSBodyPart
{
	Head, UpperBody, Other
};

USTRUCT(BlueprintType)
struct FCSDamageInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FORCEINLINE FCSDamageInfo(ACSCharacterBase* InInstigator = nullptr, const int32 InDamage = 0, const FVector& InWorldLocation = FVector::ZeroVector, const FVector& InWorldDirection = FVector::ZeroVector, const ECSBodyPart InHitPart = ECSBodyPart::Other)
		: Instigator(InInstigator), Damage(InDamage), WorldLocation(InWorldLocation), WorldDirection(InWorldDirection), HitPart(InHitPart)
	{
	}

	UPROPERTY(Category = "CS", BlueprintReadWrite) ACSCharacterBase* Instigator;
	UPROPERTY(Category = "CS", BlueprintReadWrite) int32 Damage;
	UPROPERTY(Category = "CS", BlueprintReadWrite) FVector WorldLocation;
	UPROPERTY(Category = "CS", BlueprintReadWrite) FVector WorldDirection;
	UPROPERTY(Category = "CS", BlueprintReadWrite) TEnumAsByte<ECSBodyPart> HitPart;
};

UCLASS()
class COOLSUMMER_API ACSCharacterBase : public ACharacter, public ICSAnimNodeAcceptable
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) static FCSMovementInfo GetMovementInfo(const ECSMovementState State);
	
	ACSCharacterBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void AcceptAnimPlayer(FAnimNode_CSAnimPlayer* Node) override;
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) 
		bool ContainsAnimPlayer(const FName NodeName);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) 
		FAnimNode_CSAnimPlayer& GetAnimPlayer(const FName NodeName);
	UFUNCTION(Category = "CS", BlueprintCallable) 
		bool SetAnimPlayerCurrentTime(const FName NodeName, const float Seconds);
	
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void UpdateState(float DeltaSeconds);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void UpdateMovementState(float DeltaSeconds);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void UpdateCameraState(float DeltaSeconds);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void UpdateIK(float DeltaSeconds);
	virtual void UpdateState_Implementation(float DeltaSeconds);
	virtual void UpdateMovementState_Implementation(float DeltaSeconds);
	virtual void UpdateCameraState_Implementation(float DeltaSeconds);
	virtual void UpdateIK_Implementation(float DeltaSeconds);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FRotator CalcShootNoise(UCSWeaponBase* Weapon);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Meta = (Tooltip = "World Direction.")) 
		FVector CalcShootDirection(UCSWeaponBase* Weapon, const FVector& MuzzleLocation);
	virtual FRotator CalcShootNoise_Implementation(UCSWeaponBase* Weapon);
	virtual FVector CalcShootDirection_Implementation(UCSWeaponBase* Weapon, const FVector& MuzzleLocation);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Meta = (Tooltip = "Damage for ME. NOT for others."))
		float GetDamageMultiplier(const ECSBodyPart HitPart);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		int32 ReceiveDamage(const FCSDamageInfo& DamageInfo);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void Dead(const FCSDamageInfo& FinalBlow);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void Killed(ACSCharacterBase* DeadByMe, const FCSDamageInfo& FinalBlow);
	virtual float GetDamageMultiplier_Implementation(const ECSBodyPart HitPart);
	virtual int32 ReceiveDamage_Implementation(const FCSDamageInfo& DamageInfo);
	virtual void Dead_Implementation(const FCSDamageInfo& FinalBlow);
	virtual void Killed_Implementation(ACSCharacterBase* DeadByMe, const FCSDamageInfo& FinalBlow);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void TakeHit(ACSProjectileBase* HitProjectile, const FVector& Location, const FVector& Direction, const ECSBodyPart HitPart);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void TakeExplosion(ACSExplosive* Exploded, ACSCharacterBase* InInstigator, const FVector& Direction, const int32 Damage, const float Alpha);
	virtual void TakeHit_Implementation(ACSProjectileBase* HitProjectile, const FVector& Location, const FVector& Direction, const ECSBodyPart HitPart);
	virtual void TakeExplosion_Implementation(ACSExplosive* Exploded, ACSCharacterBase* InInstigator, const FVector& Direction, const int32 Damage, const float Alpha);
	
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void OnProjectileShooted(UCSWeaponBase* Weapon, ACSProjectileBase* Projectile, const FVector& Location, const FVector& Direction, const FRotator& Inaccuracy, const FRotator& Noise, const float Speed, const bool bSoundPlayed, const bool bShellEjected);
	virtual void OnProjectileShooted_Implementation(UCSWeaponBase* Weapon, ACSProjectileBase* Projectile, const FVector& Location, const FVector& Direction, const FRotator& Inaccuracy, const FRotator& Noise, const float Speed, const bool bSoundPlayed, const bool bShellEjected);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void OnShellEjected(UCSWeaponBase* Weapon, ACSShellBase* Shell, const FVector& Velocity);
	virtual void OnShellEjected_Implementation(UCSWeaponBase* Weapon, ACSShellBase* Shell, const FVector& Velocity);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetHeadLocation();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetChestLocation();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetPelvisLocation();
	virtual FVector GetHeadLocation_Implementation();
	virtual FVector GetChestLocation_Implementation();
	virtual FVector GetPelvisLocation_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetLeftFootLocation();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetRightFootLocation();
	virtual FVector GetLeftFootLocation_Implementation();
	virtual FVector GetRightFootLocation_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Meta = (ToolTip = "Derived Classes are have calculate AnimRatio. The Other Params are calculated in CSCharacterBase."))
		void GetMovementAnimInfo(float& OutMoveAnimRatio, float& OutRealSpeed, FVector& OutRealDirLocal, FVector& OutRealDirWorld);
	// Derived Classes are have calculate AnimRatio. The Other Params are calculated in CSCharacterBase.
	virtual void GetMovementAnimInfo_Implementation(float& OutMoveAnimRatio, float& OutRealSpeed, FVector& OutRealDirLocal, FVector& OutRealDirWorld);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FRotator GetAimRotationLocal();
	virtual FRotator GetAimRotationLocal_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, Meta = (Tooltip = "Using Actor's Rotation.")) 
		void AddLocalMovement(const FVector& LocalDirection, const float DesiredSpeed, FVector& OutWorldDirection);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void AddWorldMovement(const FVector& WorldDirection, const float DesiredSpeed);
	virtual void AddLocalMovement_Implementation(const FVector& LocalDirection, const float DesiredSpeed, FVector& OutWorldDirection);
	virtual void AddWorldMovement_Implementation(const FVector& WorldDirection, const float DesiredSpeed);
	
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure) 
		bool CheckMovementState(const ECSMovementState Check);
	virtual bool CheckMovementState_Implementation(const ECSMovementState Check);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure) 
		void CheckAllMovementState(bool& OutIdle, bool& OutProne, bool& OutCrouch, bool& OutWalk, bool& OutRun, bool& OutSprint);
	virtual void CheckAllMovementState_Implementation(bool& OutIdle, bool& OutProne, bool& OutCrouch, bool& OutWalk, bool& OutRun, bool& OutSprint);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Meta = (Tooltip = "GetVelocity().Z > Limit")) 
		bool CheckJumping(float Limit = 1);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Meta = (Tooltip = "GetVelocity().Z < Limit"))
		bool CheckFalling(float Limit = -1);
	virtual bool CheckJumping_Implementation(float Limit);
	virtual bool CheckFalling_Implementation(float Limit);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		ECSBodyPart CheckBodyPart(const FName BoneName);
	virtual ECSBodyPart CheckBodyPart_Implementation(const FName BoneName);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		bool HasAnyAmmoFor(UCSWeaponBase* Weapon);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		int32 ConsumeAmmoFor(UCSWeaponBase* Weapon, const int32 WantToConsume);
	virtual bool HasAnyAmmoFor_Implementation(UCSWeaponBase* Weapon);
	virtual int32 ConsumeAmmoFor_Implementation(UCSWeaponBase* Weapon, const int32 WantToConsume);

	FORCEINLINE int32 GetHitPoint()
	{
		return HitPoint;
	}
	FORCEINLINE int32 GetMaxHitPoint()
	{
		return MaxHitPoint;
	}
	FORCEINLINE bool IsDead()
	{
		return bDead;
	}

protected:
	UPROPERTY(Category = "CS", BlueprintReadWrite) UCSWeaponBase* MainWeapon;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere, Meta = (ToolTip = "Optional, It will be the MainWeapon when BeginPlay.")) TSubclassOf<UCSWeaponBase> StartWeaponClass;

	UPROPERTY(Category = "CS", BlueprintReadWrite) TEnumAsByte<ECSMovementState> MovementState;

	TMap<FName, FAnimNode_CSAnimPlayer*> AnimPlayerNodes;

	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) int32 HitPoint;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) int32 MaxHitPoint;
	UPROPERTY(Category = "CS", BlueprintReadWrite, VisibleAnywhere) bool bDead;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) bool bInvincible;

	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bJumping;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bFalling;
	UPROPERTY(Category = "CS", BlueprintReadWrite) bool bZooming;
	UPROPERTY(Category = "CS", BlueprintReadWrite) bool bFiring;
	UPROPERTY(Category = "CS", BlueprintReadWrite) bool bReloading;
	UPROPERTY(Category = "CS", BlueprintReadWrite) bool bReloadWhole;

	UPROPERTY(Category = "CS", BlueprintReadOnly) float FallingTime;

	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FName WeaponSocketName;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FName HeadSocketName;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FName ChestSocketName;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FName PelvisSocketName;

	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FName LeftFootSocketName;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FName RightFootSocketName;

	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) TArray<FName> ResetAnimNamesWhenShooted;

	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) TArray<FName> BodyPartNamesHead;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) TArray<FName> BodyPartNamesUpperBody;

	static const FCSMovementInfo MovementInfos[(int32)ECSMovementState::Count];
};
