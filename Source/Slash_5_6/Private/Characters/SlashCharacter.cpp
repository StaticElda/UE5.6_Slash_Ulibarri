#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "GroomComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"

#include "Items/Public/Item.h"
#include "Items/Public/Weapon/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Items/Public/Weapon/WeaponsTypes.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Public/Exp.h"
#include "Items/Public/Treasure/Treasure.h"



ASlashCharacter::ASlashCharacter()
{
 	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	
	GetCharacterMovement() -> bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	//Collisioni con le armi dei nemici
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	//Creiamo il subobject 
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	//Attacchiamo la component Hair alla Component Mesh
	//GetMesh è una funzione di Actor e ritorna la Mesh
	Hair->SetupAttachment(GetMesh());
	//Diamo un nome all'Attachement, è necessario per far funzionare l'Attachement.
	Hair->AttachmentName = FString("head");

	//Facciamo la stessa cosa con le sopracciglia
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Movimento

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASlashCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ASlashCharacter::LookUp);


	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ASlashCharacter::Jump);

	//Faccio il Bind
	PlayerInputComponent->BindAction(TEXT("E_Pressed"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("R_Pressed"), IE_Pressed, this, &ASlashCharacter::RKeyPressed);
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Diamo all'oggetto SlashCharacter una Tag
	Tags.Add(FName("EngageableTarget"));

	// SETTIAMO L'HUD
	InitializeSlashOverlay();

}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (!IsDead()) 
	{
		ActionState = EActionState::EAS_HitReaction;
		Super::GetHit_Implementation(ImpactPoint, Hitter);
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Commenta il contenuto di questa funzione per rendere SlashCharacter Immortale
	HandleDamage(DamageAmount);

	SetHUDHealth();

	return DamageAmount;
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddExp(AExp* Exp)
{
	if (Attributes && SlashOverlay) 
	{
		Attributes->AddExp(Exp->GetExps());
		SlashOverlay->SetExp(Attributes->GetExp());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay) 
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetGold(0.f);
				SlashOverlay->SetExp(0.f);
			}
		}
	}
}


void ASlashCharacter::SetHUDHealth()
{
	//Andiamo ad aggiornare la HealthBar nell'HUD dopo che SlashCharacter riceve danni.
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}


void ASlashCharacter::MoveForward(float Value) 
{
	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Emerald, FString("MoveForward called"), true);
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return ;
	}
	if ((Controller) && (Value != 0.f))
	{
		//Vogliamo ottenere il ForwardVector del Controller

		//Otteniamo il vettore Rotation del Controller
		const FRotator ControlRotation = GetControlRotation();
		// prendiamo solo la rotazione attorno all'asse Z
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		//UE ha classi e metodi apposta per usare le matrici di rotazione
		//Diamo alla matrice di rotazione 3D solo l'angolo di rotazione attorno all'asse Z
		//Usiamo i suoi metodi per ricavare il vettore relativo all'asse X ruotato.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//Facciamo muovere il personaggio
		AddMovementInput(Direction, Value, false);
	}
}

void ASlashCharacter::Turn(float Value) 
{
	if ((Controller) && (Value != 0.f))
	{
		AddControllerYawInput(Value);
	}
}

void ASlashCharacter::LookUp(float Value) 
{
	if ((Controller) && (Value != 0.f))
	{
		AddControllerPitchInput(Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return;
	}
	if ((Controller) && (Value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		// prendiamo solo la rotazione attorno all'asse Z
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		//Diamo alla matrice di rotazione 3D solo l'angolo di rotazione attorno all'asse Z
		//Usiamo i suoi metodi per ricavare il vettore relativo all'asse Y ruotato.
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Facciamo muovere il personaggio
		AddMovementInput(Right, Value, false);
		//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Emerald, FString("MoveRight called"), true);
	}
}

void ASlashCharacter::Jump() 
{
	if (!IsUnoccupied())
	{
		return;
	}
	Super::Jump() ;
	
	ActionState = EActionState::EAS_Jumping;

}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::EKeyPressed()
{
	if (IsUnoccupied()) 
	{
		AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
		if (OverlappingWeapon)
		{
			if (OverlappingWeapon->GetItemState() == EItemState::EIS_OnTheFloor)
			{
				//Se possiede già un arma la lascia a terra
				if (OwnedWeapon)
				{
					//Mettiamo l'arma a terra
					OwnedWeapon->DropOnFloor();
				}

				OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
				SetCharacterState(OverlappingWeapon);
				OwnedWeapon = OverlappingWeapon;

			}
		}
	}
	
}

//La usiamo per equipaggiare o disequipaggiare il personaggio dell'arma
void ASlashCharacter::RKeyPressed()
{
	//Se non sta saltando o facendo altra roba e Equip_UnequipMontage non è nullptr
	if (ActionState == EActionState::EAS_Unoccupied && Equip_UnequipMontage && OwnedWeapon) 
	{	
		FString EquipSectionName = "";
		FString UnequipSectionName = "";

		switch (OwnedWeapon->GetWeaponType())
		{
		case EWeaponsType::EWT_OneHanded :
			EquipSectionName = "Equip";
			UnequipSectionName = "Unequip";
		case EWeaponsType::EWT_TwoHanded : 
			EquipSectionName = "2HandEquip";
			UnequipSectionName = "2HandUnequip";
		}
		//Se è disarmato Equipaggiamo l'Arma
		if (CharacterState == ECharacterState::ECS_Unequipped)
		{
			SetCharacterState(OwnedWeapon);
			PlayEquip_UnequipMontage(FName(EquipSectionName));

			ActionState = EActionState::EAS_EquippingWeapon;
		}
		//Se ha l'arma in pugno lo disequipaggiamo
		else if (CharacterState != ECharacterState::ECS_Unequipped)
		{
			PlayEquip_UnequipMontage(FName(UnequipSectionName));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::Attack() 
{
	//Eseguo il codice di Attack nella versione della classe Padre(BaseCharacter)
	Super::Attack();

	//Facciamo si che il personaggio possa attaccare solo se armato
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return 
		ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped ;
}


int32 ASlashCharacter::PlayAttackMontage()
{
	//Per fare il jump ad una Section Casuale calcoliamo un numero casuale tra 0 e 1
	if (OwnedWeapon->GetWeaponType() == EWeaponsType::EWT_OneHanded)
	{
		if (OneHandedAttackSections.Num() <= 0) return -1;
		const int32 MaxSectionIndex = OneHandedAttackSections.Num() - 1;
		const int32 Selection1 = FMath::RandRange(0, MaxSectionIndex);
		PlayMontageSection(AttackMontage, OneHandedAttackSections[Selection1]);
		return Selection1;
	}
	else if (OwnedWeapon->GetWeaponType() == EWeaponsType::EWT_TwoHanded)
	{
		if (TwoHandedAttackSections.Num() <= 0) return -1;
		const int32 MaxSectionIndex = TwoHandedAttackSections.Num() - 1;
		const int32 Selection2 = FMath::RandRange(0, MaxSectionIndex);
		PlayMontageSection(AttackMontage, TwoHandedAttackSections[Selection2]);
		return Selection2;
	}
	return -1; // In questo caso il tipo di OwnedWeapon non è gestito
}

void ASlashCharacter::PlayEquip_UnequipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Equip_UnequipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, Equip_UnequipMontage);
	}
}

void ASlashCharacter::AttachWeaponInHand()
{
	if (OwnedWeapon)
	{
		//Andiamo ad attaccare la spada al Socket della mano
		OwnedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
	
}

void ASlashCharacter::AttachWeaponOnShoulder()
{
	if (OwnedWeapon) 
	{
		//Andiamo ad attaccare la spada al Socket della spalla
		OwnedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::HitReactEnd()
{
	if (!IsDead()) 
	{
		ActionState = EActionState::EAS_Unoccupied;
	}
	
}

void ASlashCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//Facciamo si che quando il personaggio atterra dopo un salto non possa muoversi per un breve periodo
	ActionState = EActionState::EAS_Landing;

	GetWorldTimerManager().SetTimer(
		LandingTimer,
		this,
		&ASlashCharacter::LandingTimerFinished,
		0.3f,
		false
	);

}


void ASlashCharacter::Die()
{
	Super::Die();
	
	ActionState = EActionState::EAS_Dead;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


bool ASlashCharacter::IsDead() 
{
	if (ActionState == EActionState::EAS_Dead) 
	{
		return true;
	}
	return false;
}


void ASlashCharacter::LandingTimerFinished()
{
	if (ActionState == EActionState::EAS_Dead)
	{
		return;
	}

	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetCharacterState(AWeapon* Weapon)
{
	switch (Weapon->GetWeaponType())
	{
	case EWeaponsType::EWT_OneHanded:
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		break;
	case EWeaponsType::EWT_TwoHanded:
		CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
		break;
	}

}




