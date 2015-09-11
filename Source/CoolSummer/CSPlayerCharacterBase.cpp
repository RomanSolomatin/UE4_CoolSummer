// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSWeaponBase.h"
#include "CSProjectileBase.h"
#include "CSPlayerControllerBase.h"
#include "CSAICharacterBase.h"
#include "CSObjectiveMarker.h"
#include "CSExplosive.h"
#include "CSPlayerCharacterBase.h"

const float GenerateNoiseTraceRadius = 10;
const float GenerateNoiseAttenuation = 3;
const float GenerateNoiseStealthAdvantage = 0.5;
const float GetDamageMultiplierDefensiveAdvantage = 0.5;
const float CalcShootNoiseOffensiveAdvantage = 0.5;
const float CalcShootNoiseZoomAdvantage = 0.5;
const int32 BeginPlayOffensiveAmmoAdvantage = 30;

ACSPlayerCharacterBase::ACSPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxTemperature = 500;

	bFindCameraComponentWhenViewTarget = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	LastMoveSpeed = 20;

	MaxAmmoCount = 150;
	AmmoCount = MaxAmmoCount;
}

void ACSPlayerCharacterBase::PossessedBy(AController* InController)
{
	Super::PossessedBy(InController);
	PCBase = Cast<ACSPlayerControllerBase>(InController);
	if (nullptr == PCBase)
	{
		UE_LOG(CSLog, Warning, TEXT("Failed to find CSPCBase!"));
	}
}

void ACSPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if (nullptr != PCBase)
	{
		FActorSpawnParameters Params;
		Params.bNoCollisionFail = true;
		Camera = GetWorld()->SpawnActor<ACSCameraActor>(Params);
		if (nullptr != Camera)
		{
			Camera->Initialize(this);
			PCBase->SetViewTarget(Camera);
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("Failed to Spawn Camera!"));
		}
	}
	else
	{
		UE_LOG(CSLog, Warning, TEXT("PCBase is missing in PlayerCharacter!"));
	}

	// Set Level Status, HP & Temperature
	UCSGameInstance* GI = UCSGameInstance::Get(GetWorld());
	if (nullptr != GI)
	{
		const int32 PlayerLevel = GI->GetPlayerLevel();
		HitPoint = MaxHitPoint = UCSGameInstance::GetMaxHP(PlayerLevel);
		MaxTemperature = UCSGameInstance::GetMaxTemperature(PlayerLevel);

		// Give more Ammo for Offensive Ability
		if (GI->GetPlayerAbility() == ECSPlayerAbility::Offensive)
		{
			AmmoCount = MaxAmmoCount += BeginPlayOffensiveAmmoAdvantage;
		}
	}
	else
	{
		UE_LOG(CSLog, Warning, TEXT("Cannot get CS Game Instance!"));
	}

	// Find the first Objective
	for (TActorIterator<ACSObjectiveMarker> I(GetWorld()); I; ++I)
	{
		if (I->GetID() == 0)
		{
			CurrentObjective = *I;
			break;
		}
	}
	if (nullptr != CurrentObjective)
	{
		CurrentObjective->Start(this);
	}
	else
	{
		UE_LOG(CSLog, Warning, TEXT("Cannot find the first Objective!"));
	}
}

// Called every frame
void ACSPlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (nullptr != Camera)
	{
		Camera->UpdateCamera(DeltaTime, GetActorLocation(), GetControlRotation());
	}
	else
	{
		UE_LOG(CSLog, Warning, TEXT("Cannot Update Camera!"));
	}
	if (nullptr != MainWeapon)
	{
		MainWeapon->UpdateWeapon(DeltaTime);
	}
}

void ACSPlayerCharacterBase::UpdateState_Implementation(float DeltaSeconds)
{
	Super::UpdateState_Implementation(DeltaSeconds);
	CrouchInputDelay += DeltaSeconds;
	const bool bHasMainWeapon = nullptr != MainWeapon;

	// Reloading
	if (!bDead && bHasMainWeapon)
	{
		const bool bWeaponReloading = MainWeapon->CheckState(ECSWeaponState::Reloading);
		if (bReloading)
		{
			if (bWeaponReloading)
			{
				if (bJumping || bFalling || CheckMovementState(ECSMovementState::Sprint))
				{
					if (MainWeapon->StopReload())
					{
						bReloading = false;
					}
				}
			}
			else
			{
				bReloading = false;
			}
		}
		else
		{
			if (bWeaponReloading)
			{
				bReloading = true;
				bReloadWhole = MainWeapon->NeedToPullReloadHandle();
			}
			else
			{
				if (bWantToReload && !bJumping && !bFalling && !bFiring && !CheckMovementState(ECSMovementState::Sprint))
				{
					if (MainWeapon->StartReload())
					{
						bReloading = true;
						bReloadWhole = MainWeapon->NeedToPullReloadHandle();
					}
				}
			}
		}
	}
	else
	{
		bReloading = false;
	}

	// Zooming
	if (bZooming)
	{
		if (!bWantToZoom || bDead || bJumping || bFalling || bReloading)
		{
			bZooming = false;
		}
	}
	else
	{
		if (bWantToZoom && !bDead && !bJumping && !bFalling && !bReloading)
		{
			bZooming = true;
		}
	}

	// Jumping
	if (!bJumping)
	{
		if (bWantToJump && !bDead && CanJump() && !bFalling && !CheckMovementState(ECSMovementState::Sprint) && !bReloading)
		{
			Jump();
			bJumping = true;
		}
	}

	// Firing
	if (bHasMainWeapon)
	{
		if (bFiring)
		{
			if (bDead || !MainWeapon->CheckState(ECSWeaponState::Firing))
			{
				bFiring = false;
			}
			else if (!bWantToFire || bDead || CheckMovementState(ECSMovementState::Sprint) || bJumping || bFalling || bReloading)
			{
				if (MainWeapon->StopFire())
				{
					bFiring = false;
				}
			}
		}
		else
		{
			if (!bDead && MainWeapon->CheckState(ECSWeaponState::Firing))
			{
				bFiring = true;
			}
			else if (bWantToFire && !bDead && !CheckMovementState(ECSMovementState::Sprint) && !bJumping && !bFalling && !bReloading)
			{
				if (MainWeapon->StartFire())
				{
					bFiring = true;
				}
			}
		}
	}
	else
	{
		bFiring = false;
	}
}
void ACSPlayerCharacterBase::UpdateMovementState_Implementation(float DeltaSeconds)
{
	Super::UpdateMovementState_Implementation(DeltaSeconds);
	if (!bJumping && !bFalling && !bDead)
	{
		if (nullptr != PCBase)
		{
			bool bForward, bBackward, bLeft, bRight;
			FVector DesireDirection = PCBase->CalcLocalMovementDirection(bForward, bBackward, bLeft, bRight);

			bool bDetermined = false;
			// Sprint
			if (!bDetermined)
			{
				if (CheckMovementState(ECSMovementState::Sprint))
				{
					if (bWantToSprint && bForward)
					{
						DesireDirection.Y = 0;
						SetMovementState(ECSMovementState::Sprint);
						bDetermined = true;
					}
				}
				else
				{
					if (bWantToSprint && bForward && !bZooming && !bFiring)
					{
						DesireDirection.Y = 0;
						SetMovementState(ECSMovementState::Sprint);
						bDetermined = true;
					}
				}
			}

			// Crouch
			if (!bDetermined)
			{
				const float InputLimit = 1.0f;
				if (CheckMovementState(ECSMovementState::Crouch))
				{
					if (bWantToCrouch && CrouchInputDelay >= InputLimit)
					{
						SetMovementState(ECSMovementState::Idle);
						CrouchInputDelay = 0;
					}
					else
					{
						SetMovementState(ECSMovementState::Crouch);
						bDetermined = true;
					}
				}
				else
				{
					if (bWantToCrouch && CrouchInputDelay >= InputLimit)
					{
						SetMovementState(ECSMovementState::Crouch);
						CrouchInputDelay = 0;
						bDetermined = true;
					}
				}
			}

			// Walk
			if (!bDetermined)
			{
				if (bZooming)
				{
					SetMovementState(ECSMovementState::Walk);
					bDetermined = true;
				}
			}

			// Run or Idle
			if (!bDetermined)
			{
				if (DesireDirection.IsZero())
				{
					SetMovementState(ECSMovementState::Idle);
				}
				else
				{
					SetMovementState(ECSMovementState::Run);
				}
				bDetermined = true;
			}

			// Sprint Slide Anim
			if (bInSlideAnim)
			{
				DesireMoveSpeed = UpdateSlideAnim(DeltaSeconds);
				DesireDirection.X = 1;
				DesireDirection.Y = 0;
			}

			// Final Stage
			DesireDirection.Normalize();
			DesireMoveDirectionLocal = DesireDirection;
			AddLocalMovement(DesireMoveDirectionLocal, DesireMoveSpeed, DesireMoveDirectionWorld);
			if (CheckMovementState(ECSMovementState::Crouch))
			{
				Crouch();
			}
			else
			{
				UnCrouch();
			}
		}
	}
}
void ACSPlayerCharacterBase::UpdateCameraState_Implementation(float DeltaSeconds)
{
	Super::UpdateCameraState_Implementation(DeltaSeconds);
	// Apply Control Rotation to Mesh Rotation
	bool bNeedRotateMesh = false;
	if ((CheckMovementState(ECSMovementState::Idle) || CheckMovementState(ECSMovementState::Crouch)) 
		&& bFiring)
	{
		bNeedRotateMesh = true;
	}

	switch (MovementState)
	{
	case ECSMovementState::Crouch:
		if (DesireMoveDirectionLocal.IsZero())
		{
			if (bZooming)
			{
				bNeedRotateMesh = true;
			}
		}
		else
		{
			bNeedRotateMesh = true;
		}
		break;
	case ECSMovementState::Idle:
	case ECSMovementState::Walk:
		if (bZooming)
		{
			bNeedRotateMesh = true;
		}
		break;
	default:
		bNeedRotateMesh = true;
		break;
	}

	if (bNeedRotateMesh)
	{
		const FRotator Current = GetActorRotation();
		FRotator Target = GetControlRotation();
		Target.Pitch = Current.Pitch;
		Target.Roll = Current.Roll;
		const FRotator Result = FMath::RInterpTo(Current, Target, DeltaSeconds, RotateSpeed);
		SetActorRotation(Result);
	}


	if (nullptr != Camera)
	{
		bool bDetermined = false;
		// Sprint
		if (!bDetermined)
		{
			if (CheckMovementState(ECSMovementState::Sprint))
			{
				Camera->SetZoomState(ECSZoomState::Sprint);
				bDetermined = true;
			}
		}
		// Zoom
		if (!bDetermined)
		{
			if (bZooming)
			{
				// @TODO By Weapon Type.
				Camera->SetZoomState(ECSZoomState::IronSight);
				bDetermined = true;
			}
		}
		// Final Stage
		if (!bDetermined)
		{
			Camera->SetZoomState(ECSZoomState::Idle);
		}

		bDetermined = false;
		// Crouch
		if (!bDetermined)
		{
			if (CheckMovementState(ECSMovementState::Crouch))
			{
				Camera->SetHeightState(ECSHeightState::Crouch);
				bDetermined = true;
			}
		}
		// Prone
		if (!bDetermined)
		{
			if (CheckMovementState(ECSMovementState::Prone))
			{
				Camera->SetHeightState(ECSHeightState::Prone);
				bDetermined = true;
			}
		}
		// Final Stage
		if (!bDetermined)
		{
			Camera->SetHeightState(ECSHeightState::Stand);
		}
	}
	else
	{
		UE_LOG(CSLog, Warning, TEXT("Camera is missing!"));
	}
}
void ACSPlayerCharacterBase::UpdateIK_Implementation(float DeltaSeconds)
{
	Super::UpdateIK_Implementation(DeltaSeconds);
	if (nullptr != MainWeapon)
	{
		LHandIKLocation = MainWeapon->GetLHandIKLocation();
	}
}

FRotator ACSPlayerCharacterBase::CalcShootNoise_Implementation(UCSWeaponBase* Weapon)
{
	FRotator Noise = Super::CalcShootNoise_Implementation(Weapon);
	if (UCSGameInstance::CheckPlayerAbility(ECSPlayerAbility::Offensive, GetWorld()))
	{
		Noise *= CalcShootNoiseOffensiveAdvantage;
	}
	if (bZooming)
	{
		Noise *= CalcShootNoiseZoomAdvantage;
	}
	return Noise;
}
FVector ACSPlayerCharacterBase::CalcShootDirection_Implementation(UCSWeaponBase* Weapon, const FVector& MuzzleLocation)
{
	UWorld* World = GetWorld();
	if (nullptr != World)
	{
		if (nullptr != Camera)
		{
			// @TODO Need to Think about Zero Point.
			const float RayLength = 5000;
			const FVector CameraLocation = Camera->GetCameraWorldLocation();
			const FRotator CameraRotation = GetControlRotation();
			const FVector RayDirection = CameraRotation.RotateVector(FVector::ForwardVector);
			const FVector RayEnd = CameraLocation + (RayDirection * RayLength);
			FVector TargetLocation;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			Params.AddIgnoredActor(Camera);
			FHitResult Result;
			if (World->LineTraceSingle(Result, CameraLocation, RayEnd, ECollisionChannel::ECC_Projectile, Params))
			{
				TargetLocation = Result.Location;
			}
			else
			{
				TargetLocation = RayEnd;
			}
			TargetLocation -= MuzzleLocation;
			TargetLocation.Normalize();
			return TargetLocation;
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("Failed to Calc Shoot Direction! Camera is not exists!"));
			return FVector::ForwardVector;
		}
	}
	else
	{
		UE_LOG(CSLog, Warning, TEXT("Failed to Calc Shoot Direction! World is not exists!"));
		return FVector::ForwardVector;
	}
}

float ACSPlayerCharacterBase::GetDamageMultiplier_Implementation(const ECSBodyPart HitPart)
{
	float Result = Super::GetDamageMultiplier_Implementation(HitPart);
	switch (HitPart)
	{
	case ECSBodyPart::Head:
	case ECSBodyPart::UpperBody:
		Result -= GetDamageMultiplierDefensiveAdvantage;
		break;
	}

	const int32 HalfMaxTemperature = MaxTemperature / 2;
	if (Temperature <= HalfMaxTemperature)
	{
		Result += float(Temperature) / float(HalfMaxTemperature);
	}
	else
	{
		Result += FMath::Square(float(Temperature) / float(HalfMaxTemperature));
	}
	return Result;
}

void ACSPlayerCharacterBase::TakeHit_Implementation(ACSProjectileBase* HitProjectile, const FVector& Location, const FVector& Direction, const ECSBodyPart HitPart)
{
	Super::TakeHit_Implementation(HitProjectile, Location, Direction, HitPart);
	ACSAICharacterBase* AICharacter = Cast<ACSAICharacterBase>(HitProjectile->Instigator);
	if (nullptr != AICharacter)
	{
		AddTemperature(AICharacter->GetTemperaturePerAttack());
	}
}
void ACSPlayerCharacterBase::TakeExplosion_Implementation(ACSExplosive* Exploded, ACSCharacterBase* InInstigator, const FVector& Direction, const int32 Damage, const float Alpha)
{
	Super::TakeExplosion_Implementation(Exploded, InInstigator, Direction, Damage, Alpha);
	const int32 Temperature = FMath::RoundToInt(FMath::Lerp((float)Exploded->GetCenterTemperature(), (float)Exploded->GetBorderTemperature(), Alpha));
	AddTemperature(Temperature);
}

void ACSPlayerCharacterBase::GetMovementAnimInfo_Implementation(float& OutMoveAnimRatio, float& OutRealSpeed, FVector& OutRealDirLocal, FVector& OutRealDirWorld)
{
	Super::GetMovementAnimInfo_Implementation(OutMoveAnimRatio, OutRealSpeed, OutRealDirLocal, OutRealDirWorld);
	OutMoveAnimRatio = 0 != LastMoveSpeed ? OutRealSpeed / LastMoveSpeed : 0;
	switch (MovementState)
	{
	case ECSMovementState::Walk:
		OutMoveAnimRatio /= 2;
		break;
	default:
		break;
	}
}

void ACSPlayerCharacterBase::GenerateNoise_Implementation(const FVector& Location, const float Radius)
{
	float ApplyRadius = UCSGameInstance::CheckPlayerAbility(ECSPlayerAbility::Stealth, GetWorld()) ? Radius * GenerateNoiseStealthAdvantage : Radius;
	for (TActorIterator<ACSAICharacterBase> I(GetWorld()); I; ++I)
	{
		const FVector OtherLocation = I->GetHeadLocation();
		const float DistanceSq = (OtherLocation - Location).SizeSquared();
		if (DistanceSq <= FMath::Square(ApplyRadius))
		{
			FCollisionShape Shape = FCollisionShape::MakeSphere(GenerateNoiseTraceRadius);
			FCollisionQueryParams QueryParams;
			for (TActorIterator<ACSPlayerCharacterBase> J(GetWorld()); J; ++J)
			{
				QueryParams.AddIgnoredActor(*J);
			}
			QueryParams.AddIgnoredActor(this);
			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Destructible);
			FHitResult HitResult;
			if (GetWorld()->SweepSingle(HitResult, Location, OtherLocation, FQuat::Identity, Shape, QueryParams, ObjectQueryParams))
			{
				if (*I == HitResult.Actor)
				{
					I->HearNoise(this);
				}
				else if (DistanceSq <= FMath::Square(ApplyRadius / GenerateNoiseAttenuation))
				{
					I->HearNoise(this);
				}
			}
			else
			{
				I->HearNoise(this);
			}
		}
	}
}

bool ACSPlayerCharacterBase::HasAnyAmmoFor_Implementation(UCSWeaponBase* Weapon)
{
	return 0 < AmmoCount;
}
int32 ACSPlayerCharacterBase::ConsumeAmmoFor_Implementation(UCSWeaponBase* Weapon, const int32 WantToConsume)
{
	const int32 ActualConsumed = FMath::Clamp(WantToConsume, 0, AmmoCount);
	AmmoCount -= ActualConsumed;
	return ActualConsumed;
}

float ACSPlayerCharacterBase::SetMovementState_Implementation(const ECSMovementState NewState)
{
	MovementState = NewState;
	const FCSMovementInfo& Info = MovementInfos[(int32)NewState];
	DesireMoveSpeed = Info.MoveSpeed;
	if (0 != DesireMoveSpeed)
	{
		LastMoveSpeed = DesireMoveSpeed;
	}
	RotateSpeed = Info.RotateSpeed;
	return DesireMoveSpeed;
}

void ACSPlayerCharacterBase::StartSlideAnim_Implementation(float Duration)
{
	check(0 < Duration);
	bInSlideAnim = true;
	SlideAnimDuration = Duration;
	SlideAnimTime = 0;
}
float ACSPlayerCharacterBase::UpdateSlideAnim_Implementation(float DeltaTime)
{
	if (bInSlideAnim)
	{
		if (nullptr != SlideAnimCurve)
		{
			SlideAnimTime += DeltaTime;
			if (SlideAnimDuration <= SlideAnimTime)
			{
				bInSlideAnim = false;
			}
			else
			{
				const float Ratio = SlideAnimTime / SlideAnimDuration;
				const float CurveValue = SlideAnimCurve->GetFloatValue(Ratio);
				return CurveValue * MovementInfos[(int32)ECSMovementState::Sprint].MoveSpeed;
			}
		}
		else
		{
			UE_LOG(CSLog, Error, TEXT("SlideAnimCurve is Missing!"));
			bInSlideAnim = false;
		}
	}
	return 0;
}
void ACSPlayerCharacterBase::StopSlideAnim_Implementation()
{
	bInSlideAnim = false;
}

void ACSPlayerCharacterBase::ChangeObjective_Implementation(ACSObjectiveMarker* NewObjective)
{
	check(nullptr != NewObjective);
	if (CurrentObjective != NewObjective)
	{
		CurrentObjective = NewObjective;
	}
}
