#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

//Necessario per usare la funzione VectorLenghtXY che in c++ è VSizeXY
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	//Poichè è un override chiamiamo la super per ottenere il codice della versione del Padre
	Super::NativeInitializeAnimation();

	//Inizializiamo la nostra variabile SlashCharacter

	//UE ha le sue funzioni per fare il cast, funzionano similmente a static_cast<>()
	/*
	Cast<Tipo_a_cui_fare_il_cast>(Variabile_di_cui_fare_il_Cast)
	*/
	//Se il cast non riesce perchè il value non discende dal tipo il cast fallisce e ritorna Null
	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());	
	
	//Controlliamo che il Cast non sia fallito
	if (SlashCharacter) 
	{
		//Inizializiamo SlashCharacterMovement :
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();

	}


}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (SlashCharacterMovement) 
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);

		//Usiamo la funzione UE IsFalling che restituisce un bool (true, se il personaggio sta cadendo)
		IsFalling = SlashCharacterMovement->IsFalling();
		CharacterState = SlashCharacter->GetCharacterState();
		ActionState = SlashCharacter->GetActionState();
		DeadPose = SlashCharacter->GetDeadPose();
	}
}
