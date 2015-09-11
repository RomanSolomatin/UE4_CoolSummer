// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "CSWeaponBase.generated.h"

class ACSCharacterBase;
class ACSProjectileBase;
class ACSShellBase;

UENUM(BlueprintType)
enum class ECSWeaponType
{
	AssaultRifle, Pistol, SniperRifle
};

UENUM(BlueprintType)
enum class ECSWeaponState
{
	Idle, Firing, Reloading
};

/**
 * 
 */
UCLASS(Abstract)
class COOLSUMMER_API UCSWeaponBase : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	UCSWeaponBase(const FObjectInitializer& Initializer);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void Initialize(ACSCharacterBase* InCharacter);
	virtual void Initialize_Implementation(ACSCharacterBase* InCharacter);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		bool StartFire();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		bool StopFire();
	virtual bool StartFire_Implementation();
	virtual bool StopFire_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		bool StartReload();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		bool StopReload();
	virtual bool StartReload_Implementation();
	virtual bool StopReload_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void UpdateWeapon(float DeltaSeconds);
	virtual void UpdateWeapon_Implementation(float DeltaSeconds);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure) 
		FVector GetMuzzleLocation();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Meta = (Tooltip = "In Seconds."))
		float GetShootInterval();
	virtual FVector GetMuzzleLocation_Implementation();
	virtual float GetShootInterval_Implementation();
	
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void ShootProjectile(const bool bNoPlayFireSound = false, const bool bNoEjectShell = false);
	virtual void ShootProjectile_Implementation(const bool bNoPlayFireSound, const bool bNoEjectShell);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure) 
		FVector GetShellOutLocation();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetShellFrontLocation();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetShellDirLocation();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void EjectShell();
	virtual FVector GetShellOutLocation_Implementation();
	virtual FVector GetShellFrontLocation_Implementation();
	virtual FVector GetShellDirLocation_Implementation();
	virtual void EjectShell_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void PlayFireSound();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void PlayMagazineEjectSound();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void PlayMagazineInjectSound();
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void PlayPullClipSound();
	virtual void PlayFireSound_Implementation();
	virtual void PlayMagazineEjectSound_Implementation();
	virtual void PlayMagazineInjectSound_Implementation();
	virtual void PlayPullClipSound_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure) 
		bool NeedToPullReloadHandle();
	virtual bool NeedToPullReloadHandle_Implementation();

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure) 
		ECSWeaponType GetType();
	virtual ECSWeaponType GetType_Implementation();
	
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure) 
		bool CheckState(const ECSWeaponState Check);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		void CheckAllState(bool& OutIdle, bool& OutFiring, bool& OutReloading);
	virtual bool CheckState_Implementation(const ECSWeaponState Check);
	virtual void CheckAllState_Implementation(bool& OutIdle, bool& OutFiring, bool& OutReloading);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
		FVector GetLHandIKLocation();
	virtual FVector GetLHandIKLocation_Implementation();

	FORCEINLINE ACSCharacterBase* GetCharacterBase()
	{
		return CharacterBase;
	}
	FORCEINLINE int32 GetDamage()
	{
		return Damage;
	}
	FORCEINLINE int32 GetConsecutiveFire()
	{
		return ConsecutiveFire;
	}

protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly) ACSCharacterBase* CharacterBase;

	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FTransform InitialTransform;

	UPROPERTY(Category = "CS", BlueprintReadWrite) TEnumAsByte<ECSWeaponState> State;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) int32 LoadedAmmoCount;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) int32 MagazineAmmoCount;

	UPROPERTY(Category = "CS", BlueprintReadOnly) int32 ConsecutiveFire;

	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) USoundBase* FireSound;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) USoundBase* MagazineEjectSound;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) USoundBase* MagazineInjectSound;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) USoundBase* PullClipSound;

	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) int32 Damage;
	
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) TSubclassOf<ACSProjectileBase> ProjectileClass;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FName MuzzleSocketName;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) int32 ShootPerMin;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) float MuzzleSpeed;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) FRotator Inaccuracy;

	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) TSubclassOf<ACSShellBase> ShellClass;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) float ShellOutSpeed;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) FName ShellOutSocketName;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) FName ShellFrontSocketName;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) FName ShellDirSocketName;

	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FName LHandIKSocketName;
};
