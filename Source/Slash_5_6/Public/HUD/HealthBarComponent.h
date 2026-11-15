#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_5_6_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public: 
	
	void SetHealthPercent(float Percent);

private :


	//Possiamo memorizzare il risultato del Cast in una property 
	//Gli diamo UPROPERTY() in modo che sia iscritto al GarbageCollector, 
	//così siamo sicuri che non punti a Garbage Data
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
};
