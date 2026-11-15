#include "Items/Public/Treasure/Treasure.h"
// Include per la Sphere Component
#include "Components/SphereComponent.h"

#include "Interfaces/PickupInterface.h"



ATreasure::ATreasure()
{
	ItemState = EItemState::EIS_OnTheFloor;
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GEngine)
	{
		//FString OtherActorName = OtherActor->GetName();
		//FString Message = TEXT("Actor in Begin Overlap with the sphere : ") + OtherActorName;
		
		//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, Message);

		IPickupInterface* PickInterface = Cast<IPickupInterface>(OtherActor);
		if (PickInterface)
		{
			PickInterface->AddGold(this);
		}
		SpawnPickupEffect();
		SpawnPickupSound();

		Destroy();
	}
}
