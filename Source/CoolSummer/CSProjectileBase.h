// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CSProjectileBase.generated.h"

class UCSWeaponBase;

UCLASS()
class COOLSUMMER_API ACSProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSProjectileBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void ReceiveHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void Initialize(UCSWeaponBase* InWeapon);
	virtual void Initialize_Implementation(UCSWeaponBase* InWeapon);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent) 
		void Shoot(const FVector& WorldDirection, const float Velocity);
	virtual void Shoot_Implementation(const FVector& WorldDirection, const float Velocity);

	FORCEINLINE UCSWeaponBase* GetWeaponBase()
	{
		return WeaponBase;
	}

protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly) UCSWeaponBase* WeaponBase;

	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) USphereComponent* CollisionComponent;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) UProjectileMovementComponent* ProjectileComponent;

	UPROPERTY(Category = "CS", BlueprintReadWrite, EditAnywhere) float LifeTime;
	UPROPERTY(Category = "CS", BlueprintReadWrite) float Age;
};
