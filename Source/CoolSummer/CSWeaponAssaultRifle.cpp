// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSWeaponBase.h"
#include "CSCharacterBase.h"
#include "CSWeaponAssaultRifle.h"

UCSWeaponAssaultRifle::UCSWeaponAssaultRifle(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	ShootTimeRemain = 0;
	FirstShootSensitive = 0.03;

	ReloadTimeRemain = 0;
	AmmoLoadTimeRemain = 0;
}

bool UCSWeaponAssaultRifle::StartFire_Implementation()
{
	switch (State)
	{
	case ECSWeaponState::Firing: return true;
	case ECSWeaponState::Reloading: return false;
	default:
		if (0 < LoadedAmmoCount)
		{
			ShootTimeRemain = FirstShootSensitive;
			State = ECSWeaponState::Firing;
			ConsecutiveFire = 0;
			return true;
		}
	}
	return false;
}
bool UCSWeaponAssaultRifle::StopFire_Implementation()
{
	if (ECSWeaponState::Firing == State)
	{
		State = ECSWeaponState::Idle;
		return true;
	}
	return false;
}

bool UCSWeaponAssaultRifle::StartReload_Implementation()
{
	switch (State)
	{
	case ECSWeaponState::Firing: return false;
	case ECSWeaponState::Reloading: return true;
	default:
		if (LoadedAmmoCount < MagazineAmmoCount)
		{
			if (CharacterBase->HasAnyAmmoFor(this))
			{
				if (NeedToPullReloadHandle())
				{
					ReloadTimeRemain = WholeReloadTime;
					AmmoLoadTimeRemain = WholeAmmoLoadTime;
				}
				else
				{
					ReloadTimeRemain = FastReloadTime;
					AmmoLoadTimeRemain = FastAmmoLoadTime;
				}
				State = ECSWeaponState::Reloading;
				return true;
			}
		}
	}
	return false;
}
bool UCSWeaponAssaultRifle::StopReload_Implementation()
{
	if (ECSWeaponState::Reloading == State)
	{
		State = ECSWeaponState::Idle;
		return true;
	}
	return false;
}

void UCSWeaponAssaultRifle::UpdateWeapon_Implementation(float DeltaSeconds)
{
	Super::UpdateWeapon_Implementation(DeltaSeconds);
	switch (State)
	{
	case ECSWeaponState::Firing:
	{
		ShootTimeRemain -= DeltaSeconds;
		if (0 < LoadedAmmoCount)
		{
			if (0 >= ShootTimeRemain)
			{
				ShootProjectile();
				LoadedAmmoCount--;
				ShootTimeRemain += GetShootInterval();
				ConsecutiveFire++;
			}
		}
		else
		{
			State = ECSWeaponState::Idle;
		}
		break;
	}
	case ECSWeaponState::Reloading:
	{
		ReloadTimeRemain -= DeltaSeconds;
		AmmoLoadTimeRemain -= DeltaSeconds;
		if (0 >= AmmoLoadTimeRemain)
		{
			LoadedAmmoCount += CharacterBase->ConsumeAmmoFor(this, MagazineAmmoCount - LoadedAmmoCount);
			AmmoLoadTimeRemain = FLT_MAX;
		}
		if (0 >= ReloadTimeRemain)
		{
			State = ECSWeaponState::Idle;
		}
		break;
	}
	}
}

ECSWeaponType UCSWeaponAssaultRifle::GetType_Implementation()
{
	return ECSWeaponType::AssaultRifle;
}

