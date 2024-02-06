// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULMAHealthComponent;
class UAnimMontage;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALMADefaultCharacter();

	UFUNCTION()
	ULMAHealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;


	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ArmLengthMin = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ArmLengthMax = 2600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ArmLengthSmooth = 100.0f;


	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float MaxSprintSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float TickLossStamina = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float TickRecoveryStamina = 0.3f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float YRotation = -75.0f;
	float ArmLength = 1400.0f;
	float FOV = 55.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void CameraMoveUp();
	void CameraMoveDown();

	void OnDeath();
	void OnHealthChanged(float NewHealth);

	void RotationPlayerOnCursor();

	float WalkSpeed = 0.0f;
	float CurrentStamina = 0.0f;
	bool IsRunning = false;
	void StartSprint();
	void StopSprint();
	void LossStamina();
	void RecoveryStamina();
};
