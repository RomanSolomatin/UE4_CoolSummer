// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSUtil.h"
#include "CSPlayerCharacterBase.h"
#include "CSCameraActor.h"

const FCSZoomInfo ACSCameraActor::ZoomInfos[(int32)ECSZoomState::Count] =
{
	{ 90 }, { 60 }, { 80 }
};

const FCSHeightInfo ACSCameraActor::HeightInfos[(int32)ECSHeightState::Count] =
{
	{ FVector(20, 50, 70), 180 }, { FVector(20, 50, 20), 150 }
};

FCSZoomInfo ACSCameraActor::GetZoomInfo(const ECSZoomState State)
{
	check(ECSZoomState::Count > State);
	return ZoomInfos[(int32)State];
}

FCSHeightInfo ACSCameraActor::GetHeightInfo(const ECSHeightState State)
{
	check(ECSHeightState::Count > State);
	return HeightInfos[(int32)State];
}

// Sets default values
ACSCameraActor::ACSCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bFindCameraComponentWhenViewTarget = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachTo(RootComponent);

	ZoomState = ECSZoomState::Idle;
	FOVLerpSpeed = 45;

	HeightState = ECSHeightState::Stand;
	HeightLerpSpeed = 5;
}

// Called when the game starts or when spawned
void ACSCameraActor::BeginPlay()
{
	Super::BeginPlay();
	Camera->Activate();
}

// Called every frame
void ACSCameraActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (nullptr != PlayerCharacterBase)
	{
		{
			// HP Critical Effect
			const int32 HP = PlayerCharacterBase->GetHitPoint();
			const int32 HalfMaxHP = PlayerCharacterBase->GetMaxHitPoint() / 2;
			if (HP < HalfMaxHP)
			{
				Camera->PostProcessSettings.bOverride_FilmSaturation = true;
				const float CriticalRatio = float(HP) / float(HalfMaxHP);
				Camera->PostProcessSettings.FilmSaturation = CriticalRatio;
			}
			else
			{
				Camera->PostProcessSettings.bOverride_FilmSaturation = false;
			}
		}
	}
}

void ACSCameraActor::Initialize(ACSPlayerCharacterBase* OwnerCharacter)
{
	check(nullptr != OwnerCharacter);
	PlayerCharacterBase = OwnerCharacter;
}

void ACSCameraActor::UpdateCamera(const float DeltaTime, const FVector& Origin, const FRotator& ControlRotation)
{
	{
		const FCSHeightInfo& Info = HeightInfos[(int32)HeightState.GetValue()];
		FRotator FocusOffsetRotation(0, ControlRotation.Yaw, 0);
		FVector WorldFocusOffset = FocusOffsetRotation.RotateVector(Info.Focus);
		SetActorLocation(Origin + WorldFocusOffset);
		Root->SetRelativeRotation(FRotator(ControlRotation.Pitch, ControlRotation.Yaw, 0));


		// Simulate Spring Arm. Because it has some bugs.
		const FVector RayStart = Root->GetComponentLocation();
		const FVector RayDirection = Root->GetComponentRotation().RotateVector(-FVector::ForwardVector);
		const FVector RayEnd = RayStart + (RayDirection * Info.Distance);
		FVector TargetLocation;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(PlayerCharacterBase);
		FHitResult Result;
		if (GetWorld()->LineTraceSingle(Result, RayStart, RayEnd, ECollisionChannel::ECC_Camera, Params))
		{
			TargetLocation = Result.Location;
			if (bDebugTrace)
			{
				UE_LOG(CSLog, Log, TEXT("Actor: [%s] Comp: [%s] Loc: [%f, %f, %f]"), *Result.Actor->GetName(), *Result.Component->GetName(), Result.Location.X, Result.Location.Y, Result.Location.Z);
			}
		}
		else
		{
			TargetLocation = RayEnd;
		}
		Camera->SetWorldLocation(FMath::VInterpTo(Camera->GetComponentLocation(), TargetLocation, DeltaTime, HeightLerpSpeed));
	}
	{
		// Do FOV Anim.
		const FCSZoomInfo& Info = ZoomInfos[(int32)ZoomState.GetValue()];
		Camera->FieldOfView = FMath::FInterpTo(Camera->FieldOfView, Info.FOV, DeltaTime, FOVLerpSpeed);
	}
}

FVector ACSCameraActor::GetCameraWorldLocation()
{
	return Camera->GetComponentLocation();
}

void ACSCameraActor::SetZoomState(const ECSZoomState NewState)
{
	ZoomState = NewState;
}
void ACSCameraActor::SetHeightState(const ECSHeightState NewState)
{
	HeightState = NewState;
}
