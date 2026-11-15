

#pragma once	
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"	
#include "Item.generated.h"	

//Forward Declaration
class USphereComponent;
class UNiagaraSystem;

//Creiamo un EnumType per gestire gli stati degli oggetti
//Per ora non la esponiamo al Reflection
enum class EItemState : uint8
{
	EIS_OnTheFloor UMETA(DisplayName = "OnTheFloor"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
};

UCLASS()	
class SLASH_5_6_API AItem : public AActor
{
	GENERATED_BODY()	
	
public:	
	
	AItem();

	virtual void Tick(float DeltaTime) override;

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))	
	float Frequenza = 2 ;

	template <typename T>
	T Avg(T First, T Second);
	 

protected:
	
	virtual void BeginPlay() override;

	virtual void SpawnPickupEffect();
	virtual void SpawnPickupSound();


	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* PickupSound;


	EItemState ItemState = EItemState::EIS_OnTheFloor ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oscillazione")	
	float Raggio = 1;

	UFUNCTION(BlueprintPure)
	float transformedSin(float value);

	UFUNCTION(BlueprintPure)
	float transformedCos(float value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;
	

	//Definiamo la funzione di callback, i parametri devono coincidere con quelli richiesti dal delegate
	//La esponiamo al reflection system, è necessario per fare il bind al delegate
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* ItemEffect;

public:

	EItemState GetItemState();

	
	
};


template<typename T>
inline T AItem::Avg(T First, T Second) 
{
	return (First + Second)/2;
}
