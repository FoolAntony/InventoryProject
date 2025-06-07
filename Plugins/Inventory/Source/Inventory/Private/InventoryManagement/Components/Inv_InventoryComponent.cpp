// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"


UInv_InventoryComponent::UInv_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInv_InventoryComponent::ToggleInventoryMenu()
{
	// If our menu opened, will close it, else will open it
	bInventoryMenuOpen ? CloseInventoryMenu() : OpenInventoryMenu();
}

void UInv_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructInventory(); //Creating inventory widget when game starts
}

void UInv_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());		//Get current controller owned by player
	checkf(OwningController.IsValid(), TEXT("Inventory Component should have Player Controller as Owner")) //If controller invalid, error drops
	if (!OwningController->IsLocalController()) return; // Exit function if found controller is not a local player's controller
														//(not controller owned by some other player)

	//Creating inventory widget with found Player Controller defined as InventoryMenuClass blueprint
	InventoryMenu = CreateWidget<UInv_InventoryBase>(OwningController.Get(), InventoryMenuClass);
	
	//Making it visible on player's screen
	InventoryMenu->AddToViewport();
	
	//Hide inventory widget once generated
	CloseInventoryMenu();
	
}

void UInv_InventoryComponent::OpenInventoryMenu()
{
	if (!IsValid(InventoryMenu)) return; //Exit function is inventory widget does not exist

	InventoryMenu->SetVisibility(ESlateVisibility::Visible); //Displaying inventory widget
	bInventoryMenuOpen = true;

	if (!OwningController.IsValid()) return; //Also exit if an owning controller does not exist

	// Setting input mode to use UI and making cursor visible
	FInputModeGameAndUI InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(true);
	
}

void UInv_InventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu)) return; // Exit function if inventory widget does not exist

	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed); //Hiding widget
	bInventoryMenuOpen = false;

	if (!OwningController.IsValid()) return; // Also exit if an owning controller does not exist

	// Setting input mode to ignore UI components and hiding cursor
	FInputModeGameOnly InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(false);
	
}



