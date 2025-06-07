// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Highlightable.h"
#include "Components/StaticMeshComponent.h"
#include "Inv_HighlightableStaticMesh.generated.h"

/**
 *	Actor class storing a modified static mesh component with an ability to highlight an object when traced by system
 *  Should be replaced with a regular StaticMesh component in blueprint;
 */
UCLASS()
class INVENTORY_API UInv_HighlightableStaticMesh : public UStaticMeshComponent, public IInv_Highlightable
{
	GENERATED_BODY()

public:
	//====================
	//	FUNCTIONS
	//====================

	// Virtual functions overridden from functions stored in Highlightable interface
	virtual void Highlight_Implementation() override;
	virtual void UnHighlight_Implementation() override;

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UMaterialInterface> HighlightMaterial; // Overlay material; Can be modified in Blueprint
	
};
