#pragma once

#include "CoreMinimal.h"
#include "Items/Public/Item.h"
#include "Treasure.generated.h"

//Forward Declarations
class USoundBase;

UCLASS()
class SLASH_5_6_API ATreasure : public AItem
{
	GENERATED_BODY()

public:
	ATreasure();

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold;

public:

	FORCEINLINE int32 GetGold() { return Gold; }


};
