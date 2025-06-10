// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	//====================
	//	FUNCTIONS
	//====================

	void SetTileIndex(int32 Index) {TileIndex = Index;}
	int32 GetTileIndex() const {return TileIndex;}

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	int32 TileIndex;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
};
