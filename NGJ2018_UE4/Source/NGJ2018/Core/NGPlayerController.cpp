#include "NGPlayerController.h"
#include "Networking/UDPSender.h"

void ANGPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void ANGPlayerController::TestSendPacket(const FNGPacket &packet)
{
	AUDPSender::WithUDPSender(this, [packet](AUDPSender &sender) {
		sender.SendNetData(packet);
	});
}