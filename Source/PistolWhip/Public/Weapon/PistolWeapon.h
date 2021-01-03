// 2021 github.com/EugeneTel/PistolWhip-UE4

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PistolWeapon.generated.h"

UCLASS(Abstract, NotBlueprintable)
class PISTOLWHIP_API APistolWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	APistolWeapon();

	virtual void Tick(float DeltaTime) override;
	
	virtual void StartFire();

	void SetOwningPawn(APawn* NewOwner);

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	APawn* OwnerPawn;
		
	/** Enable debug mode */
	UPROPERTY(Category=Weapon, EditAnywhere)
	bool bDebug = false;

	UPROPERTY(Category=Weapon, VisibleAnywhere)
	class USceneComponent* SceneComponent;

	UPROPERTY(Category=Weapon, VisibleAnywhere)
	class UBoxComponent* BoxComponent;

	/** Weapon mesh  */
	UPROPERTY(Category=Weapon, VisibleAnywhere)
	USkeletalMeshComponent* MeshComponent;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	FName MuzzleName = FName("Muzzle");

	/** fire animation */
	UPROPERTY(Category=Animation, EditDefaultsOnly)
	UAnimationAsset* FireAnim;

#if WITH_EDITORONLY_DATA
	/** Component shown in the editor only to indicate weapon facing */
	UPROPERTY()
	class UArrowComponent* ArrowComponent;
#endif

	virtual bool CanFire() const;
	
	/** play weapon animations */
	void PlayWeaponAnimation(UAnimationAsset* Animation);

	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	/** weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(APistolWeapon::FireWeapon,);

	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetMuzzleDirection() const;

};
