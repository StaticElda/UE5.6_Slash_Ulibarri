#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

UCLASS()
class SLASH_5_6_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetExp(int32 Exp);

private:

	// Attenzione !
	// Il bind funziona solo se l'oggeto nel blueprint e il nome della property sono uguali
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldCountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ExpCountText;



};
