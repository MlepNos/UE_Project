#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyPlayerCharacter.generated.h"

class UPaperFlipbook;
class UPaperFlipbookComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class AMyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyPlayerCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input (called from controller)
	void Input_Move(float Value);
	void Input_JumpPressed();
	void Input_JumpReleased();

protected:
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere)
	UPaperFlipbookComponent* Sprite;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	// Flipbooks (assign on the placed actor OR load in C++ later)
	UPROPERTY(EditAnywhere, Category = "Animation")
	UPaperFlipbook* IdleFlipbook;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UPaperFlipbook* RunFlipbook;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UPaperFlipbook* JumpStartFlipbook;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UPaperFlipbook* FallFlipbook;

	// Movement
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxRunSpeed = 700.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AirControl = 0.65f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float JumpZ = 700.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float GravityScale = 2.2f;

	// Camera
	UPROPERTY(EditAnywhere, Category = "Camera")
	bool bUseOrthographic = true;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "512.0"))
	float OrthoWidth = 2048.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraZOffset = 250.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraLag = 12.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float LookAheadDistance = 350.f;

private:
	void UpdateAnimation();
	void UpdateSpriteFacing(float MoveInput);

	float MoveAxis = 0.f;
	FVector LookAheadTarget;
};
