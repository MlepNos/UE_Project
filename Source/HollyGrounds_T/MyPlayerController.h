#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Assign these either in Class Defaults OR load by path in code
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_Default = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Move = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Jump = nullptr;

private:
	void LoadInputAssetsByPath();
	void BindEnhancedInput();

	// Handlers that Enhanced Input binds to
	UFUNCTION() void OnMoveTriggered(const FInputActionValue& Value);
	UFUNCTION() void OnMoveCompleted(const FInputActionValue& Value);
	UFUNCTION() void OnJumpStarted(const FInputActionValue& Value);
	UFUNCTION() void OnJumpCompleted(const FInputActionValue& Value);
};
