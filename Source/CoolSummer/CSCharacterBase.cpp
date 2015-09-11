// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSWeaponBase.h"
#include "CSProjectileBase.h"
#include "CSShellBase.h"
#include "CSExplosive.h"
#include "CSCharacterBase.h"

const FCSMovementInfo ACSCharacterBase::MovementInfos[(int32)ECSMovementState::Count] =
{
	{ 0, 1000 }, { 100, 1000 }, { 200, 1000 }, { 200, 1000 }, { 450, 20 }, { 650, 5 }
};

FCSMovementInfo ACSCharacterBase::GetMovementInfo(const ECSMovementState State)
{
	check(ECSMovementState::Count > State);
	return MovementInfos[(int32)State];
}

// Sets default values
ACSCharacterBase::ACSCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxHitPoint = 100;
	HitPoint = MaxHitPoint;

	WeaponSocketName = TEXT("WeaponSocket");
	HeadSocketName = TEXT("HeadSocket");
	ChestSocketName = TEXT("ChestSocket");
	PelvisSocketName = TEXT("PelvisSocket");
	
	LeftFootSocketName = TEXT("LFootSocket");
	RightFootSocketName = TEXT("RFootSocket");

	ResetAnimNamesWhenShooted.Add(TEXT("RifleZoomFire"));
	ResetAnimNamesWhenShooted.Add(TEXT("RifleHipFire"));
	ResetAnimNamesWhenShooted.Add(TEXT("RifleHipFireFull"));
	ResetAnimNamesWhenShooted.Add(TEXT("RifleCrouchFire"));
	ResetAnimNamesWhenShooted.Add(TEXT("RifleCrouchZoomFire"));
	ResetAnimNamesWhenShooted.Add(TEXT("RifleCrouchHipFireFull"));

	BodyPartNamesHead.Add(TEXT("head"));

	BodyPartNamesUpperBody.Add(TEXT("spine_01"));
	BodyPartNamesUpperBody.Add(TEXT("spine_02"));
	BodyPartNamesUpperBody.Add(TEXT("spine_03"));
	BodyPartNamesUpperBody.Add(TEXT("clavicle_r"));
	BodyPartNamesUpperBody.Add(TEXT("clavicle_l"));
}

void ACSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if (nullptr != (UClass*)StartWeaponClass)
	{
		MainWeapon = NewObject<UCSWeaponBase>(this, (UClass*)StartWeaponClass);
		if (nullptr != MainWeapon)
		{
			MainWeapon->RegisterComponent();
			MainWeapon->AttachTo(GetMesh(), WeaponSocketName, EAttachLocation::SnapToTarget, true);
			MainWeapon->Initialize(this);
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("Failed to Create Start Weapon!"));
		}
	}
}
void ACSCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateState(DeltaTime);
	UpdateMovementState(DeltaTime);
	UpdateCameraState(DeltaTime);
	UpdateIK(DeltaTime);
}

void ACSCharacterBase::AcceptAnimPlayer(FAnimNode_CSAnimPlayer* Node)
{
	AnimPlayerNodes.Add(Node->NodeName, Node);
}
bool ACSCharacterBase::ContainsAnimPlayer(const FName NodeName)
{
	return AnimPlayerNodes.Contains(NodeName);
}
FAnimNode_CSAnimPlayer& ACSCharacterBase::GetAnimPlayer(const FName NodeName)
{
	return *AnimPlayerNodes[NodeName];
}
bool ACSCharacterBase::SetAnimPlayerCurrentTime(const FName NodeName, const float Seconds)
{
	FAnimNode_CSAnimPlayer** Found = AnimPlayerNodes.Find(NodeName);
	if (nullptr != Found)
	{
		(*Found)->SetCurrentTime(Seconds);
		return true;
	}
	return false;
}

void ACSCharacterBase::UpdateState_Implementation(float DeltaSeconds)
{
	bJumping = CheckJumping();
	bFalling = CheckFalling();
	if (bFalling)
	{
		FallingTime += DeltaSeconds;
	}
}
void ACSCharacterBase::UpdateMovementState_Implementation(float DeltaSeconds)
{
}
void ACSCharacterBase::UpdateCameraState_Implementation(float DeltaSeconds)
{
}
void ACSCharacterBase::UpdateIK_Implementation(float DeltaSeconds)
{
}

FRotator ACSCharacterBase::CalcShootNoise_Implementation(UCSWeaponBase* Weapon)
{
	const float Rand = Weapon->GetConsecutiveFire() / 5;
	return FRotator(FMath::FRandRange(-Rand, Rand), FMath::FRandRange(-Rand, Rand), FMath::FRandRange(-Rand, Rand));
}
FVector ACSCharacterBase::CalcShootDirection_Implementation(UCSWeaponBase* Weapon, const FVector& MuzzleLocation)
{
	UE_LOG(CSLog, Error, TEXT("ACSCharacterBase::CalcShootDirection() Not Implemented!"));
	return FVector::ForwardVector;
}

float ACSCharacterBase::GetDamageMultiplier_Implementation(const ECSBodyPart HitPart)
{
	switch (HitPart)
	{
	case ECSBodyPart::Head:
		return 3;
	case ECSBodyPart::UpperBody:
		return 1.5;
	}
	return 1;
}
int32 ACSCharacterBase::ReceiveDamage_Implementation(const FCSDamageInfo& DamageInfo)
{
	if (!bInvincible && !bDead)
	{
		const int32 FinalDamage = FMath::RoundToInt((float)DamageInfo.Damage * GetDamageMultiplier(DamageInfo.HitPart.GetValue()));
		const int32 ActualDamage = FMath::Clamp(FinalDamage, 0, HitPoint);
		HitPoint -= ActualDamage;
		if (0 >= HitPoint)
		{
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetMesh()->SetSimulatePhysics(true);
			if (!DamageInfo.WorldDirection.IsZero())
			{
				const FVector Impulse = DamageInfo.WorldDirection * 50000;
				if (DamageInfo.WorldLocation.IsZero())
				{
					GetMesh()->AddImpulse(Impulse);
				}
				else
				{
					GetMesh()->AddImpulseAtLocation(Impulse, DamageInfo.WorldLocation);
				}
			}

			if (!bDead)
			{
				bDead = true;
				Dead(DamageInfo);
				if (nullptr != DamageInfo.Instigator)
				{
					DamageInfo.Instigator->Killed(this, DamageInfo);
				}
			}
		}
		return ActualDamage;
	}
	return 0;
}

void ACSCharacterBase::Dead_Implementation(const FCSDamageInfo& FinalBlow)
{
}
void ACSCharacterBase::Killed_Implementation(ACSCharacterBase* DeadByMe, const FCSDamageInfo& FinalBlow)
{
}

void ACSCharacterBase::TakeHit_Implementation(ACSProjectileBase* HitProjectile, const FVector& Location, const FVector& Direction, const ECSBodyPart HitPart)
{
	ReceiveDamage(FCSDamageInfo(HitProjectile->GetWeaponBase()->GetCharacterBase(), HitProjectile->GetWeaponBase()->GetDamage(), Location, Direction, HitPart));
}
void ACSCharacterBase::TakeExplosion_Implementation(ACSExplosive* Exploded, ACSCharacterBase* InInstigator, const FVector& Direction, const int32 Damage, const float Alpha)
{
	ReceiveDamage(FCSDamageInfo(InInstigator, Damage, FVector::ZeroVector, Direction, ECSBodyPart::Other));
}

void ACSCharacterBase::OnProjectileShooted_Implementation(UCSWeaponBase* Weapon, ACSProjectileBase* Projectile, const FVector& Location, const FVector& Direction, const FRotator& Inaccuracy, const FRotator& Noise, const float Speed, const bool bSoundPlayed, const bool bShellEjected)
{
	for (const FName N : ResetAnimNamesWhenShooted)
	{
		SetAnimPlayerCurrentTime(N, 0);
	}
}

void ACSCharacterBase::OnShellEjected_Implementation(UCSWeaponBase* Weapon, ACSShellBase* Shell, const FVector& Velocity)
{
}

FVector ACSCharacterBase::GetHeadLocation_Implementation()
{
	return GetMesh()->GetSocketLocation(HeadSocketName);
}
FVector ACSCharacterBase::GetChestLocation_Implementation()
{
	return GetMesh()->GetSocketLocation(ChestSocketName);
}
FVector ACSCharacterBase::GetPelvisLocation_Implementation()
{
	return GetMesh()->GetSocketLocation(PelvisSocketName);
}

FVector ACSCharacterBase::GetLeftFootLocation_Implementation()
{
	return GetMesh()->GetSocketLocation(LeftFootSocketName);
}
FVector ACSCharacterBase::GetRightFootLocation_Implementation()
{
	return GetMesh()->GetSocketLocation(RightFootSocketName);
}

void ACSCharacterBase::GetMovementAnimInfo_Implementation(float& OutMoveAnimRatio, float& OutRealSpeed, FVector& OutRealDirLocal, FVector& OutRealDirWorld)
{
	const FVector Velocity = GetVelocity();
	OutRealSpeed = Velocity.Size();
	OutRealDirWorld = FVector::ForwardVector;
	if (0 != OutRealSpeed)
	{
		OutRealDirWorld = Velocity / OutRealSpeed;
	}
	OutRealDirLocal = GetActorRotation().UnrotateVector(OutRealDirWorld);
}

FRotator ACSCharacterBase::GetAimRotationLocal_Implementation()
{
	FRotator Result = GetControlRotation();
	Result.Yaw -= GetActorRotation().Yaw;
	Result.Pitch = Result.Pitch > 180 ? Result.Pitch - 360 : Result.Pitch;
	Result.Yaw = Result.Yaw > 180 ? Result.Yaw - 360 : Result.Yaw;
	Result.Roll = Result.Roll > 180 ? Result.Roll - 360 : Result.Roll;
	return Result;
}

void ACSCharacterBase::AddLocalMovement_Implementation(const FVector& LocalDirection, const float DesiredSpeed, FVector& OutWorldDirection)
{
	FRotator Rot = GetActorRotation();
	OutWorldDirection = Rot.RotateVector(LocalDirection);
	AddWorldMovement(OutWorldDirection, DesiredSpeed);
}
void ACSCharacterBase::AddWorldMovement_Implementation(const FVector& WorldDirection, const float DesiredSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = DesiredSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = DesiredSpeed;
	GetCharacterMovement()->AddInputVector(WorldDirection);
}

bool ACSCharacterBase::CheckMovementState_Implementation(const ECSMovementState Check)
{
	return MovementState.GetValue() == Check;
}

void ACSCharacterBase::CheckAllMovementState_Implementation(bool& OutIdle, bool& OutProne, bool& OutCrouch, bool& OutWalk, bool& OutRun, bool& OutSprint)
{
	const ECSMovementState Check = MovementState.GetValue();
	OutIdle = ECSMovementState::Idle == Check;
	OutProne = ECSMovementState::Prone == Check;
	OutCrouch = ECSMovementState::Crouch == Check;
	OutWalk = ECSMovementState::Walk == Check;
	OutRun = ECSMovementState::Run == Check;
	OutSprint = ECSMovementState::Sprint == Check;
}

bool ACSCharacterBase::CheckJumping_Implementation(float Limit)
{
	return GetVelocity().Z > Limit;
}
bool ACSCharacterBase::CheckFalling_Implementation(float Limit)
{
	return GetVelocity().Z < Limit;
}

ECSBodyPart ACSCharacterBase::CheckBodyPart_Implementation(const FName BoneName)
{
	if (BodyPartNamesHead.Contains(BoneName))
	{
		return ECSBodyPart::Head;
	}
	if (BodyPartNamesUpperBody.Contains(BoneName))
	{
		return ECSBodyPart::UpperBody;
	}
	return ECSBodyPart::Other;
}

bool ACSCharacterBase::HasAnyAmmoFor_Implementation(UCSWeaponBase* Weapon)
{
	return true;
}

int32 ACSCharacterBase::ConsumeAmmoFor_Implementation(UCSWeaponBase* Weapon, const int32 WantToConsume)
{
	return WantToConsume;
}


