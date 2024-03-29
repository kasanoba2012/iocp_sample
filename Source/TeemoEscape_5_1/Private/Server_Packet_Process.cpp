// Fill out your copyright notice in the Description page of Project Settings.


#include "Server_Packet_Process.h"
#include <utility>
#include <cstring>
#include "UserManager.h"
#include <Windows.h>
#include <sqltypes.h>

#include "Client_Packet.h"

void UServer_Packet_Process::Init(const UINT32 max_client)
{
	// unordered_map
	// https://blog.naver.com/PostView.nhn?blogId=webserver3315&logNo=221678909965&parentCategoryNo=&categoryNo=48&viewDate=&isShowPopularPosts=true&from=search
	recv_funtion_dictionary_ = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	// (PACKET_ID 설정) Key, Value 설정
	recv_funtion_dictionary_[(int)PACKET_ID::SYS_USER_CONNECT] = &UServer_Packet_Process::ProcessuserConnect;
	recv_funtion_dictionary_[(int)PACKET_ID::SYS_USER_DISCONNECT] = &UServer_Packet_Process::ProcessUserDisConnect;
	recv_funtion_dictionary_[(int)PACKET_ID::LOGIN_REQUEST] = &UServer_Packet_Process::ProcessLogin;
	recv_funtion_dictionary_[(int)PACKET_ID::SIGNIN_REQUEST] = &UServer_Packet_Process::ProcessSignup;
	recv_funtion_dictionary_[(int)PACKET_ID::CHAT_SEND_REQUEST] = &UServer_Packet_Process::ProcessChatting;
	recv_funtion_dictionary_[(int)PACKET_ID::IN_GAME_REQUEST] = &UServer_Packet_Process::ProocessInGame;
	recv_funtion_dictionary_[(int)PACKET_ID::IN_GAME_MOVE_START] = &UServer_Packet_Process::ProocessInPlayerMove;

	CreateCompent(max_client);
}

void UServer_Packet_Process::CreateCompent(const UINT32 max_client)
{
	user_manager_ = new UserManager;
	user_manager_->Init(max_client);
}


bool UServer_Packet_Process::Run()
{
	//이 부분을 패킷 처리 부분으로 이동 시킨다.
	is_run_process_thread_ = true;
	// 패킷 쓰레드 생성
	process_thread_ = std::thread([this]() {ProcessPacket(); });

	return true;
}

void UServer_Packet_Process::End()
{
	is_run_process_thread_ = false;

	if (process_thread_.joinable())
	{
		process_thread_.join();
	}
}

void UServer_Packet_Process::ReceivePacketData(const UINT32 client_index, const UINT32 size, char* P_data)
{
	// GetUserByConnIdx 어떤 클라이언트가 메세지를 보냈는지 확인
	auto P_user = user_manager_->GetUserByConnIdx(client_index);

	// 메세지를 보낸 클라이언트 객체에 데이터를 담음
	P_user->SetPacketData(size, P_data);
	
	// TODO 여기에 리시브 내용을 GameInstance에 넣어야됨?

	// 큐에 리시브가 발생했다는 것을 알려줌
	EnqueuePacketData(client_index);
}

void UServer_Packet_Process::ProcessPacket()
{
	while (is_run_process_thread_)
	{
		bool is_idle = true;

		// 이미 연결이 된 유저가 보낸 요청이 있는 경우
		if (auto packet_data = DequePacketData(); packet_data.packet_id_ > (UINT16)PACKET_ID::SYS_END)
		{
			is_idle = false;
			// 요청이 있는 경우 처리
			ProcessRecvPacket(packet_data.client_index_, packet_data.packet_id_, packet_data.data_size_, packet_data.P_data_ptr_);
		}

		// 시스템 패킷 (연결 & 연결 종료가 발생한 경우)
		if (auto packet_data = DequeSystemPacketData(); packet_data.packet_id_ != 0)
		{
			is_idle = false;
			// 요청이 있는 경우 처리
			ProcessRecvPacket(packet_data.client_index_, packet_data.packet_id_, packet_data.data_size_, packet_data.P_data_ptr_);
		}

		if (is_idle)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

void UServer_Packet_Process::EnqueuePacketData(const UINT32 client_index)
{
	std::lock_guard<std::mutex> guard(lock_);
	//in_coming_packet_user_index_.push_back(client_index);
}

PacketInfo UServer_Packet_Process::DequePacketData()
{
	UINT32 user_index = 0;

	{
		std::lock_guard<std::mutex> grard(lock_);
		// 현재 Send 요청을 보낸 유저가 있는지 확인
		if (in_coming_packet_user_index_.empty())
		{
			return PacketInfo();
		}

		// 요청한 데이터가 있다면 index 추출
		user_index = in_coming_packet_user_index_.front();
		in_coming_packet_user_index_.pop_front();
	}

	// 알아낸 index로 유저 객체 가져옴
	auto P_user = user_manager_->GetUserByConnIdx(user_index);
	auto packet_data = P_user->GetPacket();
	packet_data.client_index_ = user_index;

	return packet_data;
}

void UServer_Packet_Process::PushSystemPacket(PacketInfo packet)
{
	std::lock_guard<std::mutex> guard(lock_);
	// 패킷 헤더에 따라서 처리
	//system_packet_queue_.push_back(packet);
}

void UServer_Packet_Process::DbRun()
{
	odbc.Init();
	odbc.ConnetMssql(L"odbc_test.dsn");
	odbc.Load();
}

SQLTCHAR* UServer_Packet_Process::ConvertCharToSqlTCHAR(const char* charArray)
{
	int charArrayLen = strlen(charArray) + 1;
	int numChars = MultiByteToWideChar(CP_ACP, 0, charArray, charArrayLen, NULL, 0);

	SQLTCHAR* sqlTCharArray = new SQLTCHAR[numChars];

	MultiByteToWideChar(CP_ACP, 0, charArray, charArrayLen, (LPWSTR)sqlTCharArray, numChars);

	return sqlTCharArray;
}

PacketInfo UServer_Packet_Process::DequeSystemPacketData()
{
	std::lock_guard<std::mutex> guard(lock_);
	if (system_packet_queue_.empty())
	{
		return PacketInfo();
	}

	auto packet_data = system_packet_queue_.front();
	system_packet_queue_.pop_front();

	return packet_data;
}

void UServer_Packet_Process::ProcessRecvPacket(const UINT32 client_index, const UINT16 packet_id, const UINT16 packet_size, char* P_packet)
{
	// 요청한 패킷 id로 클라이언트 객체 찾음
	auto iter = recv_funtion_dictionary_.find(packet_id);
	if (iter != recv_funtion_dictionary_.end())
	{
		// 찾은 객체로 처리
		(this->*(iter->second))(client_index, packet_size, P_packet);
	}
}

void UServer_Packet_Process::ProcessuserConnect(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	// 새로 연결 받기
	printf("[ProcessUserConnect] clientIndex: %d\n", client_index);
	// 하나의 user_manger 객체 할당
	auto P_user = user_manager_->GetUserByConnIdx(client_index);
	// 객체 초기화
	P_user->Clear();
}

void UServer_Packet_Process::ProcessUserDisConnect(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	printf("[ProcessUserDisConnect] clientIndex: %d\n", client_index);
	// 연결이 끊어진 경우 user 반환
	ClearConnectionInfo(client_index);
}

void UServer_Packet_Process::ProcessLogin(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	auto  P_login_req_packet = reinterpret_cast<LOGIN_REQUEST_PACKET*>(P_packet);

	auto P_user_id = ConvertCharToSqlTCHAR(P_login_req_packet->user_id_);
	auto P_user_pw = ConvertCharToSqlTCHAR(P_login_req_packet->user_pw_);
	UE_LOG(LogTemp, Warning, TEXT("[ProcessLogin1] Id : %s / PW : %s\n"), P_user_id, P_user_pw);

	if (odbc.LoginCheckSQL(P_user_id, P_user_pw))
	{

		UE_LOG(LogTemp, Warning, TEXT("[ProcessLogin] (If) Id : %s / PW : %s\n"), P_user_id, P_user_pw);

		Login_Send_Packet login_res_packet;
		login_res_packet.packet_id = (int)PACKET_ID::LOGIN_RESPONSE_SUCCESS;
		login_res_packet.packet_length = sizeof(login_res_packet);
		login_res_packet.clinet_id = client_index;

		SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ProcessLogin] (Else) Id : %s / PW : %s\n"), P_user_id, P_user_pw);

		Login_Send_Packet login_res_packet;
		login_res_packet.packet_id = (int)PACKET_ID::LOGIN_RESPONSE_FAIL;
		login_res_packet.packet_length = sizeof(login_res_packet);

		SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	}

	// TODO. 중복 로그인 안되는 기능
	//if (user_manager_->GetCurrentUserCnt() >= user_manager_->GetMaxUserCnt())
	//{
	//	//접속자수가 최대수를 차지해서 접속불가
	//	login_res_packet.result_ = (UINT16)ERROR_CODE::LOGIN_USER_USED_ALL_OBJ;
	//	SendPacketFunc(client_index, sizeof(LOGIN_RESPONSE_PACKET), (char*)&login_res_packet);
	//	return;
	//}

	// TODO. 허용 접속 인원보다 많으면 차단
	////여기에서 이미 접속된 유저인지 확인하고, 접속된 유저라면 실패한다.
	//if (user_manager_->FindUserIndexByID(P_user_id) == -1)
	//{
	//	login_res_packet.result_ = (UINT16)ERROR_CODE::NONE;
	//	SendPacketFunc(client_index, sizeof(LOGIN_RESPONSE_PACKET), (char*)&login_res_packet);
	//}
	//else
	//{
	//	//접속중인 유저여서 실패를 반환한다.
	//	login_res_packet.result_ = (UINT16)ERROR_CODE::LOGIN_USER_ALREADY;
	//	SendPacketFunc(client_index, sizeof(LOGIN_REQUEST_PACKET), (char*)&login_res_packet);
	//	return;
	//}
}

void UServer_Packet_Process::ProcessSignup(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	dbitem record;

	auto  P_signup_req_packet = reinterpret_cast<LOGIN_REQUEST_PACKET*>(P_packet);

	auto P_user_id = ConvertCharToSqlTCHAR(P_signup_req_packet->user_id_); ;
	auto P_user_pw = ConvertCharToSqlTCHAR(P_signup_req_packet->user_pw_); ;

	UE_LOG(LogTemp, Warning, TEXT("[ProcessSignup] Id : %s / PW : %s\n"), P_user_id, P_user_pw);

	record.name = P_user_id;
	record.pass = P_user_pw;

	if (Fclient_connect_.IsBound() == true)	Fclient_connect_.Execute();

	if (odbc.AddSQL(record))
	{
		//odbc.Load();

		UE_LOG(LogTemp, Warning, TEXT("[ProcessSignup] (If) Id : %s / PW : %s\n"), P_user_id, P_user_pw);

		Login_Send_Packet login_res_packet;
		login_res_packet.packet_id = (int)PACKET_ID::SIGNIN_RESPONSE_SUCCESS;
		login_res_packet.packet_length = sizeof(login_res_packet);
		login_res_packet.clinet_id = client_index;

		SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ProcessSignup] (Else) Id : %s / PW : %s\n"), P_user_id, P_user_pw);

		Login_Send_Packet login_res_packet;
		login_res_packet.packet_id = (int)PACKET_ID::SIGNIN_RESPONSE_FAIL;
		login_res_packet.packet_length = sizeof(login_res_packet);
		login_res_packet.clinet_id = client_index;

		SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	}
}

void UServer_Packet_Process::ProcessChatting(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	Defind defind;
	auto  P_login_req_packet = reinterpret_cast<Login_Send_Packet*>(P_packet);

	Login_Send_Packet login_res_packet;
	login_res_packet.packet_id = (int)PACKET_ID::CHAT_SEND_RESPONSE_SUCCESS;
	login_res_packet.packet_length = sizeof(login_res_packet);
	login_res_packet.clinet_id = client_index;
	strcpy_s(login_res_packet.user_id, sizeof(login_res_packet.user_id), P_login_req_packet->user_id);

	UE_LOG(LogTemp, Warning, TEXT("[ProcessChatting Send] packet_id : %d, packet_length : %d packet_data %s\n"),
		login_res_packet.packet_id, login_res_packet.packet_length, *defind.CharArrayToFString(login_res_packet.user_id));

	BroadCastSendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	//SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
}

void UServer_Packet_Process::ProocessInGame(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	SetMoveInfo move_info_packet;
	move_info_packet.packet_id = (int)PACKET_ID::IN_GAME_SUCCESS;
	move_info_packet.packet_length = sizeof(move_info_packet);
	move_info_packet.clinet_id = client_index;
	move_info_packet.fvector_ = FVector(100.0f, 0.0f, 0.0f);
	move_info_packet.frotator_ = FRotator(0.0f, 0.0f, 500.0f);

	UE_LOG(LogTemp, Warning, TEXT("[ProocessInGame] fvector_ : %s / frotator_ : %s\n"), *move_info_packet.fvector_.ToString(), *move_info_packet.frotator_.ToString());

	BroadCastSendPacketFunc(client_index, sizeof(move_info_packet), (char*)&move_info_packet);
	//SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
}

void UServer_Packet_Process::ProocessInPlayerMove(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	auto  P_move_info_packet = reinterpret_cast<SetMoveInfo*>(P_packet);
	SetMoveInfo move_info_packet;
	move_info_packet.packet_id = (int)PACKET_ID::IN_GAME_MOVE_END;
	move_info_packet.packet_length = sizeof(move_info_packet);
	move_info_packet.clinet_id = client_index;
	move_info_packet.fvector_ = P_move_info_packet->fvector_;
	move_info_packet.frotator_ = P_move_info_packet->frotator_;

	UE_LOG(LogTemp, Warning, TEXT("[move_info_2] client : %d / fvector_ : %s / frotator_ : %s\n"), client_index, *move_info_packet.fvector_.ToString(), *move_info_packet.frotator_.ToString());

	BroadCastSendPacketFunc(client_index, sizeof(move_info_packet), (char*)&move_info_packet);
}

void UServer_Packet_Process::ClearConnectionInfo(INT32 client_index)
{
	auto P_req_user = user_manager_->GetUserByConnIdx(client_index);

	if (P_req_user->GetDomainState() != User::DOMAIN_STATE::NONE)
	{
		user_manager_->DeleteUserInfo(P_req_user);
	}
}










