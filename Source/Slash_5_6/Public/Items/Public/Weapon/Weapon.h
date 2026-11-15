#pragma once

#include "CoreMinimal.h"
#include "Items/Public/Item.h"
#include "Items/Public/Weapon/WeaponsTypes.h"
#include "Weapon.generated.h"

//Forward declaration
class USoundBase;
class UBoxComponent;

UCLASS()
class SLASH_5_6_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	//Dichiariamo il costruttore 
	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void DropOnFloor();
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);

	TArray<AActor*> IgnoreActors;

protected:
	//Definiamo la BeginPlay da usare per fare il binding
	virtual void BeginPlay() override;
	
	// Non essendo override devo usare la MACRO UFUNCTION perchè non sono già UFUNCTION a causa del padre
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;

	bool ActorIsSameType(AActor* OtherActor);
	
	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) ;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	//Start e End sono protected e VisibleAnywhere 
	//in modo da poterli posizionare nel Blueprint alle estremità della lama
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponsType WeaponType = EWeaponsType::EWT_OneHanded;

private:
	void BoxTrace(FHitResult& BoxHit );
	void ExecuteGetHit(FHitResult& BoxHit, AActor* OtherActor);

	void PlayEquipSound();
	void EnableSphereCollision();
	void DisableShereCollision();
	void ActivateEmbers();
	void DeactivateEmbers();
	

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	float Damage = 0.f;

public:

	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

	FORCEINLINE EWeaponsType GetWeaponType() const { return WeaponType; }
};
