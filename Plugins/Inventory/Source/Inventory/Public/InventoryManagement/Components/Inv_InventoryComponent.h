// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/Inv_FastArray.h"
#include "Inv_InventoryComponent.generated.h"

/*
 *  Inventory object component
 *  Requires inventory widget to create in a viewport
 */

struct FInv_SlotAvailabilityResult;
class UInv_InventoryBase;
class UInv_InventoryItem;
class UInv_ItemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChange, UInv_InventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChange, const FInv_SlotAvailabilityResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRoomInInventory);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//====================
	//	FUNCTIONS
	//==================== 
	
	UInv_InventoryComponent();
	void ToggleInventoryMenu();
	void AddRepSubobject(UObject* SubObj);
	void SpawnDroppedItem(UInv_InventoryItem* Item, int32 StackCount);
	void Server_AddStacksToItem(UInv_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void TryAddItem(UInv_ItemComponent* ItemComponent);

	UFUNCTION(Server, Reliable)
	void Server_AddNewItem(UInv_ItemComponent* ItemComponent, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_DropItem(UInv_InventoryItem* Item, int32 StackCount);

	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	FInventoryItemChange OnItemAdded;
	FInventoryItemChange OnItemRemoved;
	FNoRoomInInventory NoRoomInInventory;
	FStackChange OnStackChanged;

protected:
	//====================
	//	FUNCTIONS
	//====================
	
	virtual void BeginPlay() override;

private:
	//====================
	//	FUNCTIONS
	//====================
	
	void ConstructInventory();
	void OpenInventoryMenu();
	void CloseInventoryMenu();

	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	UPROPERTY(Replicated)
	FInv_InventoryFastArray InventoryList;
	
	UPROPERTY()
	TObjectPtr<UInv_InventoryBase> InventoryMenu;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_InventoryBase> InventoryMenuClass;

	/*Stores currently owning PlayerController */
	TWeakObjectPtr<APlayerController> OwningController;
	bool bInventoryMenuOpen;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMin = -85.f;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMax = 85.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMin = 10.f;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMax = 50.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float RelativeSpawnElevation = -70.f;
	
};
