// LeaveMeAlone Game by Netologiya. All RightsReserved.

#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Components/LMAWeaponComponent.h"

ALMADefaultCharacter::ALMADefaultCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
}

void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (CursorMaterial) {
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);

	CurrentStamina = MaxStamina;
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!(HealthComponent->IsDead())) {
		RotationPlayerOnCursor();
	}

	if (IsRunning) {
		LossStamina();
	}
	else {
		RecoveryStamina();
	}
}

void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);

	PlayerInputComponent->BindAction("CameraMoveUp", IE_Pressed, this, &ALMADefaultCharacter::CameraMoveUp);
	PlayerInputComponent->BindAction("CameraMoveDown", IE_Pressed, this, &ALMADefaultCharacter::CameraMoveDown);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::StopSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Fire);
}

void ALMADefaultCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::CameraMoveUp()
{
	float TempArmLength = SpringArmComponent->TargetArmLength + ArmLengthSmooth;

	if (TempArmLength >= ArmLengthMax) {
		SpringArmComponent->TargetArmLength = ArmLengthMax;
	}
	else {
		SpringArmComponent->TargetArmLength = TempArmLength;
	}
}

void ALMADefaultCharacter::CameraMoveDown() {
	float TempArmLength = SpringArmComponent->TargetArmLength - ArmLengthSmooth;

	if (TempArmLength <= ArmLengthMin) {
		SpringArmComponent->TargetArmLength = ArmLengthMin;
	}
	else {
		SpringArmComponent->TargetArmLength = TempArmLength;
	}
}

void ALMADefaultCharacter::OnDeath() 
{
	CurrentCursor->DestroyRenderState_Concurrent();
	PlayAnimMontage(DeathMontage);
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(5.0f);
	if (Controller) {
		Controller->ChangeState(NAME_Spectating);
	}
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth) 
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::RotationPlayerOnCursor() 
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC) {
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor) {
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

void ALMADefaultCharacter::StartSprint()
{
	if (CurrentStamina > TickLossStamina) {
		GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
		IsRunning = true;
	}
}

void ALMADefaultCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	IsRunning = false;
}

void ALMADefaultCharacter::LossStamina()
{
	CurrentStamina -= TickLossStamina;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Stamina = %f"), CurrentStamina));

	if (CurrentStamina < TickLossStamina) {
		StopSprint();
	}
}

void ALMADefaultCharacter::RecoveryStamina()
{
	if (CurrentStamina < MaxStamina) {
		CurrentStamina += TickRecoveryStamina;
		if (CurrentStamina > MaxStamina) {
			CurrentStamina = MaxStamina;
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Stamina = %f"), CurrentStamina));
	}
}
