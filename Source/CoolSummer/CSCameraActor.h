// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "CSCameraActor.generated.h"

class ACSPlayerCharacterBase;

UENUM(BlueprintType)
enum class ECSZoomState
{
	Idle, IronSight, Sprint, Count
};

UENUM(BlueprintType)
enum class ECSHeightState
{
	Stand, Crouch, Prone, Count
};

USTRUCT(BlueprintType)
struct FCSZoomInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(Category = "CS", BlueprintReadWrite) float FOV;
};

USTRUCT(BlueprintType)
struct FCSHeightInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(Category = "CS", BlueprintReadWrite) FVector Focus;
	UPROPERTY(Category = "CS", BlueprintReadWrite) float Distance;
};

UCLASS()
class COOLSUMMER_API ACSCameraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) static FCSZoomInfo GetZoomInfo(const ECSZoomState State);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) static FCSHeightInfo GetHeightInfo(const ECSHeightState State);

	// Sets default values for this actor's properties
	ACSCameraActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Update as Orbitting TPS Camera.
	void UpdateCamera(const float DeltaTime, const FVector& Origin, const FRotator& ControlRotation);

	void Initialize(ACSPlayerCharacterBase* OwnerCharacter);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) FVector GetCameraWorldLocation();

	UFUNCTION(Category = "CS", BlueprintCallable) void SetZoomState(const ECSZoomState NewState);
	UFUNCTION(Category = "CS", BlueprintCallable) void SetHeightState(const ECSHeightState NewState);

protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly) ACSPlayerCharacterBase* PlayerCharacterBase;

	UPROPERTY(Category = "CS", BlueprintReadOnly) USceneComponent* Root;
	UPROPERTY(Category = "CS", BlueprintReadOnly) UCameraComponent* Camera;

	UPROPERTY(Category = "CS", BlueprintReadOnly) TEnumAsByte<ECSZoomState> ZoomState;
	UPROPERTY(Category = "CS", BlueprintReadWrite) float FOVLerpSpeed;

	UPROPERTY(Category = "CS", BlueprintReadOnly) TEnumAsByte<ECSHeightState> HeightState;
	UPROPERTY(Category = "CS", BlueprintReadWrite) float HeightLerpSpeed;

	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) bool bDebugTrace;

	static const FCSZoomInfo ZoomInfos[(int32)ECSZoomState::Count];
	static const FCSHeightInfo HeightInfos[(int32)ECSHeightState::Count];
};
