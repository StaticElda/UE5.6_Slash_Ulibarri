#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_5_6_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//Creiamo una variabile che vogliamo collegare al Widget in WBP_HealthBar (in questo modo modificando uno modificheremo anche l'altro), 
	//per farlo usiamo UPROPERTY(meta = (BindWidget)),
	//ATTENZIONE : perchè funzioni la variabile deve chiamarsi come il Widget
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
};
