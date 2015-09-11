// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CSExplosive.generated.h"

class ACSCharacterBase;

/**
 * 
 */
UCLASS()
class COOLSUMMER_API ACSExplosive : public AActor
{
	GENERATED_BODY()
	
public:
	ACSExplosive(const FObjectInitializer& Initializer);

	virtual void ReceiveHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void ReceiveActorBeginOverlap(AActor* Other) override;

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintNativeEvent)
		void Explode(ACSCharacterBase* InInstigator);
	virtual void Explode_Implementation(ACSCharacterBase* InInstigator);

	FORCEINLINE int32 GetCenterTemperature()
	{
		return CenterTemperature;
	}
	FORCEINLINE int32 GetBorderTemperature()
	{
		return BorderTemperature;
	}

protected:
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) USphereComponent* TouchComponent;
	UPROPERTY(Category = "CS", BlueprintReadOnly, VisibleAnywhere) UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) UParticleSystem* ExplodeParticle;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) FVector ExplodeParticleScale;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) USoundBase* ExplodeSound;

	UPROPERTY(Category = "CS", BlueprintReadOnly) bool bExploded;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) bool bTouchExplode;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) float ExplodeRadius;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) int32 CenterDamage;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) int32 BorderDamage;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) int32 CenterTemperature;
	UPROPERTY(Category = "CS", BlueprintReadOnly, EditAnywhere) int32 BorderTemperature;
};
