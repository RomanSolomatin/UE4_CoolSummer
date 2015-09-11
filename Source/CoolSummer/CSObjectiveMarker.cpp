// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSPlayerCharacterBase.h"
#include "CSObjectiveMarker.h"

UTexture2D* ACSObjectiveMarker::SpriteTexture = nullptr;

ACSObjectiveMarker::ACSObjectiveMarker()
{
	static struct SStaticWorks
	{
		SStaticWorks()
			: SpriteTextureFinder(TEXT("Texture2D'/Engine/EditorMaterials/TargetIconSpawn.TargetIconSpawn'"))
		{
			SpriteTexture = SpriteTextureFinder.Object;
		}
		ConstructorHelpers::FObjectFinder<UTexture2D> SpriteTextureFinder;
	} StaticWorks;

	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(SphereComponent);
	SphereComponent->bGenerateOverlapEvents = true;
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Projectile, ECollisionResponse::ECR_Ignore);

	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->SetSprite(SpriteTexture);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->AttachTo(RootComponent);

	ID = -1;
}

void ACSObjectiveMarker::BeginPlay()
{
	Super::BeginPlay();
	
}
void ACSObjectiveMarker::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (!bSimpleTouchObjective && nullptr != PlayerCharacter && KillCheck() && CheckObjectiveCompleted())
	{
		Finished();
	}
}
void ACSObjectiveMarker::ReceiveActorBeginOverlap(AActor* Other)
{
	if (bSimpleTouchObjective && nullptr != PlayerCharacter && PlayerCharacter == Other)
	{
		Finished();
	}
}

void ACSObjectiveMarker::Start_Implementation(ACSPlayerCharacterBase* InPlayerCharacter)
{
	check(nullptr != InPlayerCharacter);
	PlayerCharacter = InPlayerCharacter;
	if (nullptr != StartSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), StartSound, PlayerCharacter->GetActorLocation());
	}
}

bool ACSObjectiveMarker::CheckObjectiveCompleted_Implementation()
{
	UE_LOG(CSLog, Warning, TEXT("Objective did not implement Check!"));
	return false;
}

void ACSObjectiveMarker::Finished_Implementation()
{
	if (!bFinalObjective && nullptr != Next)
	{
		Next->Start(PlayerCharacter);
		PlayerCharacter->ChangeObjective(Next);
		Destroy();
	}
	else
	{
		UE_LOG(CSLog, Warning, TEXT("Next Objective Missing."));
	}
}
