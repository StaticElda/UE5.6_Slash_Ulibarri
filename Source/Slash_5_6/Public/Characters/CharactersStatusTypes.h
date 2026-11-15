#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "EquippedOneHandedWeapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "EquippedTwoHandedWeapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Jumping UMETA(DisplayName = "Jumping"),
	EAS_Landing UMETA(DisplayName = "Landing"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_EquippingWeapon UMETA(DisplayName = "EquippingWeapon"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum  EDeadPose 
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),

	//per non andare out of rande dobbiamo aggiungere un ulteriore Enum
	EDP_MAX UMETA(DisplayName = "DefaultMax")
};


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	
	EES_Idle UMETA(DisplayName = "Idle"),

	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patroling UMETA(DisplayName = "Patroling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	//Qua stiamo attivamente in animazione di Attacco 
	EES_Engaged UMETA(DisplayName = "Engaged"),


	
};

