// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_SlottedItem.generated.h"

/**
 * 
 */
class UImage;
class UInv_InventoryItem;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemClicked, int32, GridIndex, const FPointerEvent&, MouseEvent);

UCLASS()
class INVENTORY_API UInv_SlottedItem : public UUserWidget
{
	GENERATED_BODY()

public:
	//====================
	//	FUNCTIONS
	//====================

	virtual FReply NativeOnMouseButtonDown(const  FGeometry& InGeometry, const FPointerEvent& MouseEvent) override;

	bool IsStackable() const {return bIsStackable;}
	void SetIsStackable(const bool bStackable) {bIsStackable = bStackable;}
	UImage* GetImageIcon() const {return Image_Icon;}
	void SetGridIndex(const int32 Index) {GridIndex = Index;}
	int32 GetGridIndex() const {return GridIndex;}
	void SetGridDimensions(const FIntPoint& Dimensions) {GridDimensions = Dimensions;}
	FIntPoint GetGridDimensions() const {return GridDimensions;}
	UInv_InventoryItem* GetInventoryItem() const {return InventoryItem.Get();}

	void SetInventoryItem(UInv_InventoryItem* Item);
	void SetImageBrush(const FSlateBrush& Brush) const;
	void UpdateStackCount(const int32 StackCount);
	FSlottedItemClicked OnSlottedItemClicked;
	
	
private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 GridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable = false;
};
