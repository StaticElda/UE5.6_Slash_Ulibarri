#include "Pawns/Bird.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"


//Aggiungiamo gli include necessari, possiamo trovarli nella documentazione :
//https://dev.epicgames.com/documentation/en-us/unreal-engine/using-spring-arm-components-in-unreal-engine

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"




ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));

	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(RootComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//Creiamo il Subobject e attacchiamolo al RootComponent
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	//Settiamo la lunghezza dello SpringArm
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);


}


void ABird::BeginPlay()
{
	Super::BeginPlay();
	

}


void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//Facciamo il binding con l'AxisMapping
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABird::MoveForward);
	
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABird::Turn);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABird::LookUp);
	
}

//Definisco la funzione con cui fare il Binding
void ABird::MoveForward(float InputParameter)
{
	//Controlliamo che l'InputParameter non sia 0, cioè che stiamo premendo W
	//e che il controller non sia nullptr, 
	// tutti i pawn hanno un puntatore Controller che punta il loro controllore
	if ((Controller) && (InputParameter != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		//La funzione AddMovementInput richiede come parametro:
		// una direzione (FVector)
		// un valore (float)
		// un true se si intende usare la forza (bool)(Opzionale a false)
		AddMovementInput(Forward, InputParameter, false);

	}
}
void ABird::Turn(float  InputParameter) 
{
	if ((Controller) && (InputParameter != 0.f)) 
	{
		AddControllerYawInput(InputParameter);
	}
}

void ABird::LookUp(float  InputParameter) 
{
	if ((Controller) && (InputParameter != 0.f))
	{
		AddControllerPitchInput(InputParameter);
	}
}
