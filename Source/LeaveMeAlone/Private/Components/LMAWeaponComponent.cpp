// LeaveMeAlone Game by Netologiya. All RightsReserved.

#include "Components/LMAWeaponComponent.h"
#include "Animations/LMAReloadFinishedAnimNotify.h"
#include "GameFramework/Character.h"
#include "Weapon/LMABaseWeapon.h"

ULMAWeaponComponent::ULMAWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULMAWeaponComponent::StartFire()
{
	if (IsValid(Weapon) && !AnimReloading) {
		Weapon->StartFire();
	}
}

void ULMAWeaponComponent::StopFire()
{
	if (IsValid(Weapon) && !AnimReloading)
	{
		Weapon->StopFire();
	}
}

void ULMAWeaponComponent::Reload()
{
	CallBackReload();
}

void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	InitAnimNotify();
}

void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULMAWeaponComponent::SpawnWeapon()
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);
	if (IsValid(Weapon)) {
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character) {
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");
		}
	}

	Weapon->OutOfAmmo.AddUObject(this, &ULMAWeaponComponent::CallBackChangeClip);
}

void ULMAWeaponComponent::InitAnimNotify()
{
	if (!ReloadMontage) {
		return;
	}

	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents) {
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish) {
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);
			break;
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh)
{
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh) {
		AnimReloading = false;
	}
}

bool ULMAWeaponComponent::CanReload() const
{
	if (!(Weapon->IsCurrentClipFull()))
	{
		return !AnimReloading;
	}
	return AnimReloading;
}

void ULMAWeaponComponent::CallBackChangeClip()
{
	CallBackReload();
}

void ULMAWeaponComponent::CallBackReload()
{
	if (!CanReload())
	{
		return;
	}

	Weapon->ChangeClip();
	AnimReloading = true;
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	Character->PlayAnimMontage(ReloadMontage);
}
