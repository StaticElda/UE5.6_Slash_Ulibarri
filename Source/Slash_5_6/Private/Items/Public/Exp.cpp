
#include "Items/Public/Exp.h"
#include "Interfaces/PickupInterface.h"



void AExp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
		PickInterface->AddExp(this);

		SpawnPickupEffect();
		SpawnPickupSound();

		Destroy();
	}
	
}
