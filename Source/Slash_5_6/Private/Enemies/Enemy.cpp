#include "Enemies/Enemy.h"

//Include necessario per mageggiare la SkeletalMeshComponent
#include "Components/SkeletalMeshComponent.h"
#include "Navigation/PathFollowingComponent.h"

#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
//Include per disegnare le Debug Arrow
//#include "Kismet/KismetSystemLibrary.h"
#include "Items/Public/Weapon/Weapon.h"
#include "HUD/HealthBarComponent.h"
#include "AIController.h"
#include "Items/Public/Exp.h"



AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	//Impostiamo il Collision Channel della SkeletalMesh a WorldDynamic perch� 
	//i collision presets della BoxTrace della spada ignora i pawn
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);

	//Facciamo si che Blocchi il canale Visibility
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	//Non vogliamo che la camera reagisca all'Enemy
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	//Abilitiamo gli OverlapEvents 
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Enemy PawnSensingComponent
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	//Settiamogli un p� di properties come abbiamo fatto nel Blueprint
	PawnSensing->SightRadius = 1000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead())
	{
		return;
	}
	if (IsPatroling())
	{
		CheckPatrolTarget();
	}
	else
	{
		if (IsChasing() || IsAttacking() || IsEngaged() )
		{
			CheckCombatTarget();
		}
	}
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	const APawn* Hitter = EventInstigator->GetPawn();
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_Engaged &&
		EventInstigator->GetPawn()->ActorHasTag(FName("EngageableTarget")) &&
		!EventInstigator->GetPawn()->ActorHasTag(FName("Dead"));

	if (bShouldChaseTarget)
	{
		//Recuperiamo il pawn che ha causato i danni (es il character posseduto dal Player)
		CombatTarget = EventInstigator->GetPawn();
		
		ClearPatrolTimer();
		//Faccio andare Enemy in chasing state
		StartChasing();
		
	}
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (OwnedWeapon)
	{
		OwnedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	//Chiamo GetHit nella versione di BaseCharacter
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	//Impedisce ad Enemy di rimettersi a pattugliare dopo essere stato attaccato durante una pausa 
	ClearPatrolTimer();
	ClearAttackTimer();

	StopAttackMontage();

	//Impedisce di mostrare la HealthBar di un Enemy già morto
	if (!IsDead()) 
	{
		ShowHealthBar();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitializeEnemy();

	//ENEMY SENSING
	if (PawnSensing)
	{
		//Chiamo Add Dynamic sul Delegate (PawnSeen dovr� avere gli stessi parametri del Delegate (Cio� un Pawn*))
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	Tags.Add(FName("Enemy"));
}

void AEnemy::InitializeEnemy()
{
	//Settiamo la HealthBar
	if (Attributes && HealthBarWidget)
	{
		//Aggiorniamo la HealthBar
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	//Reandiamo la HealthBar invisibile
	HideHealthBar();

	//ENEMY PATROLING
	MoveToTarget(PatrolTarget);

	//SPAWN DELL'ARMA
	SpawnDefaultWeapon();
}

void AEnemy::Die()
{
	//Impedisce che Enemy muoia piu volte 
	if (EnemyState == EEnemyState::EES_Dead) 
	{
		return;
	}

	EnemyState = EEnemyState::EES_Dead;

	
	ClearAttackTimer();

	//Disattiviamo le BoxCollision sull'arma in modo da non ferire nessuno dopo morto 
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	Super::Die();
	DisableCapsuleCollision();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	HideHealthBar();
	SetLifeSpan(DeathLifeSpan);

	//Spawn dell'Exp
	SpawnExp();

}

void AEnemy::SpawnExp()
{
	UWorld* World = GetWorld();
	if (World && ExpClass && Attributes)
	{

		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 20.f);
		AExp* SpawnedExp = World->SpawnActor<AExp>(ExpClass, SpawnLocation, GetActorRotation());

		//Per evitare il crash se SpawnedExp non spawna correttamente e si prova ad accederci
		if (SpawnedExp) 
		{
			SpawnedExp->SetExps(Attributes->GetExp());
		}
		
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) 
	{
		LoseInterest();
		StartPatroling();
		return;
		
	}
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
	return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (HealthBarWidget)
	{
		//Aggiorniamo la HealthBar
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_Idle;
	CheckCombatTarget();
}

void AEnemy::HitReactEnd()
{
	EnemyState = EEnemyState::EES_Idle;
	CheckCombatTarget();
}


int32 AEnemy::PlayAttackMontage()
{
	//Eseguo il codice di PlayAttackMontage nella versione della classe Padre(BaseCharacter)
	return Super::PlayAttackMontage();
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		//Non ci serve neanche specificare location e rotation tanto la attacchiamo al Socket
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		OwnedWeapon = DefaultWeapon;
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	//Inizializzo EnemyController, ci servir� per chiamare MoveTo
	if (Target == nullptr)
	{
		return;
	}

	if (EnemyController == nullptr)
	{
		EnemyController = Cast<AAIController>(GetController());
	}

	if (EnemyController)
	{
		// la funzione MoveTo ci chiede in input una MoveRequest e un OutPath
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(TargetAcceptanceRadius);

		//Il NavPath serve a gestire i nodi nella navigazione (Serve solo per il Debug qua, poi puoi levarlo)
		FNavPathSharedPtr NavPath;
		if (NavPath.IsValid())
		{
			//Passiamo a MoveTo l'indirizzo si NavPath perch� � effettivamente un puntatore
			EnemyController->MoveTo(MoveRequest, &NavPath);

			TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();

			//Andiamo a disegnare una debug sphere per ogni PathPoint
			for (auto& Point : PathPoints)
			{
				const FVector& Location = Point.Location;
				DrawDebugSphere(GetWorld(), Location, 5.f, 5, FColor::Yellow, false, 10.f);
			}

			//Facciamo una DebugSphere rossa per segnalare il PatrolTarget
			DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 10.f, 5, FColor::Red, false, 10.f);
		}
		else
		{
			EnemyController->MoveTo(MoveRequest);
			//Facciamo una DebugSphere rossa per segnalare il PatrolTarget
			DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 10.f, 5, FColor::Green, false, 10.f);
		}
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target != nullptr)
	{
		const double DistanceToTarget = (Target->GetActorLocation() - this->GetActorLocation()).Size();
		if (DistanceToTarget <= Radius)
		{
			return true;
		}
	}
	return false;
}

void AEnemy::ChangePatrolTarget()
{
	//Cambiamo PatrolTarget
	//Aggiungo in fondo il Target raggiunto
	if (bLoopPatrol)
	{
		PatrolTargets.Add(PatrolTarget);
	}
	//Pesco il nuovo Target dalla cima
	if (!PatrolTargets.IsEmpty())
	{
		if (bRandomizePatrol)
		{
			//PatrolTargets.Num() - 2 cos� escludo l'ultimo elemento che � il Target appena raggiunto
			int32 rand = FMath::RandRange(0, PatrolTargets.Num() - 2);
			PatrolTarget = PatrolTargets[rand];
			PatrolTargets.RemoveAt(rand);
		}
		else
		{
			PatrolTarget = PatrolTargets[0];
			PatrolTargets.RemoveAt(0);
		}
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		ChangePatrolTarget();
		const float RandomDelay = FMath::RandRange(MinPatrolWaitTime, MaxPatrolWaitTime);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, RandomDelay);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (CombatTarget)
	{
		//LOSE INTEREST
		bool test = IsOutsideCombatRadius();
		if (test)
		{
			ClearAttackTimer();
			LoseInterest();
			if (!IsEngaged())
			{
				StartPatroling();
			}
		}
		//FROM ATTACKING TO CHASE
		else if (IsOutsideAttackRadius() && !IsChasing() && !IsEngaged())
		{
			//Se siamo nel CombatRadius, ma non nell'AttackRadius andiamo in chasing se non ci eravamo gi�
			//Poich� questa funzione viene chiamata dentro la Tick verr� chiamata molto spesso, 
			//quindi controlliamo se Enemy non sia gi� in chase per evitare operazioni rindondanti
			ClearAttackTimer();
			StartChasing();
		}
		//FROM CHASE TO ATTACKING
		else if (CanAttack())
		{
			StartAttackTimer();
		}
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		//Reandiamo la HealthBar invisibile
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		//Reandiamo la HealthBar visibile
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	//Svuotiamo CombatTarget
	CombatTarget = nullptr;
	HideHealthBar();
	UE_LOG(LogTemp, Warning, TEXT("Loose Interest"));
}

void AEnemy::StartPatroling()
{
	EnemyState = EEnemyState::EES_Patroling;
	GetCharacterMovement()->MaxWalkSpeed = PatrolingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartChasing()
{
	//Controlliamo se Il SeenPawn � il giocatore
	//Potremmo fare un cast ma � un operazione costosa e PawnSeen viene chiamata spesso
	//Per risparmiare possiamo usare gli ActorTag

	//Se vede il personaggio cambia il suo EnemyState in Chasing
	EnemyState = EEnemyState::EES_Chasing;
	//Aumento la velocit� di Enemy in modo che corra 
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	//Muoviamo Enemy contro il CombatTarget
	MoveToTarget(CombatTarget);
	UE_LOG(LogTemp, Warning, TEXT("Start Chasing"));
}

bool AEnemy::IsPatroling()
{
	return EnemyState == EEnemyState::EES_Patroling;
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartAttackTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Attacking"));
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackDelay = FMath::RandRange(AttackDelayMin, AttackDelayMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, FString::Printf(TEXT("PawnSeenCalled")));

	//Concentriamo il contenuto degli if in una variabile locale bool da controllare 
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_Engaged &&
		SeenPawn->ActorHasTag(FName("EngageableTarget")) && 
		!SeenPawn->ActorHasTag(FName("Dead"));

	// Se il PawnSeen non ha il Tag Dead Enemy lo ignora e continua a farsi le sue cose
	if (bShouldChaseTarget  )
	{
		//Ripuliamo il PatrolTimer 
		//altrimenti al termine del timer potrebbe cambiare PatreolTarget e rimettersi a pattugliare
		ClearPatrolTimer();
		CombatTarget = SeenPawn;

		StartChasing();
		UE_LOG(LogTemp, Warning, TEXT("Pawn Seen Start Chasing"));
	}
}
