#pragma once

#include "CoreMinimal.h"

#include "Interfaces/HitInterface.h"
#include "Characters/BaseCharacter.h"

#include "Enemy.generated.h"


//Forward Declaration
class UAnimMontage;
class AExp;

UCLASS()
class SLASH_5_6_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemy();

	// Override di Actor
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

	//Override di IHitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;


protected:

	// Override di Actor
	virtual void BeginPlay() override;

	//Override di BaseCharacter
	virtual void Die() override;
	void SpawnExp();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	virtual void HitReactEnd() override;
	// Play Montage functions
	virtual int32 PlayAttackMontage() override;	

	UPROPERTY(VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patroling;

	

private: 

	// AI Behavior
	void InitializeEnemy();
	void SpawnDefaultWeapon();
	void MoveToTarget(AActor* Target);
	bool InTargetRange(AActor* Target, double Radius);
	void ChangePatrolTarget();

	void CheckPatrolTarget();

	UFUNCTION(BlueprintCallable)
	void CheckCombatTarget();

	void HideHealthBar();
	void ShowHealthBar();

	void LoseInterest();
	void StartPatroling();
	void StartChasing();
	
	bool IsChasing();
	bool IsPatroling();
	bool IsAttacking();
	bool IsEngaged();
	bool IsDead();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();

	void ClearPatrolTimer();
	void PatrolTimerFinished();

	void StartAttackTimer();
	void ClearAttackTimer();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); //Callback for OnPawnSeen in UPawnSensingComponent

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float TargetAcceptanceRadius = 100.f;

	UPROPERTY(VisibleAnywhere)
	class UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double AttackRadius = 230.f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY()
	class AAIController* EnemyController;

	//Patroling

	//Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	bool bLoopPatrol = true;

	UPROPERTY(EditAnywhere, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	bool bRandomizePatrol = true;

	UPROPERTY(EditAnywhere, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	float MinPatrolWaitTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	float MaxPatrolWaitTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float PatrolingSpeed = 125.f;
	
	// Combat

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnyWhere, Category = "Combat")
	float AttackDelayMin = 0.f;

	UPROPERTY(EditAnyWhere, Category = "Combat")
	float AttackDelayMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AExp> ExpClass;
	
};
