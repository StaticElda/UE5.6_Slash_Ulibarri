#pragma once

#include "CoreMinimal.h"

//Non possiamo usare una forward declaration perchè Enemy eredita da HitInterface
#include "Interfaces/HitInterface.h"
#include "Characters/CharactersStatusTypes.h"

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"


//ForwardDeclaration
class AWeapon;

UCLASS()
class SLASH_5_6_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	virtual void Jump() override;
	virtual void Attack();
	virtual bool CanAttack();
	virtual void Die();
	virtual bool IsAlive();
	virtual void HandleDamage(float DamageAmount);
	void DisableCapsuleCollision();

	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void PlayHitParticles(const FVector& ImpactPoint);

	//Play Montage functions
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void PlayHitReactMontage(const FName& SectionName);
	void PlayMontageSection(UAnimMontage* AnimationMontage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNamesArray);
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void HitReactEnd();



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* OwnedWeapon;

	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* Attributes;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* HitParticles;

	//Animation Montages
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName>AttackMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName>DeathMontageSections;
	
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeadPose> DeadPose;

public:
	FORCEINLINE TEnumAsByte<EDeadPose> GetDeadPose() { return DeadPose; }
};
