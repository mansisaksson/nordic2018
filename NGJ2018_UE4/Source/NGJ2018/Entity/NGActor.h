// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NGActor.generated.h"

UCLASS()
class NGJ2018_API ANGActor : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Replicate = true;
	
private:
	bool mIsExternal = false;

public:	
	ANGActor();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetIsExternal(bool isExternal);

	UFUNCTION(BlueprintPure)
		bool IsExternal();
};
