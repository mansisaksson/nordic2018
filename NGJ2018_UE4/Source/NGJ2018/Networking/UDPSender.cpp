#include "UDPSender.h"
#include "Engine.h"
#include "Engine/World.h"

AUDPSender::AUDPSender()
{
	SenderSocket = NULL;
	ShowOnScreenDebugMessages = true;
}

AUDPSender* AUDPSender::GetUDPSender(UObject* worldContext)
{
	static TWeakObjectPtr<AUDPSender> weakThis;
	if (!weakThis.IsValid())
		if (UWorld* World = GEngine->GetWorldFromContextObject(worldContext, EGetWorldErrorMode::LogAndReturnNull))
			weakThis = World->SpawnActor<AUDPSender>();

	return weakThis.Get();
}

void AUDPSender::WithUDPSender(UObject* worldContext, const TFunction<void(AUDPSender &)> &callback)
{
	if (auto* udpSender = AUDPSender::GetUDPSender(worldContext))
		callback(*udpSender);
}

void AUDPSender::BeginPlay()
{
	Super::BeginPlay();
	OpenUDPSocket(TEXT("udpSender"), CVarIPAddress.GetValueOnGameThread(), CVarPort.GetValueOnGameThread());
}

void AUDPSender::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CloseUDPSocket();
}

bool AUDPSender::OpenUDPSocket(
	const FString& SocketName,
	const FString& IP,
	const int32 Port) 
{
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	RemoteAddr->SetIp(*IP, bIsValid);
	RemoteAddr->SetPort(Port);

	if (!bIsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDP Sender>> IP address was not valid! %s"), *IP);
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*SocketName)
		.AsReusable()
		.WithBroadcast();

	//Set Send Buffer Size
	int32 SendSize = 16 * 1024;
	SenderSocket->SetSendBufferSize(SendSize, SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);

	UE_LOG(LogNet, Log, TEXT("\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	UE_LOG(LogNet, Log, TEXT("****UDP**** Sender Initialized Successfully!!!"));
	UE_LOG(LogNet, Log, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n"));

	return true;
}

bool AUDPSender::CloseUDPSocket()
{
	if (SenderSocket)
	{
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
		return true;
	}
	return false;
}

bool AUDPSender::SendNetData(FNGPacket netData)
{
	if (!SenderSocket)
	{
		UE_LOG(LogNet, Warning, TEXT("No sender socket! %s"));
		return false;
	}

	FString message = netData.Serialize();
	//UE_LOG(LogTemp, Log, TEXT("Message: %s"), *message);
	FTCHARToUTF8 utf8str(*message);
	int32 DestLen = utf8str.Length();

	// **** Send package *****
	int32 BytesSent = 0;
	SenderSocket->SendTo((unsigned char*)(TCHAR_TO_UTF8(*message)), DestLen, BytesSent, *RemoteAddr);

	if (BytesSent <= 0)
	{
		const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
		UE_LOG(LogNet, Error, TEXT("%s"), *Str);
		return false;
	}

	//UE_LOG(LogTemp, Log, TEXT("Bytes sent %d"), BytesSent);

	return true;
}