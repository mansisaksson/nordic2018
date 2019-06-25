// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Networking/NetDataStructures.h"
#include "NGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NGJ2018_API ANGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	UFUNCTION()
		void TestSendPacket(const FNGPacket &packet);
};
