#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MyPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

// Load assets from Content folder by path (only used if not assigned in Class Defaults)

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys =
			LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Default)
			{
				Subsys->AddMappingContext(IMC_Default, 0);
			}
		}
	}
}
void AMyPlayerController::OnMoveTriggered(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow,
		FString::Printf(TEXT("Move: %.2f"), Axis));

	if (AMyPlayerCharacter* P = Cast<AMyPlayerCharacter>(GetPawn()))
	{
		P->Input_Move(Axis);
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	BindEnhancedInput();
}

void AMyPlayerController::BindEnhancedInput()
{
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_Move)
		{
			EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyPlayerController::OnMoveTriggered);
			EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &AMyPlayerController::OnMoveCompleted);
		}
		if (IA_Jump)
		{
			EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyPlayerController::OnJumpStarted);
			EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AMyPlayerController::OnJumpCompleted);
		}
	}
}

// ------------ Handlers ------------


void AMyPlayerController::OnMoveCompleted(const FInputActionValue& /*Value*/)
{
	if (AMyPlayerCharacter* P = Cast<AMyPlayerCharacter>(GetPawn()))
	{
		P->Input_Move(0.f);
	}
}

void AMyPlayerController::OnJumpStarted(const FInputActionValue& /*Value*/)
{
	if (AMyPlayerCharacter* P = Cast<AMyPlayerCharacter>(GetPawn()))
	{
		P->Input_JumpPressed();
	}
}

void AMyPlayerController::OnJumpCompleted(const FInputActionValue& /*Value*/)
{
	if (AMyPlayerCharacter* P = Cast<AMyPlayerCharacter>(GetPawn()))
	{
		P->Input_JumpReleased();
	}
}
