#include "NGActor.h"
#include "NGSceneManager.h"

ANGActor::ANGActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ANGActor::BeginPlay()
{
	ANGSceneManager::WithSceneManager(this, [&](ANGSceneManager &sceneManager) {
		sceneManager.AddActorToNetworkScene(this);
	});

	Super::BeginPlay();
}

void ANGActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANGActor::SetIsExternal(bool isExternal)
{
	mIsExternal = isExternal;
}

bool ANGActor::IsExternal() 
{
	return mIsExternal;
}
