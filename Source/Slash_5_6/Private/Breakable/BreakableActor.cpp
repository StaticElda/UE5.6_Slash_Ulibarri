
#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Public/Treasure/Treasure.h"
#include "Components/CapsuleComponent.h"

ABreakableActor::ABreakableActor()
{
	//Non ci serve che i Breakable Actors abbiano la Tick attiva 	
	PrimaryActorTick.bCanEverTick = false;

	//Inizializziamo il GeometryCollection
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);

	//Settiamogli le Collisioni 
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GeometryCollection->EnableClustering = true;

	//Inizializiamo la Capsule 
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	//Diciamogli di ignorare tutto tranne i Pawn
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}	


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{

	if (bBroken) return;
	//Settiamo bBroken a true, ora abiamo un modo di sapere se il vaso è già rotto
	bBroken = true;

	UWorld* World = GetWorld();
	//.Num() ci ritorna la taglia dell'array
	if (World && TreasureClasses.Num()>0) 
	{
		
		FVector Location = GetActorLocation();
		Location.Z += 125.f;

		//Genero un numero intero casuale fra 0 e la taglia dell'array -1
		const int32 RandSelection = FMath::RandRange(0, TreasureClasses.Num() - 1);

		//Attenzione : Siccome l'array è dinamico (E la sua taglia può variare a runtime) 
		//accedere a un indice out of bound non causa errore di complilazione, 
		//ma fa crushare il programma 
		World->SpawnActor<ATreasure>(TreasureClasses[RandSelection], Location, GetActorRotation());
		
		//GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Emerald, FString("Vaso Rotto!"), true);
	}
	
}

