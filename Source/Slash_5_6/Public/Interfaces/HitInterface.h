

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

//Un Interface ha due classi, questo per aiutare il Rreflection Sistem di Unreal

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_5_6_API IHitInterface
{
	GENERATED_BODY()


public:

	//Rendiamo GetHit un BlueprintNativeEvent
	UFUNCTION(BlueprintNativeEvent)
	void GetHit(const FVector& ImpactPoint, AActor* Hitter ); 
};
