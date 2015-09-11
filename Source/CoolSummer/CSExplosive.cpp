// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSPlayerCharacterBase.h"
#include "CSProjectileBase.h"
#include "CSWeaponBase.h"
#include "CSExplosive.h"

ACSExplosive::ACSExplosive(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	TouchComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Touch"));
	SetRootComponent(TouchComponent);
	TouchComponent->bGenerateOverlapEvents = true;
	TouchComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	TouchComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	TouchComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	TouchComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	TouchComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Projectile, ECollisionResponse::ECR_Ignore);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->AttachTo(RootComponent);
	StaticMeshComponent->SetMobility(EComponentMobility::Movable);
	StaticMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

	ExplodeParticleScale = FVector(1);

	ExplodeRadius = 500;
	CenterDamage = 200;
	BorderDamage = 20;
	CenterTemperature = 0;
	BorderTemperature = 0;
}

void ACSExplosive::ReceiveHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	if (!bExploded)
	{
		ACSProjectileBase* Projectile = Cast<ACSProjectileBase>(Other);
		if (nullptr != Projectile)
		{
			Explode(Projectile->GetWeaponBase()->GetCharacterBase());
		}
	}
}
void ACSExplosive::ReceiveActorBeginOverlap(AActor* Other)
{
	Super::ReceiveActorBeginOverlap(Other);
	if (!bExploded && bTouchExplode)
	{
		ACSPlayerCharacterBase* PlayerCharacter = Cast<ACSPlayerCharacterBase>(Other);
		if (nullptr != PlayerCharacter)
		{
			Explode(PlayerCharacter);
		}
	}
}

void ACSExplosive::Explode_Implementation(ACSCharacterBase* InInstigator)
{
	if (!bExploded)
	{
		bExploded = true;

		const FVector MyLocation = GetActorLocation();
		const register float RadiusSq = FMath::Square(ExplodeRadius);
		for (TActorIterator<ACSCharacterBase> I(GetWorld()); I; ++I)
		{
			const FVector OtherLocation = I->GetActorLocation();
			FVector Direction = OtherLocation - MyLocation;
			const register float DistanceSq = Direction.SizeSquared();
			if (DistanceSq <= RadiusSq)
			{
				const register float Distance = FMath::Sqrt(DistanceSq);
				Direction /= Distance;
				const register float Ratio = Distance / ExplodeRadius;
				const int32 Damage = FMath::RoundToInt(FMath::Lerp((float)CenterDamage, (float)BorderDamage, Ratio));
				I->TakeExplosion(this, InInstigator, Direction, Damage, Ratio);
			}
		}

		if (nullptr != ExplodeParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticle, MyLocation)->SetRelativeScale3D(ExplodeParticleScale);
		}
		if (nullptr != ExplodeSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplodeSound, MyLocation);
		}
		Destroy();

		for (TActorIterator<ACSExplosive> I(GetWorld()); I; ++I)
		{
			if (this != *I && !I->bExploded)
			{
				const FVector OtherLocation = I->GetActorLocation();
				FVector Direction = OtherLocation - MyLocation;
				const register float DistanceSq = Direction.SizeSquared();
				if (DistanceSq <= RadiusSq)
				{
					I->Explode(InInstigator);
				}
			}
		}
	}
}
