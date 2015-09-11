// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSPlayerCharacterBase.h"
#include "CSPlayerControllerBase.h"

ACSPlayerControllerBase::ACSPlayerControllerBase(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	MaxPitch = 30;
	MinPitch = -50;
}

void ACSPlayerControllerBase::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	PlayerCharacterBase = Cast<ACSPlayerCharacterBase>(InPawn);
	if (nullptr == PlayerCharacterBase)
	{
		UE_LOG(CSLog, Warning, TEXT("Failed to get PlayerCharacterBase!"));
	}
}

void ACSPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACSPlayerControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIgnoreInput)
	{
		bMoveForward = false;
		bMoveBackward = false;
		bMoveLeft = false;
		bMoveRight = false;

		if (nullptr != PlayerCharacterBase)
		{
			PlayerCharacterBase->SetWantToJump(false);
			PlayerCharacterBase->SetWantToSprint(false);
			PlayerCharacterBase->SetWantToFire(false);
			PlayerCharacterBase->SetWantToCrouch(false);
			PlayerCharacterBase->SetWantToZoom(false);
			PlayerCharacterBase->SetWantToReload(false);
		}
	}
}

void ACSPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("MoveForward"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartMoveForward);
	InputComponent->BindAction(TEXT("MoveForward"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopMoveForward);

	InputComponent->BindAction(TEXT("MoveBackward"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartMoveBackward);
	InputComponent->BindAction(TEXT("MoveBackward"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopMoveBackward);

	InputComponent->BindAction(TEXT("MoveLeft"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartMoveLeft);
	InputComponent->BindAction(TEXT("MoveLeft"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopMoveLeft);

	InputComponent->BindAction(TEXT("MoveRight"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartMoveRight);
	InputComponent->BindAction(TEXT("MoveRight"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopMoveRight);

	InputComponent->BindAxis(TEXT("CameraPitch"), this, &ACSPlayerControllerBase::CameraPitch);
	InputComponent->BindAxis(TEXT("CameraYaw"), this, &ACSPlayerControllerBase::CameraYaw);

	InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartJump);
	InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopJump);

	InputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartSprint);
	InputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopSprint);

	InputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::CSStartFire);
	InputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::CSStopFire);

	InputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartCrouch);
	InputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopCrouch);

	InputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartZoom);
	InputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopZoom);

	InputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &ACSPlayerControllerBase::StartReload);
	InputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Released, this, &ACSPlayerControllerBase::StopReload);
}

FVector ACSPlayerControllerBase::CalcLocalMovementDirection(bool& OutForward, bool& OutBackward, bool& OutLeft, bool& OutRight)
{
	FVector Result = FVector::ZeroVector;
	if (bMoveForward && !bMoveBackward)
	{
		Result.X = 1;
		OutForward = true;
		OutBackward = false;
	}
	else if (!bMoveForward && bMoveBackward)
	{
		Result.X = -1;
		OutForward = false;
		OutBackward = true;
	}
	else
	{
		OutForward = false;
		OutBackward = false;
	}

	if (bMoveLeft && !bMoveRight)
	{
		Result.Y = -1;
		OutLeft = true;
		OutRight = false;
	}
	else if (!bMoveLeft && bMoveRight)
	{
		Result.Y = 1;
		OutLeft = false;
		OutRight = true;
	}
	else
	{
		OutLeft = false;
		OutRight = false;
	}

	return Result;
}

void ACSPlayerControllerBase::StartMoveForward()
{
	if (!bIgnoreInput)
	{
		bMoveForward = true;
	}
}
void ACSPlayerControllerBase::StopMoveForward()
{
	if (!bIgnoreInput)
	{
		bMoveForward = false;
	}
}

void ACSPlayerControllerBase::StartMoveBackward()
{
	if (!bIgnoreInput)
	{
		bMoveBackward = true;
	}
}
void ACSPlayerControllerBase::StopMoveBackward()
{
	if (!bIgnoreInput)
	{
		bMoveBackward = false;
	}
}

void ACSPlayerControllerBase::StartMoveLeft()
{
	if (!bIgnoreInput)
	{
		bMoveLeft = true;
	}
}
void ACSPlayerControllerBase::StopMoveLeft()
{
	if (!bIgnoreInput)
	{
		bMoveLeft = false;
	}
}

void ACSPlayerControllerBase::StartMoveRight()
{
	if (!bIgnoreInput)
	{
		bMoveRight = true;
	}
}
void ACSPlayerControllerBase::StopMoveRight()
{
	if (!bIgnoreInput)
	{
		bMoveRight = false;
	}
}

void ACSPlayerControllerBase::StartJump()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToJump(true);
	}
}
void ACSPlayerControllerBase::StopJump()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToJump(false);
	}
}

void ACSPlayerControllerBase::StartSprint()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToSprint(true);
	}
}
void ACSPlayerControllerBase::StopSprint()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToSprint(false);
	}
}

void ACSPlayerControllerBase::CSStartFire()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToFire(true);
	}
}
void ACSPlayerControllerBase::CSStopFire()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToFire(false);
	}
}

void ACSPlayerControllerBase::StartCrouch()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToCrouch(true);
	}
}
void ACSPlayerControllerBase::StopCrouch()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToCrouch(false);
	}
}

void ACSPlayerControllerBase::StartZoom()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToZoom(true);
	}
}
void ACSPlayerControllerBase::StopZoom()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToZoom(false);
	}
}

void ACSPlayerControllerBase::StartReload()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToReload(true);
	}
}
void ACSPlayerControllerBase::StopReload()
{
	if (!bIgnoreInput && nullptr != PlayerCharacterBase)
	{
		PlayerCharacterBase->SetWantToReload(false);
	}
}

void ACSPlayerControllerBase::CameraPitch(float Scale)
{
	if (!bIgnoreInput)
	{
		FRotator Src = GetControlRotation();
		Src.Pitch = FMath::ClampAngle(Src.Pitch + Scale, MinPitch, MaxPitch);
		Src.Normalize();
		SetControlRotation(Src);
	}
}
void ACSPlayerControllerBase::CameraYaw(float Scale)
{
	if (!bIgnoreInput)
	{
		FRotator Src = GetControlRotation();
		Src.Yaw += Scale;
		Src.Normalize();
		SetControlRotation(Src);
	}
}
