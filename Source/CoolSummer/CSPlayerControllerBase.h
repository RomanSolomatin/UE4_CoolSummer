// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "CSPlayerControllerBase.generated.h"

class ACSPlayerCharacterBase;

/**
 * 
 */
UCLASS()
class COOLSUMMER_API ACSPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACSPlayerControllerBase(const FObjectInitializer& Initializer);

	virtual void Possess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupInputComponent() override;

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure, Meta = (Tooltip = "Calculate Needing Local Movement Direction from Player Input.")) 
		FVector CalcLocalMovementDirection(bool& OutForward, bool& OutBackward, bool& OutLeft, bool& OutRight);

	void StartMoveForward();
	void StopMoveForward();

	void StartMoveBackward();
	void StopMoveBackward();

	void StartMoveLeft();
	void StopMoveLeft();

	void StartMoveRight();
	void StopMoveRight();

	void StartJump();
	void StopJump();

	void StartSprint();
	void StopSprint();

	void CSStartFire();
	void CSStopFire();

	void StartCrouch();
	void StopCrouch();

	void StartZoom();
	void StopZoom();

	void StartReload();
	void StopReload();

	void CameraPitch(float Scale);
	void CameraYaw(float Scale);
	
protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly) ACSPlayerCharacterBase* PlayerCharacterBase;

	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bMoveForward;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bMoveBackward;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bMoveLeft;
	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bMoveRight;

	UPROPERTY(Category = "CS", BlueprintReadOnly) float MaxPitch;
	UPROPERTY(Category = "CS", BlueprintReadOnly) float MinPitch;

	UPROPERTY(Category = "CS", BlueprintReadWrite) bool bIgnoreInput;
};
