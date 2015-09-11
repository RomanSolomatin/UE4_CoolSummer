// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "CSWeaponBase.h"
#include "CSWeaponAssaultRifle.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = "CS", Blueprintable, Abstract)
class COOLSUMMER_API UCSWeaponAssaultRifle : public UCSWeaponBase
{
	GENERATED_BODY()
	
public:
	UCSWeaponAssaultRifle(const FObjectInitializer& Initializer);

	virtual bool StartFire_Implementation() override;
	virtual bool StopFire_Implementation() override;

	virtual bool StartReload_Implementation() override;
	virtual bool StopReload_Implementation() override;

	virtual void UpdateWeapon_Implementation(float DeltaSeconds) override;

	virtual ECSWeaponType GetType_Implementation() override;
	
protected:
	float ShootTimeRemain;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) float FirstShootSensitive;

	float ReloadTimeRemain;
	float AmmoLoadTimeRemain;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere, Meta = (Tooltip = "The Duration of Reloading Motion. NOT Ammo Fill up Timing. Time Include for Pulling Reload Handle.")) float WholeReloadTime;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere, Meta = (Tooltip = "The Duration for When Ammo Fill up. NOT Motion Time. Time Include for Pulling Reload Handle.")) float WholeAmmoLoadTime;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere, Meta = (Tooltip = "The Duration of Reloading Motion. NOT Ammo Fill up Timing. Time without Pulling Reload Handle.")) float FastReloadTime;
	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere, Meta = (Tooltip = "The Duration for When Ammo Fill up. NOT Motion Time. Time without Pulling Reload Handle.")) float FastAmmoLoadTime;
};
