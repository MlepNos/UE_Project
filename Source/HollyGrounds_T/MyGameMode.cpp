#include "MyGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerController.h" 
AMyGameMode::AMyGameMode()
{
    // Use the Blueprint pawn
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
        TEXT("/Game/BP_PlayerCharacter") // no _C here
    );
    if (PlayerPawnBPClass.Succeeded())
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(
        TEXT("/Game/BP_MyPlayerController") // path to your BP
    );

    if (PlayerControllerBPClass.Succeeded())
    {
        PlayerControllerClass = PlayerControllerBPClass.Class;
    }
}
