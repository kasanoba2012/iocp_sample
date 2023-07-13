// Fill out your copyright notice in the Description page of Project Settings.


#include "Teemo_Remote_FSM.h"
#include "Remotable_Teemo.h"
#include "Teemo.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include "TeemoGameInstance.h"
#include "Client_Packet.h"
#include "Sound/SoundBase.h"


// Sets default values for this component's properties
UTeemo_Remote_FSM::UTeemo_Remote_FSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;



}

// Called when the game starts
void UTeemo_Remote_FSM::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// 소유 객체 가져오기
	me = Cast<ARemotable_Teemo>(GetOwner());

	UWorld* World = GetWorld();
	if (World)
	{
		teemo_game_instance = Cast<UTeemoGameInstance>(World->GetGameInstance());
	}

	//teemo_game_instance = Cast<UTeemoGameInstance>(GetgameIn)
	//teemo_game_instance = Cast<UTeemoGameInstance>(World->);
	//UE_LOG(LogTemp, Warning, TEXT("Teemo_Remote_FSM %d"), me_index->remote_teemo_index);
}


void UTeemo_Remote_FSM::OnDamageProcess()
{
	// 데미지 감소 패킷전송
	SetMoveInfo login_send_packet;
	login_send_packet.packet_id = (int)CLIENT_PACKET_ID::IN_GAME_DAMAGE_REQUEST;
	login_send_packet.clinet_id = me->remote_teemo_client_index;
	login_send_packet.packet_length = sizeof(login_send_packet);
	teemo_game_instance->Server_Packet_Send((char*)&login_send_packet, login_send_packet.packet_length);
	me->SoundHit();
	// 체력감소
	hp--;
	UE_LOG(LogTemp, Warning, TEXT("Teemo_Remote_FSM %d index : %d"), hp, me->remote_teemo_client_index);
	
	if (hp == 0)
	{
		// 사망처리
		//me->Destroy();
		me->SoundDead();
		hp = 3;
	}
}




// Called every frame
void UTeemo_Remote_FSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

