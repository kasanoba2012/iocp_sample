// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Packet.h"

// client
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <thread>
#include <queue>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

#include "Client_Packet.h"
#include "InGamePacket.h"
#include "RecvBuffer.h"
// client end

#include "Engine/GameInstance.h"
#include "TeemoGameInstance.generated.h"
/**
 * 
 */

DECLARE_DELEGATE(FClient_Connect); // 접속 델리게이트
DECLARE_DELEGATE(FClient_Damage); // 캐릭터 데미지 피격 델리게이트
// client

DECLARE_DELEGATE_OneParam(FDle_InGameConnect, const uint16&);

DECLARE_DELEGATE_OneParam(FDle_InGameInit, const FSetMoveInfo&);
DECLARE_DELEGATE_OneParam(FDle_MoveInfo, const FSetMoveInfo&); // 캐릭터 오브젝트 매니저용 델리게이트
DECLARE_DELEGATE_OneParam(FDle_MoveInfo_Remote, const FSetMoveInfo&); // 캐릭터 오브젝트 매니저용 델리게이트
DECLARE_DELEGATE_OneParam(FDle_MoveInfo_Remote_Init, const FSetMoveInfo&); // 리모트 캐릭터 초기 위치 알려주기

//Dynamic
//DECLARE_DELEGATE_OneParam(FChat_Broadcast_Success, const FString&); // 채팅 델리게이트
DECLARE_DELEGATE_TwoParams(FChat_Broadcast_Two_Success, const FString&, const FString&); // 채팅 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Dynamic_OneParam, FString, SomeParameter);

// client end

class UserManager;
class Odbc;

// client_max_size
const int client_max_size = 5;
UCLASS()
class TEEMOESCAPE_5_1_API UTeemoGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	bool server_client_check = false;
	bool timer_destroy_sw = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UApp_Server* iocp_net_server_;

	//UPROPERTY(Transient)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UClient_To_Server* client_connect;

public: // 좌표값 받는 곳
	//FVector CurrentLocation;
	TArray<FVector> current_location_array;
	
	int32 index_num = 0;

	//char* signin_id[33];
	char signin_id[33];
	//char signin_id[33] = "minsuhong";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ATeemo* teemo_;

///////////////////
// 패킷처리 추가 //
///////////////////
public:
	FTimerHandle server_timer_handle_;
	FTimerHandle client_timer_login_handle_;
	FTimerHandle client_timer_ingame_handle_;

	void TimerProcessPacket();

	void ProcessPacket();

	bool is_run_process_thread_ = false;

public:
	FClient_Connect Fclient_connect_;

public:

	void PacketInit(const UINT32 max_client);

	void PacketEnd();

	void DbRun();

	void ReceivePacketData(const UINT32 client_index, const UINT32 size, char* P_data);

	void PushSystemPacket(PacketInfo packet);

	std::function<void(UINT32, UINT32, char*)> SendPacketFunc;
	std::function<void(UINT32, UINT32, char*)> BroadCastSendPacketFunc;

	SQLTCHAR* ConvertCharToSqlTCHAR(const char* charArray);

private:
	void CreateCompent(const UINT32 max_client);

	void ClearConnectionInfo(INT32 client_index);

	void EnqueuePacketData(const UINT32 client_index);

	PacketInfo DequePacketData();

	PacketInfo DequeSystemPacketData();

	void ProcessRecvPacket(const UINT32 client_index, const UINT16 packet_id, const UINT16 packet_size, char* P_packet);

	void ProcessuserConnect(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProcessUserDisConnect(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProcessLogin(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProcessSignup(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProcessChatting(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProocessInGame(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProocessPlayerMove(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProocessRemotePlayerInit(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProocessInPlayerDamage(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProocessPacketTest(UINT32 client_index, UINT16 packet_size, char* P_packet);

	typedef void (UTeemoGameInstance::* PROCESS_RECV_PACKET_FUNCTION)(UINT32, UINT16, char*);

	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> recv_funtion_dictionary_;

	UserManager* user_manager_;

	Odbc* odbc;

	std::function<void(int, char*)> send_mq_data_func_;

	std::thread process_thread_;

	std::mutex lock_;

	// 실제 데이터가 왔을때 사용 하는 큐
	std::deque<UINT32> server_in_coming_packet_user_index_;

	// 네트워크 연결 & 끊어짐을 처리하는 큐
	std::deque<PacketInfo> system_packet_queue_;

// client
public:	// Delegate
	FDle_InGameConnect Fuc_in_game_connect;

	FDle_InGameInit Fuc_in_game_init;
	FDle_MoveInfo Fun_move_info_;
	FDle_MoveInfo_Remote Fun_move_info_remote_;
	FDle_MoveInfo_Remote_Init Fun_move_info_remote_init_;
	//FChat_Broadcast_Success Fuc_boradcast_success;
	FChat_Broadcast_Two_Success Fuc_boradcast_two_success;

	FClient_Damage Fuc_Player_Damage;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FDele_Dynamic_OneParam Fuc_Dynamic_OneParam;

	bool in_game_ = false;
	int32 client_index_;

	int32 remote_client_index;

	FSetMoveInfo client_info_array[client_max_size];
public:
	void Server_Connect();

	void Client_Shutdown();

	void Server_Packet_Send(const char* packet, int packet_size);

	void Signin();

	void receive_thread();
	
	void ClientTimerLoginAndChatProcessPacket();
	void ClientTimerInGameProcessPacket();
	void receiveData(int threadId);

	int recvn(SOCKET s, char *buf, int len, int flags);

	// 캐릭터 동기화
public:
	UFUNCTION(BlueprintCallable, Category = Login)
	void InGameAccept();

	FVector DefaultLocation = FVector(-1200.0f, 0.0f, 90.0f);;

public:
	SOCKET sock;
	SOCKADDR_IN sa; // 목적지+포트
	Login_Send_Packet signin_packet;

	std::thread rece_thread;
	std::thread rece_queue_thread;
	std::thread rece_queue_move_info_thread;

	bool recevie_connected = true;

	// Define a queue to store the received data
	std::queue<FSetMoveInfo*> receive_header_check_data_queue; // 로그인 & 채팅 큐
	std::queue<FSetMoveInfo*> receive_ingame_moveinfo_data_queue; // 인게임 큐

	// Define a mutex to ensure thread-safe access to the queue
	std::mutex received_data_mutex;

	//Defind defind;

	Defind defind;

	bool client_check = false;

	// 캐릭터 동기화
	FSetMoveInfo set_move_info_;

	RecvBuffer client_recv_buffer_;
	char client_buffer_[1024];
// client end
};