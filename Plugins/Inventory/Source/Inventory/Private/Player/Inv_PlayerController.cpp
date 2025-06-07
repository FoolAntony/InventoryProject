// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Inv_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Widgets/HUD/Inv_HUDWidget.h"

void AInv_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	//if enhanced input subsystem is valid -> add contect mapping to it
	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	CreateHUDWidget();
}

// Sets up input components and actions to be used by Player character
void AInv_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(PrimaryInteractionAction, ETriggerEvent::Started, this, &AInv_PlayerController::PrimaryInteract);
}

// Primary function when interact is triggered
void AInv_PlayerController::PrimaryInteract()
{
	UE_LOG(LogTemp, Log, TEXT("Primary Interact"));
}

// Creates UI HUD Component when called
void AInv_PlayerController::CreateHUDWidget()
{
	if (!IsLocalController()) return; //if local controller is missing, exit function
	HUDWidget = CreateWidget<UInv_HUDWidget>(this, HUDWidgetClass); //create widget based on widget class

	//if created widget is valid -> add it to viewport
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

