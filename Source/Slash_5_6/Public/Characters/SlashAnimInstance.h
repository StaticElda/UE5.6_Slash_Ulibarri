#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharactersStatusTypes.h"
#include "SlashAnimInstance.generated.h"

//Forward declaration
class ASlashCharacter ;
class UCharacterMovementComponent;

UCLASS()
class SLASH_5_6_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	//Event Blueprint Initialize Animation viene eseguita appena il codice viene compilato,
	//quindi anche prima del play. (E' un po come la BeginPlay ma funziona anche fuori dal PlayMode)
	//La sua corrispondente in C++ :
	virtual void NativeInitializeAnimation() override;

	//Event Blueprint Update Animation viene eseguita ad ogni frame anche fuori dalla PlayMode
	//è un pò come una Tick.
	//La sua corrispondente in C++ : 
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	//Dichiariamo le variabili che ci serviranno per gestire le transizioni fra stati
	//La dichiariamo BlueprintReadWrite in modo da poterla leggere e modificare nell'EventGraph.
	UPROPERTY(BlueprintReadWrite)
	ASlashCharacter* SlashCharacter;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	UCharacterMovementComponent* SlashCharacterMovement;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool IsFalling;

	//Puoi usare | in Category per mettere la property in una Sottocategoria
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = "Action State")
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeadPose> DeadPose;

};
