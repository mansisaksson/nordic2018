// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Networking.h"
#include "GameFramework/Actor.h"
#include "Networking/NetDataStructures.h"
#include "GameFramework/Actor.h"
#include "UDPReceiver.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnPacketReceived, const FNGPacket& /* packet*/);

UCLASS()
class NGJ2018_API AUDPReceiver : public AActor
{
	GENERATED_BODY()
private:
	int32 mLastReceivedPacketID;
	TArray<FNGPacket> mPacketStack;

	FUdpSocketReceiver* UDPReceiver = nullptr;
	FSocket* ListenSocket;

	FCriticalSection mMutex;
	FEvent* mSemaphore;

public:
	AUDPReceiver();

	FOnPacketReceived OnPacketReceived;

	static AUDPReceiver* GetUDPReceiver(UObject* worldContext);
	static void WithUDPReceiver(UObject* worldContext, const TFunction<void(AUDPReceiver &)> &callback);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool OpenConnection(const FString& SocketName, const FString& IP, const int32 Port);
	bool CloseUDPSocket();

	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);

};
