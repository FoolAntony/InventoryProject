// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"

#include "Items/Inv_InventoryItem.h"

#include "Components/Image.h"

void UInv_GridSlot::SetOccupiedTexture()
{
	SetGridSlotState(EInv_GridSlotState::Occupied);
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UInv_GridSlot::SetUnoccupiedTexture()
{
	SetGridSlotState(EInv_GridSlotState::Unoccupied);
	Image_GridSlot->SetBrush(Brush_Unoccupied);
	
}

void UInv_GridSlot::SetSelectedTexture()
{
	SetGridSlotState(EInv_GridSlotState::Selected);
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UInv_GridSlot::SetGrayedOutTexture()
{
	SetGridSlotState(EInv_GridSlotState::GrayedOut);
	Image_GridSlot->SetBrush(Brush_GrayedOut);
}

void UInv_GridSlot::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseEnter(MyGeometry, MouseEvent);
	GridSlotHovered.Broadcast(TileIndex, MouseEvent);
}

void UInv_GridSlot::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);
	GridSlotUnhovered.Broadcast(TileIndex, MouseEvent);
}

FReply UInv_GridSlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	GridSlotClicked.Broadcast(TileIndex, MouseEvent);
	return FReply::Handled();
}

void UInv_GridSlot::SetInventoryItem(UInv_InventoryItem* Item)
{
	InventoryItem = Item;
}
