// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Packet.h"

#include <unordered_map>
#include <deque>
#include <functional>
#include <thread>
#include <mutex>

#include "Server_Packet_Process.generated.h"

DECLARE_DELEGATE(FClient_Connect); // 접속 델리게이트

class UserManager;
/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API UServer_Packet_Process : public UObject
{
	GENERATED_BODY()
	
public:
	FClient_Connect Fclient_connect_;

public:
	void Init(const UINT32 max_client);

	bool Run();

	void End();

	void ReceivePacketData(const UINT32 client_index, const UINT32 size, char* P_data);

	void PushSystemPacket(PacketInfo packet);

	void DbRun();

	std::function<void(UINT32, UINT32, char*)> SendPacketFunc;
	std::function<void(UINT32, UINT32, char*)> BroadCastSendPacketFunc;

	SQLTCHAR* ConvertCharToSqlTCHAR(const char* charArray);

public:
	Odbc odbc;

private:
	void CreateCompent(const UINT32 max_client);

	void ClearConnectionInfo(INT32 client_index);

	void EnqueuePacketData(const UINT32 client_index);

	PacketInfo DequePacketData();

	PacketInfo DequeSystemPacketData();

	void ProcessPacket();

	void ProcessRecvPacket(const UINT32 client_index, const UINT16 packet_id, const UINT16 packet_size, char* P_packet);

	void ProcessuserConnect(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProcessUserDisConnect(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProcessLogin(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProcessSignup(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProcessChatting(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProocessInGame(UINT32 client_index, UINT16 packet_size, char* P_packet);

	void ProocessInPlayerMove(UINT32 client_index, UINT16 packet_size, char* P_packet);

	typedef void (UServer_Packet_Process::* PROCESS_RECV_PACKET_FUNCTION)(UINT32, UINT16, char*);

	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> recv_funtion_dictionary_;

	UserManager* user_manager_;

	std::function<void(int, char*)> send_mq_data_func_;

	bool is_run_process_thread_ = false;

	std::thread process_thread_;

	std::mutex lock_;

	// 실제 데이터가 왔을때 사용 하는 큐
	std::deque<UINT32> in_coming_packet_user_index_;

	// 네트워크 연결 & 끊어짐을 처리하는 큐
	std::deque<PacketInfo> system_packet_queue_;

	
};
