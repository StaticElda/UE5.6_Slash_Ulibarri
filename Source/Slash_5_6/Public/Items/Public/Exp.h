#pragma once

#include "CoreMinimal.h"
#include "Items/Public/Item.h"
#include "Exp.generated.h"


UCLASS()
class SLASH_5_6_API AExp : public AItem
{
	GENERATED_BODY()

protected: 
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Exp Properties")
	int32 Exps;

public:
	FORCEINLINE int32 GetExps() { return Exps; }
	FORCEINLINE void SetExps(int32 NumberOfExp) { Exps = NumberOfExp; }
};
