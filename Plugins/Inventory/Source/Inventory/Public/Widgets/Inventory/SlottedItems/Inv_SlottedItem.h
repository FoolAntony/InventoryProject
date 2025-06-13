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

UCLASS()
class INVENTORY_API UInv_SlottedItem : public UUserWidget
{
	GENERATED_BODY()

public:
	//====================
	//	FUNCTIONS
	//====================

	bool IsStackable() const {return bIsStackable;}
	void SetIsStackable(const bool bStackable) {bIsStackable = bStackable;}
	UImage* GetImageIcon() const {return Image_Icon;}
	void SetGridIndex(const int32 Index) {GridIndex = Index;}
	int32 GetGridIndex() const {return GridIndex;}
	void SetGridDimensions(const FIntPoint& Dimensions) {GridDimensions = Dimensions;}
	FIntPoint GetGridDimensions() const {return GridDimensions;}
	void SetInventoryItem(UInv_InventoryItem* Item);
	UInv_InventoryItem* GetInventoryItem() const {return InventoryItem.Get();}

	void SetImageBrush(const FSlateBrush& Brush) const;
	
	
private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	int32 GridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable = false;
};
