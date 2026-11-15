#include "HUD/SlashOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar) 
	{
		//Usiamo il metodo SetPercent di UProgressBar
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetGold(int32 Gold)
{
	if (GoldCountText) 
	{
		// Abbiamo Gold che è un int32 dobbiamo convertirlo in un FText, lo faremo mediante un FString
		FText Text = FText::FromString(FString::Printf(TEXT(" % d"), Gold));
		GoldCountText->SetText(Text);
	}
}

void USlashOverlay::SetExp(int32 Exp)
{
	if (ExpCountText)
	{
		FText Text = FText::FromString(FString::Printf(TEXT(" % d"), Exp));
		ExpCountText->SetText(Text);
	}
}


