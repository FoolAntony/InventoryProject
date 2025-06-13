// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Inv_GridTypes.h"
#include "Inv_InventoryGrid.generated.h"

struct FInv_ImageFragment;
struct FInv_GridFragment;
struct FGameplayTag;
class UInv_SlottedItem;
struct FInv_ItemManifest;
class UInv_ItemComponent;
class UInv_InventoryComponent;
class UInv_GridSlot;
class UCanvasPanel;

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:
	//====================
	//	FUNCTIONS
	//====================
	
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	virtual void NativeOnInitialized() override;
	FInv_SlotAvailabilityResult HasRoomForItem (const UInv_ItemComponent* ItemComponent);

	UFUNCTION()
	void AddItem(UInv_InventoryItem* Item);
	
	

private:
	//====================
	//	FUNCTIONS
	//====================

	void ConstructGrid();
	bool MatchesCategory(const UInv_InventoryItem* Item) const;
	bool IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const;
	bool IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const;
	bool DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const;
	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const;
	int32 DetermineFillAmountForSlot(const bool bStackable,
									 const int32 MaxStackSize,
									 const int32 AmountToFill,
									 const UInv_GridSlot* GridSlot) const;
	int32 GetStackAmount(const UInv_GridSlot* GridSlot) const;
	
	bool CheckSlotConstraints(const UInv_GridSlot* GridSlot,
							  const UInv_GridSlot* SubGridSlot,
							  const TSet<int32>& CheckedIndices,
							  TSet<int32>& OutTentativelyClaimedIndices,
							  const FGameplayTag& ItemType,
							  const int32 MaxStackSize) const;
	
	bool HasValidItem(const UInv_GridSlot* GridSlot) const;
	
	bool HasRoomAtIndex(const UInv_GridSlot* GridSlot,
		const FIntPoint Dimensions,
		const TSet<int32> CheckedIndices,
		TSet<int32>& OutTentativelyClaimedIndices,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize);
	
	FInv_SlotAvailabilityResult HasRoomForItem (const UInv_InventoryItem* Item);
	FInv_SlotAvailabilityResult HasRoomForItem (const FInv_ItemManifest& Manifest);
	void AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem);
	void AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	FIntPoint GetItemDimentions(const FInv_ItemManifest& Manifest) const;
	
	UInv_SlottedItem* CreateSlottedItem(
		UInv_InventoryItem* Item,
		const bool bStackable,
		const int32 StackAmount,
		const FInv_GridFragment* GridFragment,
		const FInv_ImageFragment* ImageFragment,
		const int32 Index);

	void AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment, UInv_SlottedItem* SlottedItem) const;
	
	void SetSlottedItemImage(
		const UInv_SlottedItem* SlottedItem,
		const FInv_GridFragment* GridFragment,
		const FInv_ImageFragment* ImageFragment) const;
	
	FVector2D GetImageDrawSize (const FInv_GridFragment* GridFragment) const;
	void UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem,  const int32 StackAmount);
	
	
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_SlottedItem> SlottedItemClass;

	UPROPERTY()
	TMap<int32, TObjectPtr<UInv_SlottedItem>> SlottedItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true") ,Category = "Inventory")
	EInv_ItemCategory ItemCategory;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_GridSlot> GridSlotClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize;
};
