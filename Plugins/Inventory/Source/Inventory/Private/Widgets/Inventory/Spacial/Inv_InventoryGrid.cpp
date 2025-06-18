// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spacial/Inv_InventoryGrid.h"

#include "InteractiveToolManager.h"
#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"

void UInv_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	const FVector2D CanvasPos = UInv_WidgetUtils::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	if (CursorExitedCanvas(CanvasPos, UInv_WidgetUtils::GetWidgetSize(CanvasPanel), MousePos))
	{
		return;
	}
	
	UpdateTileParameters(CanvasPos, MousePos);
}

void UInv_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D MousePosition)
{
	// if mouse not in canvas panel -> return

	if (!bMouseWithinCanvas) return;
	// Calculate the tile quadrant, tile index and coordinates

	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	LastTileParameters = TileParameters;
	TileParameters.TileCoordinates = HoveredTileCoordinates;
	TileParameters.TileIndex = UInv_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);

	//highlight and unhighlight
	OnTileParametersUpdated(TileParameters);
	
}

void UInv_InventoryGrid::OnTileParametersUpdated(const FInv_TileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;

	//Get Hover Item's dimensions
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	
	// Calculate starting coordinate for highlighting
	const FIntPoint StartingPoint = CalculateStartingCoordinate(Parameters.TileCoordinates, Dimensions, Parameters.TileQuadrant);
	ItemDropIndex = UInv_WidgetUtils::GetIndexFromPosition(StartingPoint, Columns);

	CurrentQueryResult = CheckHoverPosition(StartingPoint, Dimensions);

	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex,  Dimensions);
		return;
	}
	UnhighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		//Single item, swap or add stacks
		const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(CurrentQueryResult.ValidItem.Get(), FragmentTags::GridFragment);

		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EInv_GridSlotState::GrayedOut);
	}

}

FInv_SpaceQueryResult UInv_InventoryGrid::CheckHoverPosition(const FIntPoint& Position,
	const FIntPoint& Dimensions)
{
	FInv_SpaceQueryResult Result;
	
	// in grid bounds?
	if (!IsInGridBounds(UInv_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions)) return Result;

	Result.bHasSpace = true;
	
	// if more than one indices occupied of the same item, does all have the same upper left index?

	TSet<int32> OccupiedUpperLeftIndices;
	UInv_InventoryStatics::ForEach2D(GridSlots,
										UInv_WidgetUtils::GetIndexFromPosition(Position, Columns),
										Dimensions,
										Columns,
										[&](const UInv_GridSlot* GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
			Result.bHasSpace = false;
		}
	});
	
	// if so, is there is only one item in the way (can we swap)?
	if (OccupiedUpperLeftIndices.Num() == 1) // one item at position - valid for swapping/combining
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem =  GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}

	return Result;
}

bool UInv_InventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize,
	const FVector2D& Location)
{
	bLastMouseWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UInv_WidgetUtils::IsWithinBounds(BoundaryPos, BoundarySize, Location);

	if (!bMouseWithinCanvas && bLastMouseWithinCanvas)
	{
		UnhighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UInv_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas) return;
	
	UnhighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetOccupiedTexture();
	});
	
	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}

void UInv_InventoryGrid::UnhighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		if (GridSlot->IsAvailable())
		{
			GridSlot->SetUnoccupiedTexture();
		}
		else
		{
			GridSlot->SetOccupiedTexture();
		}
	});
}

void UInv_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions,
	EInv_GridSlotState GridSlotState)
{
	UnhighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [State = GridSlotState](UInv_GridSlot* GridSlot)
	{
		switch (State)
		{
		case EInv_GridSlotState::Occupied:
			GridSlot->SetOccupiedTexture();
			break;
		case EInv_GridSlotState::Unoccupied:
			GridSlot->SetUnoccupiedTexture();
			break;
		case EInv_GridSlotState::Selected:
			GridSlot->SetSelectedTexture();
			break;
		case EInv_GridSlotState::GrayedOut:
			GridSlot->SetGrayedOutTexture();
			break;
		}
	});

	LastHighlightedIndex = Index;
	LastHighlightedDimensions =  Dimensions;
}


FIntPoint UInv_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions,
                                                          const EInv_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartingCoordinate;
	switch (Quadrant)
	{
		case EInv_TileQuadrant::TopLeft:
			StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
			break;
		case EInv_TileQuadrant::TopRight:
			StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
			break;
		case EInv_TileQuadrant::BottomLeft:
			StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
			break;
		case EInv_TileQuadrant::BottomRight:
			StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
			break;
		default:
			UE_LOG(LogInventory, Error, TEXT("Invalid quadrant"))
			StartingCoordinate = FIntPoint(-1, -1);
	}
	
	return StartingCoordinate;
}

FIntPoint UInv_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition,
                                                          const FVector2D MousePosition) const
{
	return FIntPoint {
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y) / TileSize)),
	};
}

EInv_TileQuadrant UInv_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition,
	const FVector2D MousePosition) const
{
	// Calculate relative position within a current tile
	const float TileLocalX = FMath::Fmod((MousePosition.X - CanvasPosition.X), TileSize);
	const float TileLocalY = FMath::Fmod((MousePosition.Y - CanvasPosition.Y), TileSize);

	// Determine which quadrant the mouse is in
	// Since widgets get drawn from top left to bottom right of the screen, less Y coordinate closer to Top of the widget,
	// same as less X coordinate closer to the left of the widget.
	// This allows us to calculate quadrant based on these parameters
	
	const bool bIsTop = TileLocalY < TileSize / 2.f; // top if Y is in upper half
	const bool bIsLeft = TileLocalX < TileSize / 2.f; // left if X is in left half

	EInv_TileQuadrant HoveredTileQuadrant{EInv_TileQuadrant::None};
	
	if (bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomRight;

	return HoveredTileQuadrant;
	
}

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructGrid();

	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
	InventoryComponent->OnStackChanged.AddDynamic(this, &ThisClass::AddStacks);
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* Item)
{
	return HasRoomForItem(Item->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FInv_ItemManifest& Manifest)
{
	FInv_SlotAvailabilityResult Result;

	// Determine if item is stackable
	const FInv_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;
	
	// Determine how many stacks to add and max stack size
	const int32 MaxStackSize = StackableFragment? StackableFragment->GetMaxStackSize(): 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;

	TSet<int32> CheckedIndices;
	// For each GridSlot:
	for (const auto& GridSlot : GridSlots)
	{
		// If no more to fill, break out of the loop early
		if (AmountToFill == 0) break;
		
		// If index is available, claim that slot
		if (IsIndexClaimed(CheckedIndices, GridSlot->GetTileIndex())) continue;

		// If the item in Grid bounds?
		if(!IsInGridBounds(GridSlot->GetTileIndex(), GetItemDimensions(Manifest))) continue;
		
		// Can item fit in a slot? (i.e. Check grid bounds fit ability)
		TSet<int32> TentativelyClaimedIndices;
		if (!HasRoomAtIndex(GridSlot,
							GetItemDimensions(Manifest),
							CheckedIndices,
							TentativelyClaimedIndices,
							Manifest.GetItemType(),
							MaxStackSize))
		{
			continue;
		}
		
		// How much to fill?
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable,
																	MaxStackSize,
																	AmountToFill,
																	GridSlot);
		if (AmountToFillInSlot == 0) continue;

		CheckedIndices.Append(TentativelyClaimedIndices);
		
		// Update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;

		Result.SlotAvailabilities.Emplace(
			FInv_SlotAvailability{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetTileIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
			}
		);

		AmountToFill -= AmountToFillInSlot;
		// How much is the Remainder?
		Result.Remainder = AmountToFill;
		if (AmountToFill == 0) return Result;
		
	}
	
	return Result;
}

bool UInv_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const
{
	return CheckedIndices.Contains(Index);
}

bool UInv_InventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot,
                                        const FIntPoint Dimensions,
                                        const TSet<int32> CheckedIndices,
                                        TSet<int32>& OutTentativelyClaimedIndices,
                                        const FGameplayTag& ItemType,
                                        const int32 MaxStackSize)
{
	// If there is room at the given index? (i.e. Are there other items claiming this index)
	bool bHasRoomAtIndex = true;
	UInv_InventoryStatics::ForEach2D(GridSlots,GridSlot->GetTileIndex(),Dimensions,Columns,
		[&](const UInv_GridSlot* SubGridSlot)
		{
			if (CheckSlotConstraints(GridSlot,
									 SubGridSlot,
									 CheckedIndices,
									 OutTentativelyClaimedIndices,
									 ItemType,
									 MaxStackSize))
			{
				OutTentativelyClaimedIndices.Add(SubGridSlot->GetTileIndex());
			}
			else
			{
				bHasRoomAtIndex = false;
			}
		});
	
	return bHasRoomAtIndex;
}

bool UInv_InventoryGrid::CheckSlotConstraints(const UInv_GridSlot* GridSlot,
											  const UInv_GridSlot* SubGridSlot,
											  const TSet<int32>& CheckedIndices,
											  TSet<int32>& OutTentativelyClaimedIndices,
											  const FGameplayTag& ItemType,
											  const int32 MaxStackSize) const
{
	// Index claimed?
	if (IsIndexClaimed(CheckedIndices,  SubGridSlot->GetTileIndex())) return false;
	
	// Has valid item?
	if (!HasValidItem(SubGridSlot))
	{
		OutTentativelyClaimedIndices.Add(SubGridSlot->GetTileIndex());
		return true;
	}
	//Is the grid slot is upper left slot?
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;
	
	// Is item is stackable?
	const UInv_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;
	
	// Is item type the same?
	if (!DoesItemTypeMatch(SubItem, ItemType)) return false;
	
	// If stackable, if it at max capacity?
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;
	
	return true;
}

bool UInv_InventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UInv_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const 
{
	if (StartIndex < 0 || StartIndex >= GridSlots.Num()) return false;
	
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

int32 UInv_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable,
													 const int32 MaxStackSize,
													 const int32 AmountToFill,
													 const UInv_GridSlot* GridSlot) const
{
	// calculate room in the slot
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);

	//if stackable, get minimum between AmountToFeel and RoomInSlot
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UInv_InventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();

	// If we're on a slot that does not hold a stack count, we must get an actual stack count
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}

	return CurrentSlotStackCount;
	
}

bool UInv_InventoryGrid::IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetTileIndex();
}

bool UInv_InventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UInv_InventoryGrid::IsRightClicked(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UInv_InventoryGrid::IsLeftClicked(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

FIntPoint UInv_InventoryGrid::GetItemDimensions(const FInv_ItemManifest& Manifest) const
{
	const FInv_GridFragment* GridFragment = Manifest.GetFragmentOfType<FInv_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UInv_InventoryGrid::RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!GridFragment) return;

	UInv_InventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetIsAvailable(true);
		GridSlot->SetStackCount(0);
	});

	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UInv_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
	
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex,
                                         const int32 PreviousGridSlot)
{
	AssignHoverItem(InventoryItem);

	HoverItem->SetPrevGridIndex(PreviousGridSlot);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ?  GridSlots[GridIndex]->GetStackCount() : 0);
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInv_HoverItem>(GetOwningPlayer(), HoverItemClass);
	}

	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(InventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;

	const FVector2D DrawSize = GetImageDrawSize(GridFragment);
	FSlateBrush IconBrush;

	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
	
}



void UInv_InventoryGrid::AddItem(UInv_InventoryItem* Item)
{
	if (!MatchesCategory(Item)) return;

	FInv_SlotAvailabilityResult Result =  HasRoomForItem(Item);

	AddItemToIndices(Result, Item);
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{
	// Get Grid fragment and Icon fragment

	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}
}

void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable,
                                        const int32 StackAmount)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;

	// Create clotted item
	UInv_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);

	// Add slotted item to canvas
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	
	// Store a new widget in a container
	SlottedItems.Add(Index, SlottedItem);
}

UInv_SlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item,
														const bool bStackable,
                                                        const int32 StackAmount,
                                                        const FInv_GridFragment* GridFragment,
                                                        const FInv_ImageFragment* ImageFragment,
                                                        const int32 Index)
{
	UInv_SlottedItem* SlottedItem = CreateWidget<UInv_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::OnSlottedItemClicked);
	
	return SlottedItem;
}

void UInv_InventoryGrid::AddSlottedItemToCanvas(const int32 Index,
												const FInv_GridFragment* GridFragment,
												UInv_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot =  UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetImageDrawSize(GridFragment));
	const FVector2D DrawPosition = UInv_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPosWithPadding = DrawPosition + FVector2D(GridFragment->GetGridPadding());

	CanvasSlot->SetPosition(DrawPosWithPadding);
	
}

void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem,
											 const FInv_GridFragment* GridFragment,
											 const FInv_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;	
	Brush.ImageSize = GetImageDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

FVector2D UInv_InventoryGrid::GetImageDrawSize(const FInv_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	return GridFragment->GetGridSize() * IconTileWidth;
}

void UInv_InventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount)
{
	check (GridSlots.IsValidIndex(Index));

	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}

	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem, FragmentTags::GridFragment);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);

	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(NewItem);
		GridSlot->SetUpperLeftIndex(Index);
		GridSlot->SetOccupiedTexture();
		GridSlot->SetIsAvailable(false);
	});

	UInv_GridSlot* GridSlot =  GridSlots[Index];
	GridSlot->SetOccupiedTexture();
}

void UInv_InventoryGrid::AddStacks(const FInv_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get())) return;

	for (const auto Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UInv_InventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UInv_InventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem) && IsLeftClicked(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GridIndex);
		return;
	}

	if (IsRightClicked(MouseEvent))
	{
		CreateItemPopUp(GridIndex);
		return;
	}
	
	// Are hover item and clicked item of the same type and stackable?
	if (IsSameStackable(ClickedInventoryItem))
	{
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		const FInv_StackableFragment* StackableFragment = ClickedInventoryItem->GetItemManifest().GetFragmentOfType<FInv_StackableFragment>();
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot = MaxStackSize - ClickedStackCount;
		const int32 HoveredStackCount = HoverItem->GetStackCount();
		
		// Should we swap the stack counts?
		if (ShouldSwapStackCounts(RoomInClickedSlot, HoveredStackCount, MaxStackSize))
		{
			//Swap stack counts
			SwapStackCounts(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}

		// Should we consume the hover item's stack? (add stacks to clicked item, room in a clicked slot >= HoveredItemStack
		if (ShouldConsumeHoverItemStacks(HoveredStackCount, RoomInClickedSlot))
		{
			//Consume item hover stacks
			ConsumeHoverItemStacks(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		// Should we fill in the stacks of the hover item (and not consume it)?
		if (ShouldFillInStack(RoomInClickedSlot, HoveredStackCount))
		{
			FillInStack(RoomInClickedSlot, HoveredStackCount - RoomInClickedSlot, GridIndex);
			return;
		}
		// Clicked slot is already full - do nothing
		if (RoomInClickedSlot == 0) return;
	}

	// Swap with the hover item (Hover item becomes clicked item and vice versa
	SwapWithHoverItem(ClickedInventoryItem, GridIndex);
	
}

void UInv_InventoryGrid::CreateItemPopUp(const int32 GridIndex)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (IsValid(GridSlots[GridIndex]->GetItemPopUp())) return;

	ItemPopUp = CreateWidget<UInv_ItemPopUp>(this, ItemPopUpClass);
	GridSlots[GridIndex]->SetItemPopUp(ItemPopUp);
	
	OwningCanvasPanel->AddChild(ItemPopUp);

	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	CanvasSlot->SetPosition(MousePosition - ItemPopUpOffset);
	CanvasSlot->SetSize(ItemPopUp->GetBoxSize());

	const int32 SliderMax = GridSlots[GridIndex]->GetStackCount() - 1;
	
	if (RightClickedItem->IsStackable() && SliderMax > 0)
	{
		ItemPopUp->OnSplit.BindDynamic(this, &ThisClass::OnPopUpMenuSplit);
		ItemPopUp->SetSliderParams(SliderMax, FMath::Max(1, GridSlots[GridIndex]->GetStackCount() / 2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}

	ItemPopUp->OnDrop.BindDynamic(this, &ThisClass::OnPopUpMenuDrop);
	
	if (RightClickedItem->IsConsumable())
	{
		ItemPopUp->OnConsume.BindDynamic(this, &ThisClass::OnPopUpMenuConsume);
	}
}

bool UInv_InventoryGrid::IsSameStackable(const UInv_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bIsSameItem && bIsStackable && HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());
}

void UInv_InventoryGrid::SwapWithHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem)) return;

	UInv_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempIsStackable = HoverItem->IsStackable();

	// Keep the same previous grid index
	// Assign hover item
	AssignHoverItem(ClickedInventoryItem,  GridIndex, HoverItem->GetPrevGridIndex());

	// Remove clicked inventory item from the grid
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	
	// Add what was the hover item to the grid
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);

	UpdateGridSlots(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
}

bool UInv_InventoryGrid::ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount,const int32 MaxStackSize)
{
	return RoomInClickedSlot == 0 && HoveredStackCount < MaxStackSize;
}

void UInv_InventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount,
	const int32 Index)
{
	UInv_GridSlot* GridSlot = GridSlots[Index];
	GridSlot->SetStackCount(HoveredStackCount);

	UInv_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoveredStackCount);

	HoverItem->UpdateStackCount(ClickedStackCount);
}

bool UInv_InventoryGrid::ShouldConsumeHoverItemStacks(const int32 HoverStackCount, const int32 RoomInClickedSlot) const
{
	return RoomInClickedSlot >= HoverStackCount;
}

void UInv_InventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount,
	const int32 Index)
{
	const int32 AmountToTransfer = HoveredStackCount;
	const int32 NewClickedStackCount = ClickedStackCount + AmountToTransfer;

	GridSlots[Index]->SetStackCount(NewClickedStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();

	const FInv_GridFragment* GridFragment= GridSlots[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FInv_GridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);

	HighlightSlots(Index, Dimensions);
}

bool UInv_InventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const
{
	return RoomInClickedSlot < HoveredStackCount;
}

void UInv_InventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UInv_GridSlot* GridSlot = GridSlots[Index];
	const int32 NewStackCount = GridSlot->GetStackCount() + FillAmount;

	GridSlot->SetStackCount(NewStackCount);

	UInv_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);

	ClickedSlottedItem->UpdateStackCount(NewStackCount);
	
	HoverItem->UpdateStackCount(Remainder);
}



void UInv_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for (int32 j =  0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			UInv_GridSlot* GridSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChild(GridSlot);

			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UInv_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));

			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot); //GridCPS - Grid Canvas Panel Slot
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition * TileSize);

			GridSlots.Add(GridSlot);
			GridSlot->GridSlotHovered.AddDynamic(this, &ThisClass::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &ThisClass::OnGridSlotUnhovered);
			GridSlot->GridSlotClicked.AddDynamic(this, &ThisClass::OnGridSlotClicked);
		}
	}
}

void UInv_InventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}

	auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		//Put item down at this index
		PutDownOnIndex(ItemDropIndex);
	}
}

void UInv_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
}

void UInv_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;

	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetPrevGridIndex(INDEX_NONE);
	HoverItem->SetImageBrush(FSlateNoResource());

	HoverItem->RemoveFromParent();
	HoverItem = nullptr;

	ShowCursor();
}

void UInv_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}

void UInv_InventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

void UInv_InventoryGrid::SetOwningCanvas(UCanvasPanel* Canvas)
{
	OwningCanvasPanel = Canvas;
}

UUserWidget* UInv_InventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}

	return VisibleCursorWidget;
}

UUserWidget* UInv_InventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}

	return HiddenCursorWidget;
}

void UInv_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;

	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UInv_InventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;

	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}

void UInv_InventoryGrid::OnPopUpMenuSplit(int32 SplitAmount, int32 Index)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;

	if (!RightClickedItem->IsStackable()) return;

	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];

	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	AssignHoverItem(RightClickedItem, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);

	
}

void UInv_InventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;

	PickUp(RightClickedItem, Index);
	DropItem();
}

void UInv_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem) || !IsValid(HoverItem->GetInventoryItem())) return;

	//TODO: Tell server to actually drop an item

	ClearHoverItem();
	ShowCursor();
}

void UInv_InventoryGrid::OnPopUpMenuConsume(int32 Index)
{
	
}

bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}

