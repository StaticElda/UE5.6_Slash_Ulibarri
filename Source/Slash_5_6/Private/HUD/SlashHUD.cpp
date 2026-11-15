#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	// Usiamo BeginPlay per creare il Widget possiamo farlo mediante la sua FactoryFunction CreateWidget
	// CreateWidget vuole in template il tipo del Widget (USlashOverlay)
	// Come argomenti : la OwnerClass (PlayerController), la classe dell'UserWidget (SlashOverlayClass)
	//
	
	UWorld* World = GetWorld();
	if (World) 
	{
		// Abbiamo un solo PlayerController nel gioco, quindi prendiamo il primo
		APlayerController* Controller = World->GetFirstPlayerController();
		
		if (Controller && SlashOverlayClass) 
		{
			//Creiamo il Widget
			SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);

			// Aggiungiamo il Widget al Viewport
			SlashOverlay->AddToViewport();

			// Da notare che così il Widget non è pi legato al livello ma al PlayerController che possiede il Pawn
		}
	}
	
}
