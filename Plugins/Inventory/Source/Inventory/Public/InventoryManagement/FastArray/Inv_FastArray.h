#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inv_FastArray.generated.h"

class UInv_InventoryComponent;
class UInv_InventoryItem;
class UInv_ItemComponent;


/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	//====================
	//	FUNCTIONS
	//====================
	
	FInv_InventoryEntry() {}

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	//friend function allows accessing private and protection sections of a class without making them public
	
	friend UInv_InventoryComponent;
	friend struct FInv_InventoryFastArray;
	
	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item = nullptr;
	
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FInv_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()
	
	//====================
	//	FUNCTIONS
	//====================
	
	FInv_InventoryFastArray() : OwnerComponent(nullptr) {}
	FInv_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

	TArray<UInv_InventoryItem*> GetAllItems() const;

	//FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);	
	// End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInv_InventoryEntry, FInv_InventoryFastArray>(Entries, DeltaParams, *this );
	}

	UInv_InventoryItem* AddEntry(UInv_ItemComponent* ItemComponent);
	UInv_InventoryItem* AddEntry(UInv_InventoryItem* Item);
	void RemoveEntry(UInv_InventoryItem* Item);

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	friend UInv_InventoryComponent;

	// Replicated list of items
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FInv_InventoryFastArray> : TStructOpsTypeTraitsBase2<FInv_InventoryFastArray>
{
	enum { WithNetDeltaSerializer = true };
};



