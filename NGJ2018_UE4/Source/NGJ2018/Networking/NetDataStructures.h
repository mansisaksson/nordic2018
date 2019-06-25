#pragma once
#include "Dom/JsonObject.h"
#include "NetDataStructures.generated.h"

static TAutoConsoleVariable<int32> CVarSimulatedPhysicsDelay(
	TEXT("net.SimulatedPhysicsDelay"),
	0,
	TEXT("Changes the amount of delay applied to the UDP (Physics) packets"));

static TAutoConsoleVariable<int32> CVarSimulatedPhysicsDelay_AddedRandom(
	TEXT("net.SimulatedPhysicsDelay_AddedRandom"),
	0,
	TEXT("adds a random delay to net.SimulatedPhysicsDelay"));

static TAutoConsoleVariable<FString> CVarIPAddress(
	TEXT("net.unityIpAddr"),
	"127.0.0.1",
	TEXT("The ip for the unity game"));

static TAutoConsoleVariable<int32> CVarPort(
	TEXT("net.unityport"),
	1337,
	TEXT("The port for the unity game"));

USTRUCT(BlueprintType)
struct NGJ2018_API FNGObject
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FVector position;
	
	UPROPERTY(BlueprintReadWrite)
		float rotationAngle;

	UPROPERTY(BlueprintReadWrite)
		FVector scale;
	
	UPROPERTY(BlueprintReadWrite)
		int32 objecId;

	UPROPERTY()
		FString type;

	TSharedPtr<FJsonObject> ToJsonObject();

	static FNGObject FromJsonObject(const TSharedPtr<FJsonObject> &jsonObject);
};

USTRUCT(BlueprintType)
struct NGJ2018_API FNGPacket
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		int32 gravityDir;

	UPROPERTY(BlueprintReadWrite)
		int32 packetID;

	UPROPERTY(BlueprintReadWrite)
		TArray<FNGObject> objects;

	FString Serialize();
	static FNGPacket Deserialize(const FString &jsonString);
};
