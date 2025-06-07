// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Inv_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/HUD/Inv_HUDWidget.h"

AInv_PlayerController::AInv_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	TraceLength = 500.f;
	ItemTraceChannel = ECC_GameTraceChannel1;
}

void AInv_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TraceForItem();
}

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

void AInv_PlayerController::TraceForItem()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return; // exit function if GEngine or GameViewport do not exist

	//Getting the center of a screen in Game World space
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter = ViewportSize / 2.f;
	FVector TraceStart;
	FVector Forward;
	
	//if not possible to deproject screen to worldspace, exit function
	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter, TraceStart, Forward)) return;

	const FVector TraceEnd = TraceStart + Forward * TraceLength; //Forward vector specifies direction, then multiplies by length to generate vector of this length
	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ItemTraceChannel); //store data of any hit result with Item channel enabled objects

	LastActor = ThisActor;
	ThisActor = HitResult.GetActor();

	if (!ThisActor.IsValid())
	{
		if (IsValid(HUDWidget)) HUDWidget->HidePickupMessage();
	}

	if (ThisActor == LastActor) return;

	if (ThisActor.IsValid())
	{
		UInv_ItemComponent* ItemComponent = ThisActor->FindComponentByClass<UInv_ItemComponent>();
		if (!IsValid(ItemComponent)) return;

		if (IsValid(HUDWidget)) HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
	}

	if (LastActor.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Ended Tracing a new actor"));
	}
}

