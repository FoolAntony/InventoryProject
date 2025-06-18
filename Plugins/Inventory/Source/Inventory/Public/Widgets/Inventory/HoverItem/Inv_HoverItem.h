// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Inv_HoverItem.generated.h"

/**
 *  The HoverItem is an item that appears and follows a mouse cursor each time
 *  the inventory item on the grid gets clicked by player
 */

class UInv_InventoryItem;
class UTextBlock;
class UImage;

UCLASS()
class INVENTORY_API UInv_HoverItem : public UUserWidget
{
	GENERATED_BODY()
public:
	//====================
	//	FUNCTIONS
	//==================== 

	UInv_InventoryItem* GetInventoryItem() const {return InventoryItem.Get();}
	FIntPoint GetGridDimensions() const {return GridDimensions;}
	int32 GetPrevGridIndex() const {return PreviousGridIndex;}
	int32 GetStackCount() const {return StackCount;}
	bool IsStackable() const {return bIsStackable;}

	void SetInventoryItem(UInv_InventoryItem* Item);
	void SetGridDimensions(const FIntPoint& NewGridDimensions) {GridDimensions = NewGridDimensions;}
	void SetPrevGridIndex(const int32 Index) {PreviousGridIndex = Index;}
	void SetImageBrush (const FSlateBrush& Brush) const;
	void UpdateStackCount (const int32 Count);
	void SetIsStackable(bool bStacks);
	FGameplayTag GetItemType() const;

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 PreviousGridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable = false;
	int32 StackCount{0};
};
