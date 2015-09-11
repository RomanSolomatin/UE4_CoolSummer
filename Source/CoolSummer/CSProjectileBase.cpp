// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSWeaponBase.h"
#include "CSCharacterBase.h"
#include "CSProjectileBase.h"


// Sets default values
ACSProjectileBase::ACSProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Block);
	CollisionComponent->SetSphereRadius(5, false);

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileComponent->SetUpdatedComponent(RootComponent);
	ProjectileComponent->bRotationFollowsVelocity = true;

	LifeTime = 10;
	Age = 0;
}

// Called when the game starts or when spawned
void ACSProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSProjectileBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	Age += DeltaTime;
	if (LifeTime <= Age)
	{
		Destroy();
	}
}

void ACSProjectileBase::ReceiveHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	ACSCharacterBase* Character = Cast<ACSCharacterBase>(Other);
	if (nullptr != Character)
	{
		Character->TakeHit(this, HitLocation, -HitNormal, Character->CheckBodyPart(Hit.BoneName));
	}
	Destroy();
}

void ACSProjectileBase::Initialize_Implementation(UCSWeaponBase* InWeapon)
{
	check(nullptr != InWeapon);
	WeaponBase = InWeapon;
}

void ACSProjectileBase::Shoot_Implementation(const FVector& WorldDirection, const float Velocity)
{
	ProjectileComponent->Velocity = WorldDirection * Velocity;
}
