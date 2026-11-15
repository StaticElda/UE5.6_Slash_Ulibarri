#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

//ForwardDeclations
class USlashOverlay;

UCLASS()
class SLASH_5_6_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

public: 

	virtual void BeginPlay() override;

private: 

	//Damogli la UPROPERTY EditDefaultsOnly 
	// in modo da poterla settare da BP_SlashHUD 
	// che erediterà da questa classe c++
	UPROPERTY (EditDefaultsOnly, Category = "Slash")
	TSubclassOf<USlashOverlay> SlashOverlayClass;

	//Abbiamo già fatto la forward declaration sopra per USlashOverlay, quindi non serve rifarla
	UPROPERTY()
	USlashOverlay* SlashOverlay;

public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
	
};
