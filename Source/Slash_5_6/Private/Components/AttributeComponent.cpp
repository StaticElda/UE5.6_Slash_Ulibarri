#include "Components/AttributeComponent.h"


UAttributeComponent::UAttributeComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}



void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	//Clamp ti permette di porre dei limiti ad un valore (in questo caso Health-Damage non può scendere sotto lo 0 
	// e non può salire sopra MaxHealth)
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

float UAttributeComponent::GetHealthPercent()
{

	return Health / MaxHealth;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

bool UAttributeComponent::IsAlive() 
{
	return Health > 0.f;
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
}

void UAttributeComponent::AddExp(int32 AmountOfExp)
{
	Exp += AmountOfExp;
}
