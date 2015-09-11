// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "CSShellBase.generated.h"

class UCSWeaponBase;

UCLASS()
class COOLSUMMER_API ACSShellBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSShellBase();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	
	virtual void ReceiveHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void Initailize(UCSWeaponBase* InWeapon);
	virtual void Initailize_Implementation(UCSWeaponBase* InWeapon);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void Drop(const FVector& Velocity);
	virtual void Drop_Implementation(const FVector& Velocity);

protected:
	FTimerHandle KillTimerHandle;
	void Kill();

protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly) UCSWeaponBase* WeaponBase;

	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) USceneComponent* SceneComponent;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) UStaticMeshComponent* MeshComponent;

	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) bool bDroppingSoundPlayed;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) USoundBase* DroppingSound;
};
