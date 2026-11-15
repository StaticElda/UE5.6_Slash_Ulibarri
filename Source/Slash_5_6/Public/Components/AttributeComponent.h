#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_5_6_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual void BeginPlay() override;

private:

	//Current Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Exp;

public:

	void ReceiveDamage(float Damage);
	float GetHealthPercent();
	bool IsAlive();

	void AddGold(int32 AmountOfGold);
	void AddExp(int32 AmountOfExp);

	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetExp() const { return Exp; }
};
