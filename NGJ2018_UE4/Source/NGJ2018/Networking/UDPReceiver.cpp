#include "UDPReceiver.h"
#include "Entity/NGSceneManager.h"
#include "Entity/NGActor.h"
#include "Engine.h"
#include "Engine/World.h"

AUDPReceiver::AUDPReceiver()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.f;
	ListenSocket = NULL;
}

AUDPReceiver* AUDPReceiver::GetUDPReceiver(UObject* worldContext)
{
	static TWeakObjectPtr<AUDPReceiver> weakThis;
	if (!weakThis.IsValid())
		if (UWorld* World = GEngine->GetWorldFromContextObject(worldContext, EGetWorldErrorMode::LogAndReturnNull))
			weakThis = World->SpawnActor<AUDPReceiver>();

	return weakThis.Get();
}

void AUDPReceiver::WithUDPReceiver(UObject* worldContext, const TFunction<void(AUDPReceiver &)> &callback)
{
	if (auto* udpReceiver = GetUDPReceiver(worldContext))
		callback(*udpReceiver);
}

void AUDPReceiver::BeginPlay()
{
	Super::BeginPlay();
	OpenConnection("udpReceiver", "0.0.0.0", CVarPort.GetValueOnGameThread());
}

void AUDPReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	mMutex.Lock();
	for (const FNGPacket &netPacket : mPacketStack)
	{
		int32 AddedRandomDelay = CVarSimulatedPhysicsDelay_AddedRandom.GetValueOnGameThread();
		int32 SimulatedNetDelay = CVarSimulatedPhysicsDelay.GetValueOnGameThread() + FMath::RandRange(0, AddedRandomDelay);
		float delay = FMath::Clamp((float)SimulatedNetDelay / 1000.f, 0.0001f, 999999.f);
		//UE_LOG(LogNet, Log, TEXT("Delaying packet with %f seconds"), delay);
		FTimerHandle DummyHandle;
		//GetWorldTimerManager().SetTimer(DummyHandle, FTimerDelegate::CreateUObject(this, &AUDPReceiver::UpdateChunkData, netPacket.RDActor, netPacket.ChunkDataArray), delay, false, delay);
		
		//UE_LOG(LogTemp, Log, TEXT("Packet %d"), netPacket.packetID);
		OnPacketReceived.Broadcast(netPacket);
	}

	mPacketStack.Empty();
	mMutex.Unlock();
}

void AUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CloseUDPSocket();

	delete UDPReceiver;
	UDPReceiver = nullptr;
}

bool AUDPReceiver::OpenConnection(
	const FString& SocketName,
	const FString& IP,
	const int32 Port
)
{
	mLastReceivedPacketID = 0;
	FIPv4Address Addr;
	FIPv4Address::Parse(IP, Addr);

	//Create Socket
	FIPv4Endpoint Endpoint(Addr, Port);

	//BUFFER SIZE
	int32 BufferSize = 2 * 1024 * 1024;

	ListenSocket = FUdpSocketBuilder(*SocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);

	if (ListenSocket == nullptr)
	{
		UE_LOG(LogNet, Error, TEXT("Failed to init udp receiver"));
		return false;
	}

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(0);
	UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP RECEIVER"));
	UDPReceiver->OnDataReceived().BindUObject(this, &AUDPReceiver::Recv);
	UDPReceiver->Start();

	UE_LOG(LogNet, Log, TEXT("\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	UE_LOG(LogNet, Log, TEXT("****UDP**** Receiver Initialized Successfully!!!"));
	UE_LOG(LogNet, Log, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n"));

	return true;
}

bool AUDPReceiver::CloseUDPSocket()
{
	if (UDPReceiver)
		UDPReceiver->Stop();

	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
		return true;
	}

	return false;
}

#include <string>
void AUDPReceiver::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	FString messageString(ArrayReaderPtr->Num(), UTF8_TO_TCHAR(ArrayReaderPtr->GetData()));
	//UE_LOG(LogNet, Log, TEXT("message: %s"), *messageString);

	FNGPacket ngPacket = FNGPacket::Deserialize(messageString);
	if (mLastReceivedPacketID >= ngPacket.packetID)
	{
		//UE_LOG(LogNet, Warning, TEXT("Received packet in the wrong order"));
		//return; // Might still contain usefull information
	}

	mLastReceivedPacketID = ngPacket.packetID;

	mMutex.Lock();
	mPacketStack.Add(ngPacket);
	mMutex.Unlock();
}
