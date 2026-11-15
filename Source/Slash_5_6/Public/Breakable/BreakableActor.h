#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

//Non possiamo usare una forward declaration perchè Enemy eredita da HitInterface
#include "Interfaces/HitInterface.h"

#include "BreakableActor.generated.h"

//Farward Declaration 
class UGeometryCollectionComponent;


UCLASS()
class SLASH_5_6_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	
	ABreakableActor();

	virtual void Tick(float DeltaTime) override;

	//Facciamo l'Override della funzione GetHit di HitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	
	virtual void BeginPlay() override;


private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* Capsule;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UGeometryCollectionComponent* GeometryCollection;
	
	UPROPERTY(EditAnywhere, Category="Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	bool bBroken = false;
};
