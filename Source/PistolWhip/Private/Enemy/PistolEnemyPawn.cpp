// 2021 github.com/EugeneTel/PistolWhip-UE4


#include "Enemy/PistolEnemyPawn.h"

#include "Log.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/PistolWeapon_Projectile.h"

APistolEnemyPawn::APistolEnemyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("Capsule"));
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	RootComponent = CapsuleComponent;

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor(150, 200, 255);
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SetupAttachment(CapsuleComponent);
		ArrowComponent->bIsScreenSizeScaled = true;
	}
#endif // WITH_EDITORONLY_DATA

	Mesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	if (Mesh)
	{
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Mesh->SetupAttachment(CapsuleComponent);
		static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
		Mesh->SetCollisionProfileName(MeshCollisionProfileName);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCanEverAffectNavigation(false);
	}

	// defaults
	bAlive = true;
	DestroyDelay = 2.0f;

}

float APistolEnemyPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	Death();
	
	return DamageAmount;
}

void APistolEnemyPawn::InitEnemyConfig(FEnemyData& InEnemyConfig)
{
	EnemyConfig = InEnemyConfig;

	SetMovementType(EnemyConfig.MovementType);
	if (EnemyConfig.SplineComponent)
	{
		SetSplineComponent(EnemyConfig.SplineComponent);
	}
}

bool APistolEnemyPawn::IsFiringEnabled() const
{
	return EnemyConfig.bFiringEnabled;
}

void APistolEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
}

void APistolEnemyPawn::Death_Implementation()
{
	bAlive = false;
	Mesh->SetCollisionProfileName(FName("Ragdoll"));
	Mesh->SetSimulatePhysics(true);
	CapsuleComponent->SetCollisionProfileName(FName("NoCollision"));

	if (Weapon)
	{
		Weapon->Destroy();
	}

	GetWorldTimerManager().SetTimer(TimerHandle_Destroy, this, &APistolEnemyPawn::DestroyEnemy, DestroyDelay, false);
}

void APistolEnemyPawn::DestroyEnemy()
{
	Destroy();
}

void APistolEnemyPawn::EquipWeapon()
{
	if (GetWorld() && EnemyConfig.WeaponClass && bAlive)
	{
		// spawn weapon
		Weapon = Cast<APistolWeapon>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), EnemyConfig.WeaponClass, FTransform()));
		if (Weapon)
		{
			Weapon->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
			Weapon->EquippedBy(this);
			Weapon->SetActorRelativeLocation(FVector(0.0f, 2.0f, 3.0f));
			Weapon->SetActorRelativeRotation(FRotator(30.0f, -180.0f, 2.0f));

			UGameplayStatics::FinishSpawningActor(Weapon, FTransform());

			// delegate
			if (OnWeaponEquipped.IsBound())
			{
				OnWeaponEquipped.Broadcast();
			}
		}
	}
}