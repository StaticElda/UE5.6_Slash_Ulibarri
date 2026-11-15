#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	//Controlliamo se la property contiene un nullptr
	if (HealthBarWidget == nullptr)
	{
		//Ritorna il l'istanza Widget collegata a questa Component
		//Facciamo un cast per portarla al tipo corretto,  GetUserWidgetObject ritorna un ptr a UUserWidget
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	//Controlliamo che il cast abbia avuto successo
	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		//Accediamo alle property della ProgressBar
		//Puoi dare un'occhiata alla documentazione di ProgressBar per cercare metodi e properties
		//e anche l'include : #include "Components/ProgressBar.h"
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
