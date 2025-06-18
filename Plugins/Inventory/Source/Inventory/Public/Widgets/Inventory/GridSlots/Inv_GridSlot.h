// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualizeTexture.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UInv_InventoryItem;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent, int32, GridIndex, const FPointerEvent&, MouseEvent);

enum class EInv_GridSlotState: uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};

UCLASS()
class INVENTORY_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	//====================
	//	FUNCTIONS
	//====================

	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	int32 GetTileIndex() const {return TileIndex;}
	int32 GetUpperLeftIndex() const {return UpperLeftIndex;}
	int32 GetStackCount() const {return StackCount;}
	void SetTileIndex(int32 Index) {TileIndex = Index;}
	void SetUpperLeftIndex(const int32 Index) {UpperLeftIndex = Index;}
	void SetStackCount(const int32 Count) {StackCount = Count;}

	bool IsAvailable() const {return bAvailable;}
	void SetIsAvailable(const bool bIsAvailable) {bAvailable = bIsAvailable;}

	TWeakObjectPtr<UInv_InventoryItem> GetInventoryItem() const {return InventoryItem;}
	void SetInventoryItem(UInv_InventoryItem* Item);

	EInv_GridSlotState GetGridSlotState() const {return  GridSlotState;}
	void SetGridSlotState(const EInv_GridSlotState State) {GridSlotState = State;}

	void SetOccupiedTexture();
	void SetUnoccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();

	//=========================
	//	PARAMETERS & VARIABLES
	//=========================
	
	FGridSlotEvent GridSlotClicked;
	FGridSlotEvent GridSlotHovered;
	FGridSlotEvent GridSlotUnhovered;

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	int32 TileIndex{INDEX_NONE};
	int32 StackCount{0};
	int32 UpperLeftIndex{INDEX_NONE};
	bool bAvailable{true};
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	EInv_GridSlotState GridSlotState;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GrayedOut;

};
