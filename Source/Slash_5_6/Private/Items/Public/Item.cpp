#include "Items/Public/Item.h"
#include "DrawDebugHelpers.h"
#include "DebugMacros.h"
#include <numbers>


#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));

	//Disabilito le Collisioni
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent = ItemMesh;

	//Sphere Component
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComp->SetupAttachment(GetRootComponent());

	//Niagara Component
	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(GetRootComponent());

}


void AItem::BeginPlay()
{
	Super::BeginPlay();

	int32 AvgInt = Avg<int32>(3, 1);
	UE_LOG(LogTemp, Warning, TEXT("AVG : %d"), AvgInt);

	float AvgFloat = Avg<float>(3.5, 1.f);
	UE_LOG(LogTemp, Warning, TEXT("AVG : %f"), AvgFloat);

	//Facciamo il Bind della Callback al Delegate, va fatto nella BeginPlay non nel costruttore.
	//AddDynamic chiede come argomento :
	//l'oggetto che deve chiamare la funzione di callback
	//La funzione di callback stessa.
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	// E con questo abbiamo fatto il bind della funzione al Delegate

	SphereComp->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::transformedSin(float value)
{
	return Raggio * FMath::Sin(value * Frequenza);
}

float AItem::transformedCos(float value)
{
	return Raggio * FMath::Cos(value * Frequenza);
}

//Definiamo la funzione di callback
void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	FString OtherActorName =  OtherActor->GetName();
	FString Message = TEXT("Actor in Begin Overlap with the sphere : ") + OtherActorName;
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, Message);
	}
	*/
	IPickupInterface* PickInterface = Cast<IPickupInterface>(OtherActor);
	if (PickInterface)
	{
		PickInterface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*
	FString OtherActorName = OtherActor->GetName();
	FString Message = TEXT("Actor in End Overlap with the sphere : ") + OtherActorName;
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, Message);
	}
	*/
	IPickupInterface* pickInterface = Cast<IPickupInterface>(OtherActor);
	if (pickInterface)
	{
		pickInterface->SetOverlappingItem(nullptr);
	}
	
}


void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	
	//Facciamo sobbalzare l'oggetto quando è OnTheFloor
	if (ItemState == EItemState::EIS_OnTheFloor) 
	{
		float Z_Offset = transformedSin(RunningTime);
		AddActorWorldOffset(FVector(0.f,0.f, Z_Offset));
	}
	
}

EItemState AItem::GetItemState()
{
	return ItemState ;
}

void AItem::SpawnPickupEffect()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation()
		);
	}
}

void AItem::SpawnPickupSound()
{
	if (PickupSound) 
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}
}






