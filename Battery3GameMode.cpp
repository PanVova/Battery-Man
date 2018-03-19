// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Battery3GameMode.h"
#include "Battery3Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SpawnVolume.h"
#include "Battery3.h"
ABattery3GameMode::ABattery3GameMode()
{
	// set default pawn class to our Blueprinted character
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	DecayRate = 0.01f;
}
void ABattery3GameMode::BeginPlay() 
{
	Super::BeginPlay();
	SetCurrentState(EBatteryPlayState::EPlaying);
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
		if (SpawnVolumeActor)
		{
			SpawnVolumeActors.AddUnique(SpawnVolumeActor);
		}
	}
	SetCurrentState(EBatteryPlayState::EPlaying);

	ABattery3Character* MyCharacter = Cast<ABattery3Character>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter)
	{
		PowerToWin = (MyCharacter->GetInitialPower()*1.25f);
	}
	if(HUDWidgetClass!=nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(),HUDWidgetClass);
		if (CurrentWidget!=nullptr) 
		{
			CurrentWidget->AddToViewport();
		}
	}

}
float  ABattery3GameMode::GetPowerToWin() 
{
	return  PowerToWin;
}

void ABattery3GameMode::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
	ABattery3Character* MyCharacter = Cast<ABattery3Character>(UGameplayStatics::GetPlayerPawn(this, 0));
	if(MyCharacter)
	{
		if (MyCharacter->GetCurrentPower()> PowerToWin) 
		{
			SetCurrentState(EBatteryPlayState::EWon);
		}
		else if (MyCharacter->GetCurrentPower() > 0)
		{
			MyCharacter->UpdatePower(-DeltaTime * DecayRate*(MyCharacter->GetInitialPower()));
		}
		else 
		{
			SetCurrentState(EBatteryPlayState::EGameOver);
		}
	}
}
EBatteryPlayState ABattery3GameMode::GetCurrentState() const 
{ 
	return CurrentState;
}
void ABattery3GameMode::SetCurrentState(EBatteryPlayState NewState)
{  
	CurrentState = NewState;
	HandleNewState(CurrentState);
}
void ABattery3GameMode::HandleNewState(EBatteryPlayState NewState) 
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	switch (NewState)
	{
	case EBatteryPlayState::EPlaying:
		for (ASpawnVolume* Volume : SpawnVolumeActors) 
		{
			Volume->SetSpawningActive(true);
			
		}
	break;
	
	case EBatteryPlayState::EWon:
		for (ASpawnVolume* Volume : SpawnVolumeActors)
		{
			Volume->SetSpawningActive(false);
		}
	break;
	
	case EBatteryPlayState::EGameOver:
		for (ASpawnVolume* Volume : SpawnVolumeActors)
		{
			Volume->SetSpawningActive(false);
		}
		
		if (PlayerController) 
		{
			PlayerController->SetCinematicMode(true, false,false,true,true);
		}
		if (MyCharacter) 
		{
			MyCharacter->GetMesh()->SetSimulatePhysics(true);
			MyCharacter->GetMovementComponent()->MovementState.bCanJump = false;
		}
	break;
	case EBatteryPlayState::EUnknown:
	break;
	}
}