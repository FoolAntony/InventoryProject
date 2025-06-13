#pragma once

#include "CoreMinimal.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "Inv_ItemManifest.generated.h"


/** Item manifest contains all necessary data for creating new inventory item */
class UInv_InventoryItem;
struct FInv_ItemFragment;


USTRUCT(BlueprintType)
struct INVENTORY_API FInv_ItemManifest
{
	GENERATED_BODY()

public:
	//=========================
	//	FUNCTIONS
	//=========================
	
	UInv_InventoryItem* Manifest(UObject* NewOuter);
	EInv_ItemCategory GetItemCategory() const {return ItemCategory;}
	FGameplayTag GetItemType() const {return ItemType;}

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ItemFragment>> Fragments;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory{EInv_ItemCategory::None};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;
};
