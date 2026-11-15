#include "Items/Public/Weapon/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());

	//Andiamo ad abilitare le collisioni sulla CollisionBox e lo mettiamo a QueryOnly
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//Andiamo a settare la risposta alla collisione per i vari tipi
	//Ci sono varie funzioni per farlo:

	//Setta la risposta a tutti i canali
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//Setta la risposta ad un solo canale
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//Ignoriamo il Pawn in modo che la collisionBox della spada non reagisca al Personaggio che la impugna

	//Inizializziamo le property e attacchiamo le components al root
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	//Facciamo il Binding fra Delegato e funzione di CallBack
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	WeaponBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnBoxEndOverlap);

}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	{

		//Settiamo ItemState ad Equiped
		ItemState = EItemState::EIS_Equipped;

		//La classe Actor ha una Property Owner e Instigator (Instigator deve contenere un Pawn)
		SetOwner(NewOwner);
		SetInstigator(NewInstigator);

		AttachMeshToSocket(InParent, InSocketName);

		DisableShereCollision();
		PlayEquipSound();
		DeactivateEmbers();
		
	}
}

void AWeapon::ActivateEmbers()
{
	if (ItemEffect)
	{
		//Deactivate e Activate (Fanno quello che immagini sul Component)
		ItemEffect->Activate();
	}
}

void AWeapon::DeactivateEmbers()
{
	//Controlliamo che EmbersEffect di Weapon non sia un nullptr (cioè che sia stato settato lato UE)
	if (ItemEffect)
	{
		//Deactivate e Activate (Fanno quello che immagini sul Component)
		ItemEffect->Deactivate();
	}
}

void AWeapon::EnableSphereCollision()
{
	//Abilitiamo le collisioni sulla SphereComponent di Weapon
	if (SphereComp)
	{
		SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

void AWeapon::DisableShereCollision()
{
	//Disabilitiamo le collisioni sulla SphereComponent di Weapon
	if (SphereComp)
	{
		SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	
}

void AWeapon::PlayEquipSound()
{
	//Riproduciamo il suono nella Property EquipSound
	if (EquipSound)
	{
		//UGamePlayStatics è un contenitore di funzioni statiche, quindi non va istanziato
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AWeapon::DropOnFloor()
{
	
	ItemState = EItemState::EIS_OnTheFloor;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(Owner->GetActorLocation());

	SetOwner(nullptr);
	SetInstigator(nullptr);

	EnableSphereCollision();

	ActivateEmbers();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor))
	{
		return;
	}
	if (GetOwner()->ActorHasTag(TEXT("EngageableTarget")) && OtherActor->ActorHasTag(TEXT("EngageableTarget"))) 
	{
		return; 
	}

	FHitResult BoxHit;
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*>ActorsToIgnore;
	ActorsToIgnore.Add(this);

	//Aggiungiamo gli actor in IgnoreActors a ActorsToIgnore
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
	);

	//AddUnique controlla che l'elemento che stai aggiungendo non sia già stato 
		//aggiunto all'array
	

	if (OtherActor && !IgnoreActors.Contains(OtherActor))
	{
		UGameplayStatics::ApplyDamage(
			//Il bersaglio che riceve i danni
			OtherActor,
			//Quanti danni vengono inflitti
			Damage,
			//Il Controller di chi provoca i danni 
			GetInstigator()->GetController(),
			//L'Actor che infligge i danni
			this,
			//Il tipo di danni, c'è una struct apposta, volendo puoi creare le classi con i tipi di danno 
			UDamageType::StaticClass()
		);

		
		ExecuteGetHit(BoxHit, OtherActor);
		CreateFields(BoxHit.ImpactPoint);

		IgnoreActors.AddUnique(OtherActor);
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit, AActor* OtherActor)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(OtherActor, BoxHit.ImpactPoint, GetOwner());
	}
}

void AWeapon::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
