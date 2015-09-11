// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSCharacterBase.h"
#include "CSProjectileBase.h"
#include "CSShellBase.h"
#include "CSWeaponBase.h"

UCSWeaponBase::UCSWeaponBase(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	State = ECSWeaponState::Idle;

	Damage = 10;

	MuzzleSocketName = TEXT("Muzzle");
	ShootPerMin = 100;
	MuzzleSpeed = 10;
	Inaccuracy = FRotator(1, 1, 1);

	ShellOutSpeed = 100;
	ShellOutSocketName = TEXT("ShellOut");
	ShellFrontSocketName = TEXT("ShellFront");
	ShellDirSocketName = TEXT("ShellDir");

	LHandIKSocketName = TEXT("LHandIKSocket");
}

void UCSWeaponBase::Initialize_Implementation(ACSCharacterBase* InCharacter)
{
	check(nullptr != InCharacter);
	CharacterBase = InCharacter;
	SetRelativeTransform(InitialTransform);
}

bool UCSWeaponBase::StartFire_Implementation()
{
	UE_LOG(CSLog, Error, TEXT("UCSWeaponBase::StartFire_Implementation() Not Implemented!"));
	return false;
}
bool UCSWeaponBase::StopFire_Implementation()
{
	UE_LOG(CSLog, Error, TEXT("UCSWeaponBase::StopFire_Implementation() Not Implemented!"));
	return false;
}

bool UCSWeaponBase::StartReload_Implementation()
{
	UE_LOG(CSLog, Error, TEXT("UCSWeaponBase::StartReload_Implementation() Not Implemented!"));
	return false;
}
bool UCSWeaponBase::StopReload_Implementation()
{
	UE_LOG(CSLog, Error, TEXT("UCSWeaponBase::StopReload_Implementation() Not Implemented!"));
	return false;
}

void UCSWeaponBase::UpdateWeapon_Implementation(float DeltaSeconds)
{
}

FVector UCSWeaponBase::GetMuzzleLocation_Implementation()
{
	return GetSocketLocation(MuzzleSocketName);
}
float UCSWeaponBase::GetShootInterval_Implementation()
{
	return float(60) / float(ShootPerMin);
}
void UCSWeaponBase::ShootProjectile_Implementation(const bool bNoPlayFireSound, const bool bNoEjectShell)
{
	if (nullptr != CharacterBase)
	{
		if ((UClass*)ProjectileClass != nullptr)
		{
			UWorld* World = GetWorld();
			if (nullptr != World)
			{
				const FVector MuzzleLocation = GetMuzzleLocation();
				const FVector SrcDirection = CharacterBase->CalcShootDirection(this, MuzzleLocation);
				const FRotator InaccuracyRand = FRotator(
					FMath::FRandRange(-Inaccuracy.Pitch, Inaccuracy.Pitch), 
					FMath::FRandRange(-Inaccuracy.Yaw, Inaccuracy.Yaw), 
					FMath::FRandRange(-Inaccuracy.Roll, Inaccuracy.Roll));
				const FRotator Noise = CharacterBase->CalcShootNoise(this);
				const FVector Direction = (InaccuracyRand + Noise).RotateVector(SrcDirection);
				FActorSpawnParameters Params;
				Params.bNoCollisionFail = true;
				Params.Instigator = CharacterBase;
				ACSProjectileBase* Projectile = World->SpawnActor<ACSProjectileBase>((UClass*)ProjectileClass, MuzzleLocation, FRotator::ZeroRotator, Params);
				if (nullptr != Projectile)
				{
					Projectile->Initialize(this);
					Projectile->Shoot(Direction, MuzzleSpeed);
					if (!bNoPlayFireSound)
					{
						PlayFireSound();
					}
					if (!bNoEjectShell)
					{
						EjectShell();
					}
					CharacterBase->OnProjectileShooted(this, Projectile, MuzzleLocation, Direction, Inaccuracy, Noise, MuzzleSpeed, !bNoPlayFireSound, !bNoEjectShell);
				}
				else
				{
					UE_LOG(CSLog, Warning, TEXT("Cannot Shoot! Actor Spawn Failed!"));
				}
			}
			else
			{
				UE_LOG(CSLog, Warning, TEXT("Cannot Shoot! World is not exists!"));
			}
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("Cannot Shoot! Projectile Class is not exists!"));
		}
	}
	else
	{
		UE_LOG(CSLog, Warning, TEXT("Cannot Shoot Projectile! Character is not exists!"));
	}
}

FVector UCSWeaponBase::GetShellOutLocation_Implementation()
{
	return GetSocketLocation(ShellOutSocketName);
}

FVector UCSWeaponBase::GetShellFrontLocation_Implementation()
{
	return GetSocketLocation(ShellFrontSocketName);
}

FVector UCSWeaponBase::GetShellDirLocation_Implementation()
{
	return GetSocketLocation(ShellDirSocketName);
}

void UCSWeaponBase::EjectShell_Implementation()
{
	if ((UClass*)ShellClass != nullptr)
	{
		if (nullptr != CharacterBase)
		{
			UWorld* World = GetWorld();
			if (nullptr != World)
			{
				const FVector Location = GetShellOutLocation();
				FVector Front = GetShellFrontLocation() - Location;
				Front.Normalize();
				FVector Direction = GetShellDirLocation() - Location;
				Direction.Normalize();

				FActorSpawnParameters Params;
				Params.bNoCollisionFail = true;
				Params.Instigator = CharacterBase;
				ACSShellBase* Shell = World->SpawnActor<ACSShellBase>((UClass*)ShellClass, Location, Front.Rotation(), Params);
				if (nullptr != Shell)
				{
					const FVector Noise(FMath::FRandRange(-0.2, 0.2), FMath::FRandRange(-0.2, 0.2), FMath::FRandRange(-0.2, 0.2));
					const FVector SrcV = CharacterBase->GetVelocity();
					const FVector DesireV = (Direction + Noise) * ShellOutSpeed;
					const FVector Velocity = SrcV + DesireV;
					Shell->Initailize(this);
					Shell->Drop(Velocity);
					CharacterBase->OnShellEjected(this, Shell, Velocity);
				}
				else
				{
					UE_LOG(CSLog, Warning, TEXT("Cannot Drop a Shell! Spawn Failed!"));
				}
			}
			else
			{
				UE_LOG(CSLog, Warning, TEXT("Cannot Drop a Shell! Class is not exists!"));
			}
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("Cannot Drop a Shell! Character is not exists!"));
		}
	}
}

void UCSWeaponBase::PlayFireSound_Implementation()
{
	UWorld* World = GetWorld();
	if (nullptr != World)
	{
		if (nullptr != FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(World, FireSound, GetComponentLocation());
		}
	}
}
void UCSWeaponBase::PlayMagazineEjectSound_Implementation()
{
	UWorld* World = GetWorld();
	if (nullptr != World)
	{
		if (nullptr != MagazineEjectSound)
		{
			UGameplayStatics::PlaySoundAtLocation(World, MagazineEjectSound, GetComponentLocation());
		}
	}
}
void UCSWeaponBase::PlayMagazineInjectSound_Implementation()
{
	UWorld* World = GetWorld();
	if (nullptr != World)
	{
		if (nullptr != MagazineInjectSound)
		{
			UGameplayStatics::PlaySoundAtLocation(World, MagazineInjectSound, GetComponentLocation());
		}
	}
}
void UCSWeaponBase::PlayPullClipSound_Implementation()
{
	UWorld* World = GetWorld();
	if (nullptr != World)
	{
		if (nullptr != PullClipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(World, PullClipSound, GetComponentLocation());
		}
	}
}

bool UCSWeaponBase::NeedToPullReloadHandle_Implementation()
{
	return LoadedAmmoCount <= 0;
}

ECSWeaponType UCSWeaponBase::GetType_Implementation()
{
	return ECSWeaponType::AssaultRifle;
}

bool UCSWeaponBase::CheckState_Implementation(const ECSWeaponState Check)
{
	return State.GetValue() == Check;
}

void UCSWeaponBase::CheckAllState_Implementation(bool& OutIdle, bool& OutFiring, bool& OutReloading)
{
	const register ECSWeaponState S = State.GetValue();
	OutIdle = ECSWeaponState::Idle == S;
	OutFiring = ECSWeaponState::Firing == S;
	OutReloading = ECSWeaponState::Reloading == S;
}

FVector UCSWeaponBase::GetLHandIKLocation_Implementation()
{
	return GetSocketLocation(LHandIKSocketName);
}
