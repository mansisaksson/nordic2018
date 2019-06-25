#pragma once
#include "Networking.h"
#include "GameFramework/Actor.h"
#include "Networking/NetDataStructures.h"
#include "UDPSender.generated.h"

UCLASS()
class NGJ2018_API AUDPSender : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Sender")
		bool ShowOnScreenDebugMessages;

public:
	AUDPSender();

	static AUDPSender* GetUDPSender(UObject* worldContext);
	static void WithUDPSender(UObject* worldContext, const TFunction<void(AUDPSender &)> &callback);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TSharedPtr<FInternetAddr> RemoteAddr;
	FSocket* SenderSocket;

	bool OpenUDPSocket(
		const FString& SocketName,
		const FString& IP,
		const int32 Port
	);

	bool CloseUDPSocket();
	bool SendNetData(FNGPacket netData);

};
