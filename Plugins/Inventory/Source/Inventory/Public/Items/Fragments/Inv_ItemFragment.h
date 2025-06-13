#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Inv_ItemFragment.generated.h"

USTRUCT(BlueprintType)
struct FInv_ItemFragment
{
	GENERATED_BODY()

	/**
	 * Since we derive children from this struct, if we also need to delete a child
	 * though a parent pointer, it will require us to use a polymorphic destructor; Hence we need a virtual destructor.
	 * 
	 * By following the rule of 5 for structs, creating a single virtual destructor function requires us to add
	 * the rest of constructors and operator (total five not including regular constructor 
	 */

	//====================
	//	FUNCTIONS
	//====================
	
	FInv_ItemFragment() {}
	FInv_ItemFragment(const FInv_ItemFragment&) = default; // copy constructor
	FInv_ItemFragment& operator=(const FInv_ItemFragment&) = default; // copy operator
	FInv_ItemFragment(FInv_ItemFragment&&) = default; // move constructor
	FInv_ItemFragment& operator=(FInv_ItemFragment&&) = default; // move operator
	virtual ~FInv_ItemFragment() {} // virtual destructor

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(const FGameplayTag Tag) {FragmentTag = Tag;}

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag =  FGameplayTag::EmptyTag;
	
};

USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(const FIntPoint& Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(const float Padding) { GridPadding = Padding; }

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{1,1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{0.f};
};

USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	UTexture2D* GetIcon() const { return Icon; }
	
private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{nullptr};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{44.f,44.f};
	
};