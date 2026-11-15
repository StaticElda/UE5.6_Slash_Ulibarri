#pragma once

#include "CoreMinimal.h"
#include "Characters/CharactersStatusTypes.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/PickupInterface.h"

#include "SlashCharacter.generated.h"


//Forward Declarations
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USlashOverlay;
class AExp;

UCLASS()
class SLASH_5_6_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddExp(AExp* Exp) override;
	virtual void AddGold(ATreasure* Treasure) override;


protected:
	virtual void BeginPlay() override;

	
	
	//Callbacks for input
	void MoveForward(float Value);
	void Turn (float Value);
	void LookUp(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;

	

	void EKeyPressed();
	void RKeyPressed();

	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;

	virtual void Die() override;
	bool IsDead();

	bool IsUnoccupied();


	//Play Montage functions
	virtual int32 PlayAttackMontage() override;
	void PlayEquip_UnequipMontage(FName SectionName);
 

	UFUNCTION(BlueprintCallable)
	void AttachWeaponInHand();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponOnShoulder();

	
	virtual void HitReactEnd() override;


	UPROPERTY(EditAnywhere)
	TArray<FName> OneHandedAttackSections;

	UPROPERTY(EditAnywhere)
	TArray<FName> TwoHandedAttackSections;

private:

	void Landed(const FHitResult& Hit) override;
	
	void LandingTimerFinished();

	void SetCharacterState(AWeapon* Weapon);

	void InitializeSlashOverlay();
	void SetHUDHealth();

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	//Usiamo  meta = (AllowPrivateAccess = "true") per permettere al Blueprint di modificare la Property anche se è private
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	FTimerHandle LandingTimer;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	//Animation Montages
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* Equip_UnequipMontage;

public:
	
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState;  }

};
