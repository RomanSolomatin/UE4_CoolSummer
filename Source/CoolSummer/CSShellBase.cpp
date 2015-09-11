// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSShellBase.h"

ACSShellBase::ACSShellBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->AttachTo(RootComponent);
	MeshComponent->SetRelativeRotation(FRotator(0, -90, 0));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
}

void ACSShellBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACSShellBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ACSShellBase::ReceiveHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	if (!bDroppingSoundPlayed && nullptr != DroppingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DroppingSound, HitLocation);
		bDroppingSoundPlayed = true;

		FTimerDelegate d;
		d.BindUObject(this, &ACSShellBase::Kill);
		GetWorldTimerManager().SetTimer(KillTimerHandle, d, 3, false);
	}
}

void ACSShellBase::Initailize_Implementation(UCSWeaponBase* InWeapon)
{
	check(nullptr != InWeapon);
	WeaponBase = InWeapon;
}

void ACSShellBase::Drop_Implementation(const FVector& Velocity)
{
	MeshComponent->AddImpulse(Velocity, NAME_None, true);
	MeshComponent->AddAngularImpulse(FVector(FMath::FRandRange(-1, 1), FMath::FRandRange(-1, 1), FMath::FRandRange(-2, 2)));
}

void ACSShellBase::Kill()
{
	GetWorldTimerManager().ClearTimer(KillTimerHandle);
	Destroy();
}
