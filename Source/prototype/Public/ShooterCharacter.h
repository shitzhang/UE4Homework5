// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"


UENUM(BlueprintType)
enum class EPlayerPose : uint8
{
	RiflePose = 0,

	PistolPose = 1,

	KatanaPose = 2,

	NoWeaponPose,

};

UENUM()
enum class EInventorySlot : uint8
{
	/* For currently equipped items/weapons */
	Hands,

	/* For primary weapons on spine bone */
	Primary,

	/* Storage for small items like pistol on pelvis */
	Secondary,

	/* Katana */
	Katana
};

class UCameraComponent;
class USpringArmComponent;
class AShooterWeapon;
class UShooterHealthComponent;
class AShooterUsableActor;
class USoundCue;

UCLASS()
class PROTOTYPE_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	/* Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PawnClientRestart() override;

	/* Stop playing all montages */
	void StopAllAnimMontages();

	FRotator ControllerRotationBeforeFreelook;

public:
	// Sets default values for this character's properties
	AShooterCharacter(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, Category = "PlayerPose")
	EPlayerPose PlayerPose;

	void DeterminPlayerPose();

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* PunchAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundTakeHit;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundDeath;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsFiring() const;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsInitiatedJump() const;

	// Free the camera when holding left Alt.
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFreelooking;

	/* Check if pawn is allowed to fire weapon */
	bool CanFire() const;

	bool CanReload() const;

	float GetTargetingSpeedModifier() const;
	float GetSprintingSpeedModifier() const;

	/* Retrieve Pitch/Yaw from current camera */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

	FORCEINLINE UCameraComponent* GetCameraComponent()
	{
		return CameraComp;
	}

	/************************************************************************/
	/* Movement                                                             */
	/************************************************************************/

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginFreelook();
	void EndFreelook();

	// input
	void BeginCrouch();
	void EndCrouch();


	void SetSprinting(bool NewSprinting);

	/* Server side call to update actual sprint state */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool NewSprinting);
	void ServerSetSprinting_Implementation(bool NewSprinting);
	bool ServerSetSprinting_Validate(bool NewSprinting);

	// input
	void BeginSprint();
	void EndSprint();


	void SetIsJumping(bool NewJumping);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetIsJumping(bool NewJumping);
	void ServerSetIsJumping_Implementation(bool NewJumping);
	bool ServerSetIsJumping_Validate(bool NewJumping);

	void BeginJump();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	void SetTargeting(bool NewTargeting);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTargeting(bool NewTargeting);
	void ServerSetTargeting_Implementation(bool NewTargeting);
	bool ServerSetTargeting_Validate(bool NewTargeting);

	// input
	void BeginTarget();
	void EndTarget();


	/************************************************************************/
	/* Object Interaction                                                   */
	/************************************************************************/

	/* Use the usable actor currently in focus, if any */
	virtual void Use();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUse();
	void ServerUse_Implementation();
	bool ServerUse_Validate();

	AShooterUsableActor* GetUsableInView() const;

	/*Max distance to use/focus on actors. */
	UPROPERTY(EditDefaultsOnly, Category = "ObjectInteraction")
	float MaxUseDistance;

	/* True only in first frame when focused on a new usable actor. */
	bool bHasNewFocus;

	AShooterUsableActor* FocusedUsableActor;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;

	/* Character wants to run, checked during Tick to see if allowed */
	UPROPERTY(Transient, Replicated)
	bool bWantsToRun;

	UPROPERTY(Transient, Replicated)
	bool bIsTargeting;

	/* Is character currently performing a jump action. Resets on landed.  */
	UPROPERTY(Transient, Replicated)
	bool bIsJumping;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UShooterHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100));
	float ZoomInterpSpeed;

	// Default FOV set during begin play
	float DefaultFOV;

	UFUNCTION()
	void OnHealthChanged(UShooterHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

protected:

	//UPROPERTY(Replicated)
	//AShooterWeapon* CurrentWeapon;

	//UPROPERTY(EditDefaultsOnly, Category = "Player")
	//TSubclassOf<AShooterWeapon> StarterWeaponClass;

	/* Attachpoint for active weapon/item in hands */
	UPROPERTY(VisibleDefaultsOnly, Category = "Sockets")
	FName WeaponAttachSocketName;

	/* Attachpoint for items carried on the belt/pelvis. */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName PelvisAttachSocketName;

	/* Attachpoint for primary weapons */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName SpineAttachSocketName;

	/* Attachpoint for secondary weapons */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName SpineAttachSecondarySocketName;

	/* Distance away from character when dropping inventory items. */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float DropWeaponMaxDistance;

public:

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void NextWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void PrevWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void EquipPrimaryWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void EquipSecondaryWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void CloseWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void PickupWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerPickupWeapon();
	void ServerPickupWeapon_Implementation();
	bool ServerPickupWeapon_Validate();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void DropWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerDropWeapon();
	void ServerDropWeapon_Implementation();
	bool ServerDropWeapon_Validate();

	void EquipWeapon(AShooterWeapon* Weapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipWeapon(AShooterWeapon* Weapon);
	void ServerEquipWeapon_Implementation(AShooterWeapon* Weapon);
	bool ServerEquipWeapon_Validate(AShooterWeapon* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	AShooterWeapon* GetCurrentWeapon() const;

	void SetCurrentWeapon(AShooterWeapon* newWeapon, AShooterWeapon* LastWeapon = nullptr);


	/* All weapons/items the player currently holds */
	UPROPERTY(Transient, Replicated)
	TArray<AShooterWeapon*> Inventory;

	/* Check if the specified slot is available, limited to one item per type (primary, secondary) */
	bool WeaponSlotAvailable(EInventorySlot CheckSlot);

	/* Return socket name for attachments (to match the socket in the character skeleton) */
	FName GetInventoryAttachPoint(EInventorySlot Slot) const;

	void DestroyInventory();

	/* OnRep functions can use a parameter to hold the previous value of the variable. Very useful when you need to handle UnEquip etc. */
	UFUNCTION()
	void OnRep_CurrentWeapon(AShooterWeapon* LastWeapon);

	void AddWeapon(AShooterWeapon* Weapon);

	void RemoveWeapon(AShooterWeapon* Weapon, bool bDestroy);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	AShooterWeapon* CurrentWeapon;

	UPROPERTY()
	AShooterWeapon* PreviousWeapon;

	/* Update the weapon mesh to the newly equipped weapon, this is triggered during an anim montage.
	   NOTE: Requires an AnimNotify created in the Equip animation to tell us when to swap the meshes. */
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SwapToNewWeaponMesh();

	void Punch();

};


