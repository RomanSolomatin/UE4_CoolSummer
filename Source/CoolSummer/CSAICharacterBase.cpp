// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSWeaponBase.h"
#include "CSProjectileBase.h"
#include "CSAICharacterBase.h"

const float IsInWatchSightTraceRadius = 5;
const float CanShootHimTraceRadius = 10;

ACSAICharacterBase::ACSAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WatchingDistance = 2000;
	WatchingDegrees = FVector(10, 30, 45);
	ShootableRange = 3000;
	KillExperience = 100;

	HomeRadius = 1000;

	bPatrolRoamPositive = true;
	PatrolWaitTimeInWaypoint = 5;
}

void ACSAICharacterBase::PossessedBy(AController* InController)
{
	Super::PossessedBy(InController);
	ACBase = Cast<ACSAIControllerBase>(InController);
	if (nullptr == ACBase)
	{
		UE_LOG(CSLog, Warning, TEXT("AI Controller is not CS Class!"));
	}
}
void ACSAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	HomeLocation = GetActorLocation();
	HomeRotation = GetActorRotation();
}
void ACSAICharacterBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (nullptr != MainWeapon)
	{
		MainWeapon->UpdateWeapon(DeltaTime);
	}
}

void ACSAICharacterBase::UpdateState_Implementation(float DeltaSeconds)
{
	Super::UpdateState_Implementation(DeltaSeconds);
	if (bDead)
	{
		if (nullptr != MainWeapon)
		{
			MainWeapon->StopFire();
			MainWeapon->StopReload();
		}

		DeadTimer += DeltaSeconds;
		if (5 <= DeadTimer)
		{
			Destroy();
		}
	}
	else
	{
		if (nullptr != ACBase)
		{
			UpdateTimer += DeltaSeconds;
			
			SearchTimer += DeltaSeconds;
			if (bNeedUpdatePatrolTimer)
			{
				PatrolTimer += DeltaSeconds;
			}

			if (0.2 <= UpdateTimer)
			{
				UpdateTimer = 0;

				switch (AIType.GetValue())
				{
				case ECSAIType::HoldPosition:
					UpdateHoldPositionState(DeltaSeconds);
					break;
				case ECSAIType::Patrol:
					if (2 <= PatrolWaypoints.Num())
					{
						UpdatePatrolState(DeltaSeconds);
					}
					else
					{
						UE_LOG(CSLog, Error, TEXT("Patrol: Need more Waypoints!"));
					}
					break;
				}
			}
		}

		if (nullptr != MainWeapon)
		{
			bFiring = MainWeapon->CheckState(ECSWeaponState::Firing);
			bReloading = MainWeapon->CheckState(ECSWeaponState::Reloading);
		}
	}
}
void ACSAICharacterBase::UpdateMovementState_Implementation(float DeltaSeconds)
{
	Super::UpdateMovementState_Implementation(DeltaSeconds);
}
void ACSAICharacterBase::UpdateCameraState_Implementation(float DeltaSeconds)
{
	Super::UpdateCameraState_Implementation(DeltaSeconds);
	if (nullptr != MainWeapon)
	{
		if (MainWeapon->CheckState(ECSWeaponState::Firing))
		{
			FRotator Rotation = GetActorRotation();
			Rotation.Yaw = GetControlRotation().Yaw;
			SetActorRotation(Rotation);
		}
	}
}
void ACSAICharacterBase::UpdateIK_Implementation(float DeltaSeconds)
{
	Super::UpdateIK_Implementation(DeltaSeconds);
}

void ACSAICharacterBase::UpdateHoldPositionState_Implementation(float DeltaSeconds)
{
	// Watch
	if (CheckHoldPositionState(ECSHoldPositionState::Watch))
	{
		SetActorRotation(HomeRotation);
		ACSPlayerCharacterBase* NewWatch = Watch();
		if (nullptr != NewWatch)
		{
			CurrentTarget = NewWatch;
			CallComrades(CurrentTarget);
			HoldPositionState = ECSHoldPositionState::Chase;
		}
	}

	// Chase
	if (CheckHoldPositionState(ECSHoldPositionState::Chase))
	{
		if (nullptr != CurrentTarget)
		{
			bool bInRange, bChest, bPelvis, bHead;
			FVector Muzzle, Chest, Pelvis, Head;
			if (CanShootHim(CurrentTarget, bInRange, bChest, bPelvis, bHead, Chest, Pelvis, Head, Muzzle))
			{
				ACBase->StopMovement();
				HoldPositionState = ECSHoldPositionState::Attack;
			}
			else
			{
				if (InHome())
				{
					ACBase->MoveToActor(CurrentTarget, -1, true, true, false);
				}
				else
				{
					ACBase->StopMovement();
					HoldPositionState = ECSHoldPositionState::Search;
					SearchTimer = 0;
					CurrentTarget = nullptr;
				}
			}
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("HoldPosition: AI is in Chase but Target is missing! Will Go Back Home!"));
			HoldPositionState = ECSHoldPositionState::BackHome;
			CurrentTarget = nullptr;
		}
	}

	// Back Home
	if (CheckHoldPositionState(ECSHoldPositionState::BackHome))
	{
		if (NearlyHome(100))
		{
			ACBase->StopMovement();
			HoldPositionState = ECSHoldPositionState::Watch;
			CurrentTarget = nullptr;
		}
		else
		{
			ACBase->MoveToLocation(HomeLocation, -1, true, true, false, false);
		}
	}

	// Search
	if (CheckHoldPositionState(ECSHoldPositionState::Search))
	{
		const float MaxSearchTime = 5;
		ACSPlayerCharacterBase* NewWatch = Watch();
		if (nullptr != NewWatch)
		{
			CallComrades(NewWatch);
			bool bInRange, bChest, bPelvis, bHead;
			FVector Muzzle, Chest, Pelvis, Head;
			if (CanShootHim(NewWatch, bInRange, bChest, bPelvis, bHead, Chest, Pelvis, Head, Muzzle))
			{
				CurrentTarget = NewWatch;
				HoldPositionState = ECSHoldPositionState::Attack;
			}
		}
		else
		{
			if (MaxSearchTime <= SearchTimer)
			{
				HoldPositionState = ECSHoldPositionState::BackHome;
				CurrentTarget = nullptr;
			}
		}
	}

	// Attack
	if (CheckHoldPositionState(ECSHoldPositionState::Attack))
	{
		if (nullptr != CurrentTarget)
		{
			bool bInRange, bChest, bPelvis, bHead;
			FVector Muzzle, Chest, Pelvis, Head;
			if (CanShootHim(CurrentTarget, bInRange, bChest, bPelvis, bHead, Chest, Pelvis, Head, Muzzle))
			{
				if (bChest)
				{
					CurrentShootLocation = Chest;
				}
				else if (bPelvis)
				{
					CurrentShootLocation = Pelvis;
				}
				else if (bHead)
				{
					CurrentShootLocation = Head;
				}
				else
				{
					UE_LOG(CSLog, Warning, TEXT("HoldPosition: AI cannot Determine Shoot Location! Will Go Back Home!"));
					HoldPositionState = ECSHoldPositionState::BackHome;
					CurrentTarget = nullptr;
				}

				if (nullptr != MainWeapon)
				{
					if (!MainWeapon->StartFire())
					{
						bReloadWhole = MainWeapon->NeedToPullReloadHandle();
						if (!MainWeapon->StartReload())
						{
							UE_LOG(CSLog, Warning, TEXT("HoldPosition: AI cannot Shoot Target! Weapon is Breaked! Will Go Back Home!"));
							HoldPositionState = ECSHoldPositionState::BackHome;
							CurrentTarget = nullptr;
						}
					}
				}
				else
				{
					UE_LOG(CSLog, Warning, TEXT("HoldPosition: AI cannot Shoot Target! No Weapon! Will Go Back Home!"));
					HoldPositionState = ECSHoldPositionState::BackHome;
					CurrentTarget = nullptr;
				}
			}
			else
			{
				HoldPositionState = ECSHoldPositionState::Chase;
			}
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("HoldPosition: AI is in Attack but Target is missing! Will Go Back Home!"));
			HoldPositionState = ECSHoldPositionState::BackHome;
			CurrentTarget = nullptr;
		}
	}
	else if (nullptr != MainWeapon)
	{
		MainWeapon->StopFire();
		MainWeapon->StopReload();
	}
}
void ACSAICharacterBase::UpdatePatrolState_Implementation(float DeltaSeconds)
{
	// Patrol
	if (CheckPatrolState(ECSPatrolState::Patrol))
	{
		if (ACBase->MoveIdle())
		{
			bNeedUpdatePatrolTimer = true;
			if (PatrolWaitTimeInWaypoint <= PatrolTimer)
			{
				PatrolTimer = 0;
				bNeedUpdatePatrolTimer = false;
				NextPatrolWaypointIndex();
				ACBase->MoveToActor(PatrolWaypoints[PatrolWaypointIndex], -1, true, true, false);
			}
			else
			{
				ACSPlayerCharacterBase* NewWatch = Watch();
				if (nullptr != NewWatch)
				{
					CurrentTarget = NewWatch;
					CallComrades(CurrentTarget);
					PatrolState = ECSPatrolState::Chase;
				}
			}
		}
		else
		{
			ACSPlayerCharacterBase* NewWatch = Watch();
			if (nullptr != NewWatch)
			{
				CurrentTarget = NewWatch;
				PatrolState = ECSPatrolState::Chase;
			}
		}
	}

	// Chase
	if (CheckPatrolState(ECSPatrolState::Chase))
	{
		if (nullptr != CurrentTarget)
		{
			bool bInRange, bChest, bPelvis, bHead;
			FVector Muzzle, Chest, Pelvis, Head;
			if (CanShootHim(CurrentTarget, bInRange, bChest, bPelvis, bHead, Chest, Pelvis, Head, Muzzle))
			{
				ACBase->StopMovement();
				PatrolState = ECSPatrolState::Attack;
			}
			else
			{
				ACBase->MoveToActor(CurrentTarget, -1, true, true, false);
			}
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("Patrol: AI Cannot Chase! Target is missing! Will Patrol!"));
			PatrolState = ECSPatrolState::Patrol;
			CurrentTarget = nullptr;
		}
	}

	// Attack
	if (CheckPatrolState(ECSPatrolState::Attack))
	{
		if (nullptr != CurrentTarget)
		{
			bool bInRange, bChest, bPelvis, bHead;
			FVector Muzzle, Chest, Pelvis, Head;
			if (CanShootHim(CurrentTarget, bInRange, bChest, bPelvis, bHead, Chest, Pelvis, Head, Muzzle))
			{
				if (bChest)
				{
					CurrentShootLocation = Chest;
				}
				else if (bPelvis)
				{
					CurrentShootLocation = Pelvis;
				}
				else if (bHead)
				{
					CurrentShootLocation = Head;
				}
				else
				{
					UE_LOG(CSLog, Warning, TEXT("Patrol: AI cannot Determine Shoot Location! Will Patrol!"));
					PatrolState = ECSPatrolState::Patrol;
					CurrentTarget = nullptr;
				}

				if (nullptr != MainWeapon)
				{
					if (!MainWeapon->StartFire())
					{
						bReloadWhole = MainWeapon->NeedToPullReloadHandle();
						if (!MainWeapon->StartReload())
						{
							UE_LOG(CSLog, Warning, TEXT("HoldPosition: AI cannot Shoot Target! Weapon is Breaked! Will Patrol!"));
							PatrolState = ECSPatrolState::Patrol;
							CurrentTarget = nullptr;
						}
					}
				}
				else
				{
					UE_LOG(CSLog, Warning, TEXT("HoldPosition: AI cannot Shoot Target! No Weapon! Will Patrol!"));
					PatrolState = ECSPatrolState::Patrol;
					CurrentTarget = nullptr;
				}
			}
			else
			{
				PatrolState = ECSPatrolState::Chase;
			}
		}
		else
		{
			UE_LOG(CSLog, Warning, TEXT("HoldPosition: AI is in Attack but Target is missing! Will Patrol!"));
			PatrolState = ECSPatrolState::Patrol;
			CurrentTarget = nullptr;
		}
	}
	else if (nullptr != MainWeapon)
	{
		MainWeapon->StopFire();
		MainWeapon->StopReload();
	}
}

FRotator ACSAICharacterBase::CalcShootNoise_Implementation(UCSWeaponBase* Weapon)
{
	const float Concecutive = Weapon->GetConsecutiveFire();
	FRotator Result;
	Result.Pitch = FMath::FRandRange(10, 50) * FMath::FRandRange(-1, 1) > 0 ? 1 : -1;
	Result.Yaw = FMath::FRandRange(10, 50) * FMath::FRandRange(-1, 1) > 0 ? 1 : -1;
	Result.Roll = FMath::FRandRange(10, 50) * FMath::FRandRange(-1, 1) > 0 ? 1 : -1;
	return Result * (Concecutive / 10);
}
FVector ACSAICharacterBase::CalcShootDirection_Implementation(UCSWeaponBase* Weapon, const FVector& MuzzleLocation)
{
	FVector Result = CurrentShootLocation - MuzzleLocation;
	Result.Normalize();
	return Result;
}

void ACSAICharacterBase::TakeHit_Implementation(ACSProjectileBase* HitProjectile, const FVector& Location, const FVector& Direction, const ECSBodyPart HitPart)
{
	Super::TakeHit_Implementation(HitProjectile, Location, Direction, HitPart);
	ACSPlayerCharacterBase* PlayerCharacter = Cast<ACSPlayerCharacterBase>(HitProjectile->GetInstigator());
	if (nullptr != PlayerCharacter)
	{
		SetNewTarget(PlayerCharacter);
		CallComrades(PlayerCharacter);
	}
}

void ACSAICharacterBase::GetMovementAnimInfo_Implementation(float& OutMoveAnimRatio, float& OutRealSpeed, FVector& OutRealDirLocal, FVector& OutRealDirWorld)
{
	Super::GetMovementAnimInfo_Implementation(OutMoveAnimRatio, OutRealSpeed, OutRealDirLocal, OutRealDirWorld);
	OutMoveAnimRatio = OutRealSpeed / MovementInfos[(int32)ECSMovementState::Run].MoveSpeed;
}

void ACSAICharacterBase::HearNoise_Implementation(ACSPlayerCharacterBase* Maker)
{
	SetNewTarget(Maker);
	CallComrades(Maker);
}

bool ACSAICharacterBase::SetNewTarget_Implementation(ACSPlayerCharacterBase* Target)
{
	check(nullptr != Target);
	if (CurrentTarget != Target)
	{
		CurrentTarget = Target;
		switch (AIType)
		{
		case ECSAIType::HoldPosition:
			HoldPositionState = ECSHoldPositionState::Chase;
			break;
		case ECSAIType::Patrol:
			PatrolState = ECSPatrolState::Chase;
			break;
		default:
			// Not Implemented!
			check(false);
			break;
		}
		return true;
	}
	return false;
}

bool ACSAICharacterBase::CheckHoldPositionState_Implementation(const ECSHoldPositionState Check)
{
	return Check == HoldPositionState.GetValue();
}

bool ACSAICharacterBase::CheckPatrolState_Implementation(const ECSPatrolState Check)
{
	return Check == PatrolState.GetValue();
}

FVector ACSAICharacterBase::GetEyeLocation_Implementation()
{
	return GetHeadLocation();
}

bool ACSAICharacterBase::InHome_Implementation()
{
	return (GetActorLocation() - HomeLocation).SizeSquared() <= FMath::Square(HomeRadius);
}

bool ACSAICharacterBase::NearlyHome_Implementation(float Tolerance)
{
	return GetActorLocation().Equals(HomeLocation, Tolerance);
}

void ACSAICharacterBase::CallComrades_Implementation(ACSPlayerCharacterBase* NewTarget)
{
	check(nullptr != NewTarget);
	for (ACSAICharacterBase* C : Comrades)
	{
		if (nullptr != C)
		{
			C->SetNewTarget(NewTarget);
		}
	}
}

bool ACSAICharacterBase::SphereTraceToPlayerCharacter_Implementation(ACSPlayerCharacterBase* Check, const FVector& Start, const FVector& End, const float Radius, TSubclassOf<AActor> IgnoreClass)
{
	check(nullptr != Check);
	UWorld* World = GetWorld();
	if (nullptr != World)
	{
		FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
		FCollisionQueryParams QueryParams;
		for (TActorIterator<AActor> I(World, (UClass*)IgnoreClass); I; ++I)
		{
			QueryParams.AddIgnoredActor(*I);
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
		if (World->SweepSingle(HitResult, Start, End, FQuat::Identity, Shape, QueryParams, ObjectQueryParams))
		{
			return HitResult.Actor == Check;
		}
	}
	return false;
}
bool ACSAICharacterBase::IsInWatchSight_Implementation(ACSPlayerCharacterBase* Check, bool& OutInDistance, bool& OutInDegrees, bool& OutCanSee)
{
	check(nullptr != Check);
	bool bInDistance = false, bInDegrees = false, bCanSee = false;
	const FVector TargetHead = Check->GetHeadLocation();
	const FVector RelativeLocation = TargetHead - GetEyeLocation();
	bInDistance = RelativeLocation.SizeSquared() <= FMath::Square(WatchingDistance);
	if (bInDistance)
	{
		float Pitch, Yaw;
		if (UCSUtil::LocationPitchYaw(GetActorRotation(), RelativeLocation, Pitch, Yaw))
		{
			bInDegrees = -WatchingDegrees.Y <= Pitch && Pitch <= WatchingDegrees.X && FMath::Abs(Yaw) <= WatchingDegrees.Z;
			if (bInDegrees)
			{
				bCanSee = SphereTraceToPlayerCharacter(Check, GetHeadLocation(), TargetHead, IsInWatchSightTraceRadius, ACSAICharacterBase::StaticClass());
			}
		}
	}
	OutInDistance = bInDistance;
	OutInDegrees = bInDegrees;
	OutCanSee = bCanSee;
	return bInDistance && bInDegrees && bCanSee;
}
ACSPlayerCharacterBase* ACSAICharacterBase::Watch_Implementation()
{
	TArray<ACSPlayerCharacterBase*> Candidates;
	for (TActorIterator<ACSPlayerCharacterBase> I(GetWorld()); I; ++I)
	{
		bool InDistance, InDegree, CanSee;
		if (IsInWatchSight(*I, InDistance, InDegree, CanSee))
		{
			Candidates.Add(*I);
		}
	}

	if (Candidates.Num() == 1)
	{
		return Candidates[0];
	}
	else if (Candidates.Num() > 0)
	{
		return Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	}
	return nullptr;
}

bool ACSAICharacterBase::CanShootHim_Implementation(ACSPlayerCharacterBase* He, bool& OutInRange, bool& OutCanChest, bool& OutCanPelvis, bool& OutCanHead, FVector& OutHisChestLocation, FVector& OutHisPelvisLocation, FVector& OutHisHeadLocation, FVector& OutMyMuzzleLocation)
{
	check(nullptr != He);
	bool bInRange = false, bCanChest = false, bCanPelvis = false, bCanHead = false;
	const FVector HisLocation = He->GetActorLocation();
	const FVector MyLocation = GetActorLocation();
	bInRange = (HisLocation - MyLocation).SizeSquared() <= FMath::Square(ShootableRange);
	if (bInRange)
	{
		if (nullptr != MainWeapon)
		{
			const FVector MuzzleLocation = MainWeapon->GetMuzzleLocation();
			const FVector ChestLocation = He->GetChestLocation();
			const FVector PelvisLocation = He->GetPelvisLocation();
			const FVector HeadLocation = He->GetHeadLocation();
			bCanChest = SphereTraceToPlayerCharacter(He, MuzzleLocation, ChestLocation, CanShootHimTraceRadius, ACSAICharacterBase::StaticClass());
			bCanPelvis = SphereTraceToPlayerCharacter(He, MuzzleLocation, PelvisLocation, CanShootHimTraceRadius, ACSAICharacterBase::StaticClass());
			bCanHead = SphereTraceToPlayerCharacter(He, MuzzleLocation, HeadLocation, CanShootHimTraceRadius, ACSAICharacterBase::StaticClass());
			OutMyMuzzleLocation = MuzzleLocation;
			OutHisChestLocation = ChestLocation;
			OutHisPelvisLocation = PelvisLocation;
			OutHisHeadLocation = HeadLocation;
		}
	}
	OutInRange = bInRange;
	OutCanChest = bCanChest;
	OutCanPelvis = bCanPelvis;
	OutCanHead = bCanHead;
	return bInRange && (bCanChest || bCanPelvis || bCanHead);
}

void ACSAICharacterBase::NextPatrolWaypointIndex_Implementation()
{
	const int32 Count = PatrolWaypoints.Num();
	const int32 Last = Count - 1;
	switch (PatrolMode)
	{
	case ECSPatrolMode::Roam:
	{
		if (bPatrolRoamPositive)
		{
			if (++PatrolWaypointIndex > Last)
			{
				PatrolWaypointIndex -= 2;
				bPatrolRoamPositive = false;
			}
		}
		else
		{
			if (--PatrolWaypointIndex < 0)
			{
				PatrolWaypointIndex += 2;
				bPatrolRoamPositive = true;
			}
		}
		break;
	}
	case ECSPatrolMode::Loop:
	{
		if (++PatrolWaypointIndex > Last)
		{
			PatrolWaypointIndex = 0;
		}
		break;
	}
	}
}

