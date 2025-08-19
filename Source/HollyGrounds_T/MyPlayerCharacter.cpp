#include "MyPlayerCharacter.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Capsule size (thin-ish for 2D)
	GetCapsuleComponent()->InitCapsuleSize(16.f, 48.f);

	// Don’t use controller rotations
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 2D plane constraint (lock to Y=0 plane)
	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bConstrainToPlane = true;
	Move->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f)); // normal points “out of screen”
	Move->bUseFlatBaseForFloorChecks = true;

	// Movement tuning
	Move->MaxWalkSpeed = MaxRunSpeed;
	Move->AirControl = AirControl;
	Move->JumpZVelocity = JumpZ;
	Move->GravityScale = GravityScale;
	Move->BrakingDecelerationWalking = 2048.f;
	Move->GroundFriction = 8.f;
	Move->bOrientRotationToMovement = false; // we flip sprite, not actor

	// Sprite (Paper2D)
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(RootComponent);
	Sprite->SetRelativeLocation(FVector(0.f, 0.f, -48.f));
	Sprite->SetLooping(true);

	// Spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = CameraLag;
	SpringArm->TargetArmLength = 0.f;
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(0.f, -90.f, 0.f)); // look along -Y
	SpringArm->SocketOffset = FVector(0.f, 0.f, CameraZOffset);

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	if (bUseOrthographic)
	{
		Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
		Camera->OrthoWidth = OrthoWidth;
	}
	else
	{
		Camera->ProjectionMode = ECameraProjectionMode::Perspective;
		Camera->FieldOfView = 25.f;
	}
}

void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	LookAheadTarget = GetActorLocation();
}

void AMyPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Horizontal accel towards desired speed
	const float DesiredSpeed = MoveAxis * MaxRunSpeed;
	const FVector Vel = GetVelocity();
	const float NewX = FMath::FInterpTo(Vel.X, DesiredSpeed, DeltaSeconds, 12.f);
	GetCharacterMovement()->Velocity.X = NewX;

	UpdateAnimation();
	UpdateSpriteFacing(MoveAxis);

	// Camera look-ahead
	FVector Loc = GetActorLocation();
	const float Dir = (MoveAxis != 0.f) ? FMath::Sign(MoveAxis) : FMath::Sign(Vel.X);
	LookAheadTarget = Loc + FVector(Dir * LookAheadDistance, 0.f, 0.f);

	const FVector ArmLoc = SpringArm->GetComponentLocation();
	const FVector Smoothed = FMath::VInterpTo(ArmLoc, LookAheadTarget + FVector(0, 0, CameraZOffset), DeltaSeconds, CameraLag);
	const FVector Delta = Smoothed - ArmLoc;

	FVector Sock = SpringArm->SocketOffset;
	Sock.X = FMath::Clamp(Sock.X + Delta.X, -LookAheadDistance, LookAheadDistance);
	SpringArm->SocketOffset = FVector(Sock.X, 0.f, CameraZOffset);
}

void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Legacy binds as a fallback (if you also set Project Settings → Input)
	PlayerInputComponent->BindAxis(TEXT("Move"), this, &AMyPlayerCharacter::Input_Move);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMyPlayerCharacter::Input_JumpPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AMyPlayerCharacter::Input_JumpReleased);
}

void AMyPlayerCharacter::Input_Move(float Value)
{
	MoveAxis = FMath::Clamp(Value, -1.f, 1.f);
}

void AMyPlayerCharacter::Input_JumpPressed()
{
	Jump();
}

void AMyPlayerCharacter::Input_JumpReleased()
{
	StopJumping();
}

void AMyPlayerCharacter::UpdateAnimation()
{
	const FVector Vel = GetVelocity();
	const bool bIsFalling = GetCharacterMovement()->IsFalling();
	UPaperFlipbook* Desired = IdleFlipbook;

	if (bIsFalling)
	{
		Desired = (Vel.Z > 0.f) ? JumpStartFlipbook : FallFlipbook;
	}
	else if (FMath::Abs(Vel.X) > 5.f)
	{
		Desired = RunFlipbook;
	}

	if (Sprite && Desired && Sprite->GetFlipbook() != Desired)
	{
		Sprite->SetFlipbook(Desired);
	}
}

void AMyPlayerCharacter::UpdateSpriteFacing(float MoveInput)
{
	if (MoveInput > 0.05f)
	{
		Sprite->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	}
	else if (MoveInput < -0.05f)
	{
		Sprite->SetRelativeScale3D(FVector(-1.f, 1.f, 1.f));
	}
}
