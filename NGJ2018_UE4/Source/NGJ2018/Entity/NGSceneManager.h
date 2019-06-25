#pragma once
#include "Networking.h"
#include "Entity/NGActor.h"
#include "Engine/DataTable.h"
#include "Networking/NetDataStructures.h"
#include "NGSceneManager.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct NGJ2018_API FNGObjectType : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ANGActor> ngActorClass;
};

UCLASS()
class NGJ2018_API ANGSceneManager : public AActor
{
	GENERATED_BODY()

private:
	TMap<int32, ANGActor*> mGuidToActorMap;
	TMap<ANGActor*, int32> mActorToGuidMap;
	TMap<int32, FTransform> mTransformMap;
	TMap<FString, TSubclassOf<ANGActor>> mTypeToClassMap;
	TMap<UClass*, FString> mClassToTypeMap;

	int32 mCurrentGravDir = 0;

	TMap<ANGActor*, float> mTimeSinceUpdate;

public:
	ANGSceneManager();

	static ANGSceneManager* GetSceneManager(UObject* worldContext);
	static void WithSceneManager(UObject* worldContext, TFunction<void (ANGSceneManager &)> callback);
	static void ForEachLocalNGActor(UObject* worldContext, TFunction<void (ANGActor* ngActor)> callback);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void UpdateNGObject(const FNGObject &ngObject);

	int32 AddActorToNetworkScene(ANGActor* actor);
	int32 AddActorToNetworkScene(ANGActor* actor, int32 id);
	void RemoveActorFromNetworkScene(ANGActor* actor);

	ANGActor* GetActorFromNetId(const int32 &id);
	int32 GetNetIdFromActor(ANGActor* actor);
	const TMap<int32, ANGActor*> &GuidActorMap();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContext"))
		static int32 GetGravityDir(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
		static void FlipGravityDir(UObject* WorldContext);

	int32 GravityDir() { return mCurrentGravDir; };
	void SetGravityDir(int32 gravDir) { mCurrentGravDir = gravDir; };

private:
	UDataTable* ObjectTypeTable();
};
