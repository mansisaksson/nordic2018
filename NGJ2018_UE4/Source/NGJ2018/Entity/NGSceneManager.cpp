#include "NGSceneManager.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Networking/UDPSender.h"
#include "Networking/UDPReceiver.h"

UDataTable* ANGSceneManager::ObjectTypeTable()
{
	static TAssetPtr<UDataTable> objectTypeTable = TAssetPtr<UDataTable>(FStringAssetReference(TEXT("DataTable'/Game/Blueprint/ObjectTypeTable.ObjectTypeTable'")));

	if (!objectTypeTable.IsValid())
		return objectTypeTable.LoadSynchronous();

	return objectTypeTable.Get();
}

ANGSceneManager::ANGSceneManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.016f;
}

ANGSceneManager* ANGSceneManager::GetSceneManager(UObject* worldContext)
{
	static TWeakObjectPtr<ANGSceneManager> weakThis;
	if (!weakThis.IsValid())
		if (UWorld* World = GEngine->GetWorldFromContextObject(worldContext, EGetWorldErrorMode::LogAndReturnNull))
			weakThis = World->SpawnActor<ANGSceneManager>();

	return weakThis.Get();
}

void ANGSceneManager::WithSceneManager(UObject* worldContext, TFunction<void(ANGSceneManager &)> callback)
{
	if (auto* sceneManager = GetSceneManager(worldContext))
		callback(*sceneManager);
}

void ANGSceneManager::ForEachLocalNGActor(UObject* worldContext, TFunction<void(ANGActor* ngActor)> callback)
{
	WithSceneManager(worldContext, [callback](ANGSceneManager &sceneManager)
	{
		auto MapCpy = sceneManager.GuidActorMap();
		for (const auto &idActorPair : MapCpy)
			if (IsValid(idActorPair.Value) && !idActorPair.Value->IsExternal())
				callback(idActorPair.Value);
	});
}

void ANGSceneManager::BeginPlay()
{
	if (UDataTable* objectTypeTable = ObjectTypeTable())
		objectTypeTable->ForeachRow<FNGObjectType>(TEXT(""), [&](const FName &key, const FNGObjectType &value) {
			mTypeToClassMap.Add(key.ToString(), value.ngActorClass);
			mClassToTypeMap.Add(value.ngActorClass, key.ToString());
		});


	AUDPReceiver::WithUDPReceiver(this, [&](AUDPReceiver &udpReceiver) {
		TWeakObjectPtr<ANGSceneManager> weakThis = this;
		udpReceiver.OnPacketReceived.AddLambda([weakThis](const FNGPacket &packet) {
			if (!weakThis.IsValid())
				return;

			for (const FNGObject& ngObject : packet.objects)
				weakThis->UpdateNGObject(ngObject);

			if (packet.gravityDir > weakThis->GravityDir())
				weakThis->SetGravityDir(packet.gravityDir);
		});
	});

	Super::BeginPlay();
}

static int32 packetId = 0;

void ANGSceneManager::Tick(float DeltaTime)
{
	FNGPacket packet;
	packet.gravityDir = mCurrentGravDir;
	packet.packetID = packetId;
	packetId++;

	ForEachLocalNGActor(this, [&](ANGActor* ngActor) {
		if (ngActor->Replicate)
		{
			FNGObject ngObject;
			ngObject.position = ngActor->GetActorLocation();
			ngObject.scale = ngActor->GetActorScale3D();

			const auto rotationVector = ngActor->GetActorQuat().GetAxisY();
			ngObject.rotationAngle = FMath::RadiansToDegrees(FMath::Atan2(rotationVector.Z, rotationVector.Y));

			ngObject.objecId = GetNetIdFromActor(ngActor);

			FString* type = mClassToTypeMap.Find(ngActor->GetClass());
			ngObject.type = type != nullptr ? *type : "undefined";

			packet.objects.Add(ngObject);
		}
	});

	AUDPSender::WithUDPSender(this, [&](AUDPSender &udpSender) {
		udpSender.SendNetData(packet);
	});

	TArray<ANGActor*> actorsToRemove;
	TMap<ANGActor*, float> valuesToUpdate;
	valuesToUpdate.Reserve(mTimeSinceUpdate.Num());
	for (auto &timeSinceActorUpdate : mTimeSinceUpdate)
	{
		const auto newTime = timeSinceActorUpdate.Value + DeltaTime;
		if (newTime > 5.f)
			actorsToRemove.Add(timeSinceActorUpdate.Key);
		else
			valuesToUpdate.Add(timeSinceActorUpdate.Key, newTime);
	}

	for (auto & actorFloatPair : valuesToUpdate)
		mTimeSinceUpdate.Add(actorFloatPair.Key, actorFloatPair.Value);

	for (ANGActor* ngActor : actorsToRemove)
	{
		RemoveActorFromNetworkScene(ngActor);
		ngActor->Destroy();
	}
}

void ANGSceneManager::UpdateNGObject(const FNGObject &ngObject)
{
	if (!mGuidToActorMap.Find(ngObject.objecId))
		if (auto* objectTypeClass = mTypeToClassMap.Find(ngObject.type))
		{
			if (ANGActor* ngActor = GetWorld()->SpawnActor<ANGActor>(*objectTypeClass))
			{
				ngActor->SetIsExternal(true);
				AddActorToNetworkScene(ngActor, ngObject.objecId);
			}
		}

	if (auto* ngActor = mGuidToActorMap.Find(ngObject.objecId))
	{
		if ((*ngActor)->IsExternal())
		{
			(*ngActor)->SetActorLocation(ngObject.position);
			(*ngActor)->SetActorScale3D(ngObject.scale);
			(*ngActor)->SetActorRotation(FQuat(FVector(1, 0, 0), FMath::DegreesToRadians(ngObject.rotationAngle)));

			mTimeSinceUpdate.Add((*ngActor), 0.f);
		}
	}
}

static int32 currentId = -1;
int32 ANGSceneManager::AddActorToNetworkScene(ANGActor* actor)
{
	if (int32* id = mActorToGuidMap.Find(actor))
		return *id;

	int32 newId = currentId;
	currentId--;
	return AddActorToNetworkScene(actor, newId);
}

int32 ANGSceneManager::AddActorToNetworkScene(ANGActor* actor, int32 id)
{
	mActorToGuidMap.Add(actor, id);
	mGuidToActorMap.Add(id, actor);
	return id;
}

void ANGSceneManager::RemoveActorFromNetworkScene(ANGActor* actor)
{
	mTimeSinceUpdate.Remove(actor);

	if (auto* ngActorId = mActorToGuidMap.Find(actor))
		mGuidToActorMap.Remove(*ngActorId);

	mActorToGuidMap.Remove(actor);
}

ANGActor* ANGSceneManager::GetActorFromNetId(const int32 &id)
{
	if (auto* ngActor = mGuidToActorMap.Find(id))
		return *ngActor;

	return nullptr;
}

int32 ANGSceneManager::GetNetIdFromActor(ANGActor* actor)
{
	if (auto* id = mActorToGuidMap.Find(actor))
		return *id;

	return 0;
}

const TMap<int32, ANGActor*> &ANGSceneManager::GuidActorMap()
{
	return mGuidToActorMap;
}

int32 ANGSceneManager::GetGravityDir(UObject* WorldContext)
{
	int32 dir = -1;
	ANGSceneManager::WithSceneManager(WorldContext, [&](ANGSceneManager &sceneManager) {
		dir = (sceneManager.GravityDir() % 2 == 0) ? 1 : -1;
	});

	return dir;
}

void ANGSceneManager::FlipGravityDir(UObject* WorldContext)
{
	ANGSceneManager::WithSceneManager(WorldContext, [&](ANGSceneManager &sceneManager) {
		sceneManager.SetGravityDir(sceneManager.GravityDir() + 1);
	});
}