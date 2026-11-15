#include "Characters/BaseCharacter.h"
#include "Items/Public/Weapon/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/CharactersStatusTypes.h"


#include "Kismet/GameplayStatics.h"



ABaseCharacter::ABaseCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	//Andiamo ad inizializzare AttributeComponent che gestirà Health e MaxHelth
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	//Non avendo mesh o altri asset non serve attaccarlo ad altri components

	//Sistemiamo anche i collision presets della CapsuleComponent
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	/*
	Usiamo questa funzione per abilitare o disabilitare la collision della BoxTrace
	durante le animazioni di attacco dell'arma
	*/

	if (OwnedWeapon)
	{
		UBoxComponent* WeaponBox = OwnedWeapon->GetWeaponBox();
		if (WeaponBox)
		{
			WeaponBox->SetCollisionEnabled(CollisionEnabled);
			//Svuoto IgnoreActors 
			//in modo che venga svuotato all'inizio e alla fine di ogni Animazione di attacco
			OwnedWeapon->IgnoreActors.Empty();
		}
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	//Controlliamo che Enemy sia Vivo 
	if (IsAlive() && Hitter)
	{
		//In questo modo non posso ferire nessuno durante un HitReact che ha interrotto un attacco 
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

		DirectionalHitReact(Hitter->GetActorLocation());
	}
	//Se Enemy è Morto
	else
	{
		//Facciamo morire il personaggio
		Die();
	}

	//Eseguiamo HitSound
	PlayHitSound(ImpactPoint);
	//Emettiamo lo spruzzo di sangue
	PlayHitParticles(ImpactPoint);

}

void ABaseCharacter::Jump()
{
	Super::Jump();
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead"))) 
	{
		CombatTarget = nullptr;
	}
}

bool ABaseCharacter::IsAlive()
{
	return (Attributes && Attributes->IsAlive());
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);

	//Ora dobbiamo mettere in DeadPose l'Enum corrispondente
	//Per farlo useremo TEnumAsByte un Wrapper per gli Enum, 
	//sfruttando il fatto che gli Enum sono int in realtà, 
	TEnumAsByte<EDeadPose> Pose(Selection);
	//Controlliamo che Pose non sia out of range
	if (Pose < EDP_MAX)
	{
		DeadPose = Pose;
	}

	return Selection ;
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	//DrawDebugSphere(GetWorld(), ImpactPoint, 10.f, 12, FColor::Orange, false, 20.f);

	const FVector Forward = GetActorForwardVector();

	//Aggiusto la Z di ImpactPoint in modo da avere due vettori paralleli al terreno
	const FVector ImpactZFixed(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	//Normaliziamo ToHit usando GetSafeNormal 
	//in questo modo abbiamo un vettore normalizzato di modulo 1
	const FVector ToHit = (ImpactZFixed - GetActorLocation()).GetSafeNormal();

	//DotProduct è una static function di FVector
	const double Cos = FVector::DotProduct(Forward, ToHit);

	//Per usare l'arcos ci serve la libreria FMath
	double Angle = FMath::Acos(Cos);

	//Convertiamo da radianti a gradi 
	Angle = FMath::RadiansToDegrees(Angle);

	//Calcoliamo il prodotto vettoriale e ci è utile perchè
	//Se ToHit è a sinistra di Forwad
	//(dal puto di vista di Forward)) allora il vettore output sarà verso il basso
	//(Per la regola della mano sinistra) e 
	//Angle dovrebbe essere negativo
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 60.f, 5.f, FColor::Red, 20.f);

	if (CrossProduct.Z < 0)
	{
		//Rendiamo Angle negativo
		Angle *= -1;
	}

	//Stampa di Angle a schermo
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1,5.f,FColor::Cyan, FString::Printf(TEXT("Angle : %f"), Angle));
	}*/


	FName Section = "FromLeft";
	if (Angle >= -45.f && Angle < 45.f)
	{
		Section = "FromFront";
	}
	else if (Angle >= 45.f && Angle < 135.f)
	{
		Section = "FromRight";
	}
	else if (Angle >= 135 && Angle < 225.f)
	{
		Section = "FromBack";
	}

	//Include per disegnare le Debug Arrow per disgnare i vettori 
	//#include "Kismet/KismetSystemLibrary.h"
	/*UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation()+Forward*60.f, 5.f, FColor::Green, 20.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation()+ToHit*60.f, 5.f, FColor::Yellow, 20.f);
	*/

	//Facciamogli eseguire la HitReactMontage
	PlayHitReactMontage(Section);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::PlayHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* AnimationMontage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimationMontage)
	{
		AnimInstance->Montage_Play(AnimationMontage);
		AnimInstance->Montage_JumpToSection(SectionName, AnimationMontage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNamesArray)
{
	//Controlliamo che l'array con i nomi delle section non sia vuoto
	if (SectionNamesArray.Num() <= 0) return -1 ;
	const int32 MaxSectionIndex = SectionNamesArray.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNamesArray[Selection]);
	return Selection;
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) 
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}


FVector ABaseCharacter::GetTranslationWarpTarget()
{
	//Ritorna il punto in cui muoversi nel MotionWarping
	
	//Vogliamo che Enemy possa muoversi un pò in WarpMotion, ma non dritto sul personaggio,
	// gli diamo una certa distanza : WarpTargetDistance

	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	//Il vector tra combatTarget e il personaggio, normalizzato (quindi otteniamo la direzione)
	FVector TargetToMe = (CombatTargetLocation - Location);
	
	//Ricaviamo il versore WarpTargetDistance
	FVector Direzione = TargetToMe.GetSafeNormal();

	float Modulo = TargetToMe.Size();

	FVector Resp = Location + (Modulo - WarpTargetDistance) * Direzione;

	//DrawDebugSphere(GetWorld(), Resp, 50.f, 10, FColor::Yellow);
	
	return Resp;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	//Ritorna la direzione in cui ruotare nel MotionWarping
	if (CombatTarget) 
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
	
}


void ABaseCharacter::DisableCapsuleCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes) 
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::AttackEnd()
{
	
}

void ABaseCharacter::HitReactEnd()
{

}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

