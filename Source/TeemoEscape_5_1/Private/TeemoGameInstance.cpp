// Fill out your copyright notice in the Description page of Project Settings.


#include "TeemoGameInstance.h"
#include "Engine/Engine.h"
#include "Teemo.h"

#include "App_Server.h" // 서버 클래스 [ODBC 연결되어 있음 Packet.h]
//#include "Client_To_Server.h" // 서버 접속 클래스
#include "Odbc.h"

#include "TimerManager.h"
#include "UserManager.h"


void UTeemoGameInstance::Init()
{
	Super::Init();

	if (iocp_net_server_ == nullptr)
	{
		iocp_net_server_ = NewObject<UApp_Server>(this, TEXT("iocp_net_server_"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Start Client!"));

	// 타이머 호출
	float TimerRate = 1.0f / 120.0f;  // 초당 60회

	GetWorld()->GetTimerManager().SetTimer(server_timer_handle_, this, &UTeemoGameInstance::TimerProcessPacket, TimerRate, true);

	GetWorld()->GetTimerManager().SetTimer(client_timer_login_handle_, this, &UTeemoGameInstance::ClientTimerLoginAndChatProcessPacket, TimerRate, true);
	GetWorld()->GetTimerManager().SetTimer(client_timer_ingame_handle_, this, &UTeemoGameInstance::ClientTimerInGameProcessPacket, TimerRate, true);
}

void UTeemoGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Warning, TEXT("Shutdown!"));

	if (iocp_net_server_->server_check_ == true)
	{
		iocp_net_server_->End();

		//delete iocp_net_server_;
	}

	if (timer_destroy_sw)
	{
		Client_Shutdown();
	}
}

// 서버 패킷 처리 리스트
void UTeemoGameInstance::PacketInit(const UINT32 max_client)
{
	// unordered_map
	// https://blog.naver.com/PostView.nhn?blogId=webserver3315&logNo=221678909965&parentCategoryNo=&categoryNo=48&viewDate=&isShowPopularPosts=true&from=search
	recv_funtion_dictionary_ = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	// (PACKET_ID 설정) Key, Value 설정
	recv_funtion_dictionary_[(int)PACKET_ID::SYS_USER_CONNECT] = &UTeemoGameInstance::ProcessuserConnect;
	recv_funtion_dictionary_[(int)PACKET_ID::SYS_USER_DISCONNECT] = &UTeemoGameInstance::ProcessUserDisConnect;
	recv_funtion_dictionary_[(int)PACKET_ID::LOGIN_REQUEST] = &UTeemoGameInstance::ProcessLogin;
	recv_funtion_dictionary_[(int)PACKET_ID::SIGNIN_REQUEST] = &UTeemoGameInstance::ProcessSignup;
	recv_funtion_dictionary_[(int)PACKET_ID::CHAT_SEND_REQUEST] = &UTeemoGameInstance::ProcessChatting;
	recv_funtion_dictionary_[(int)PACKET_ID::IN_GAME_REQUEST] = &UTeemoGameInstance::ProocessInGame;
	recv_funtion_dictionary_[(int)PACKET_ID::IN_GAME_DAMAGE_REQUEST] = &UTeemoGameInstance::ProocessInPlayerDamage;
	recv_funtion_dictionary_[(int)PACKET_ID::IN_GAME_MOVE_START] = &UTeemoGameInstance::ProocessPlayerMove;
	recv_funtion_dictionary_[(int)PACKET_ID::IN_GAME_REMOTE_REQUEST] = &UTeemoGameInstance::ProocessRemotePlayerInit;
	recv_funtion_dictionary_[(int)PACKET_ID::PACKET_TEST_SEND] = &UTeemoGameInstance::ProocessPacketTest;

	CreateCompent(max_client);
}

void UTeemoGameInstance::CreateCompent(const UINT32 max_client)
{
	user_manager_ = new UserManager;
	user_manager_->Init(max_client);
	odbc = new Odbc;
}

void UTeemoGameInstance::PacketEnd()
{
	is_run_process_thread_ = false;

	if (process_thread_.joinable())
	{
		process_thread_.join();
	}
}

void UTeemoGameInstance::ReceivePacketData(const UINT32 client_index, const UINT32 size, char* P_data)
{
	UE_LOG(LogTemp, Warning, TEXT("ReceivePacketData_GameInstance client : %d, size : %d"), client_index, size);
	// GetUserByConnIdx 어떤 클라이언트가 메세지를 보냈는지 확인
	auto P_user = user_manager_->GetUserByConnIdx(client_index);

	// 메세지를 보낸 클라이언트 객체에 데이터를 담음
	P_user->SetPacketData(size, P_data);

	// 큐에 리시브가 발생했다는 것을 알려줌
	EnqueuePacketData(client_index);
}

void UTeemoGameInstance::TimerProcessPacket()
{
	//UE_LOG(LogTemp, Warning, TEXT("TimerProcessPacket"));

	if (auto packet_data = DequePacketData(); packet_data.packet_id_ > (UINT16)PACKET_ID::SYS_END)
	{
		//is_idle = false;
		// 요청이 있는 경우 처리
		ProcessRecvPacket(packet_data.client_index_, packet_data.packet_id_, packet_data.data_size_, packet_data.P_data_ptr_);
	}

	// 시스템 패킷 (연결 & 연결 종료가 발생한 경우)
	if (auto packet_data = DequeSystemPacketData(); packet_data.packet_id_ != 0)
	{
		//is_idle = false;
		// 요청이 있는 경우 처리
		ProcessRecvPacket(packet_data.client_index_, packet_data.packet_id_, packet_data.data_size_, packet_data.P_data_ptr_);
	}

	if (timer_destroy_sw)
	{
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.ClearTimer(server_timer_handle_);
	}
}

void UTeemoGameInstance::ProcessPacket()
{
	UE_LOG(LogTemp, Warning, TEXT("ProcessPacket"));

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

void UTeemoGameInstance::DbRun()
{
	odbc->Init();
	odbc->ConnetMssql(L"odbc_test.dsn");
	// 인벤토리 DB 테스트 코드
	//odbc->Load();
	RECORD record_test;
	SQLTCHAR my_name[255] = L"min";
	//record_test = odbc->InventoryCheck(my_name);

	//odbc->InventoryCheck(my_name);
	// end
	UE_LOG(LogTemp, Warning, TEXT("ProcessPacket"));
}

void UTeemoGameInstance::EnqueuePacketData(const UINT32 client_index)
{
	std::lock_guard<std::mutex> guard(lock_);
	server_in_coming_packet_user_index_.push_back(client_index);
}

PacketInfo UTeemoGameInstance::DequePacketData()
{
	UINT32 user_index = 0;

	{
		std::lock_guard<std::mutex> grard(lock_);
		// 현재 Send 요청을 보낸 유저가 있는지 확인
		if (server_in_coming_packet_user_index_.empty())
		{
			return PacketInfo();
		}

		// 요청한 데이터가 있다면 index 추출
		user_index = server_in_coming_packet_user_index_.front();
		server_in_coming_packet_user_index_.pop_front();
	}

	// 알아낸 index로 유저 객체 가져옴
	auto P_user = user_manager_->GetUserByConnIdx(user_index);
	auto packet_data = P_user->GetPacket();
	packet_data.client_index_ = user_index;

	return packet_data;
}

void UTeemoGameInstance::PushSystemPacket(PacketInfo packet)
{
	std::lock_guard<std::mutex> guard(lock_);
	// 패킷 헤더에 따라서 처리
	system_packet_queue_.push_back(packet);
}

SQLTCHAR* UTeemoGameInstance::ConvertCharToSqlTCHAR(const char* charArray)
{
	int charArrayLen = strlen(charArray) + 1;
	int numChars = MultiByteToWideChar(CP_ACP, 0, charArray, charArrayLen, NULL, 0);

	SQLTCHAR* sqlTCharArray = new SQLTCHAR[numChars];

	MultiByteToWideChar(CP_ACP, 0, charArray, charArrayLen, (LPWSTR)sqlTCharArray, numChars);

	return sqlTCharArray;
}

PacketInfo UTeemoGameInstance::DequeSystemPacketData()
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

void UTeemoGameInstance::ProcessRecvPacket(const UINT32 client_index, const UINT16 packet_id, const UINT16 packet_size, char* P_packet)
{
	// 요청한 패킷 id로 클라이언트 객체 찾음
	auto iter = recv_funtion_dictionary_.find(packet_id);
	if (iter != recv_funtion_dictionary_.end())
	{
		// 찾은 객체로 처리
		(this->*(iter->second))(client_index, packet_size, P_packet);
	}
}

void UTeemoGameInstance::ProcessuserConnect(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	// 새로 연결 받기
	printf("[ProcessUserConnect_Gameinstance] clientIndex: %d\n", client_index);
	// 하나의 user_manger 객체 할당
	auto P_user = user_manager_->GetUserByConnIdx(client_index);
	// 객체 초기화
	P_user->Clear();
}

void UTeemoGameInstance::ProcessUserDisConnect(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	printf("[ProcessUserDisConnect_Gameinstance] clientIndex: %d\n", client_index);
	// 연결이 끊어진 경우 user 반환
	ClearConnectionInfo(client_index);
}

void UTeemoGameInstance::ProcessLogin(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	auto  P_login_req_packet = reinterpret_cast<LOGIN_REQUEST_PACKET*>(P_packet);

	// 로그인시 저장
	strcpy_s(signin_id, sizeof(signin_id), P_login_req_packet->user_id_);
	
	UE_LOG(LogTemp, Warning, TEXT("[Login.cpp1] Id : %s"), signin_id);

	auto P_user_id = ConvertCharToSqlTCHAR(P_login_req_packet->user_id_);
	auto P_user_pw = ConvertCharToSqlTCHAR(P_login_req_packet->user_pw_);
	UE_LOG(LogTemp, Warning, TEXT("[ProcessLogin_Gameinstance] Id : %s / PW : %s\n"), P_user_id, P_user_pw);

	if (odbc->LoginCheckSQL(P_user_id, P_user_pw))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ProcessLogin_Gameinstance] (If) Id : %s / PW : %s\n"), P_user_id, P_user_pw);

		FSetMoveInfo login_res_packet;
		login_res_packet.packet_id = (int)PACKET_ID::LOGIN_RESPONSE_SUCCESS;
		login_res_packet.packet_length = sizeof(login_res_packet);
		login_res_packet.client_id = client_index;
		

		SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ProcessLogin_Gameinstance] (Else) Id : %s / PW : %s\n"), P_user_id, P_user_pw);

		FSetMoveInfo login_res_packet;
		login_res_packet.packet_id = (int)PACKET_ID::LOGIN_RESPONSE_FAIL;
		login_res_packet.packet_length = sizeof(login_res_packet);

		SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	}
}

void UTeemoGameInstance::ProcessSignup(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	dbitem record;

	auto  P_signup_req_packet = reinterpret_cast<LOGIN_REQUEST_PACKET*>(P_packet);

	auto P_user_id = ConvertCharToSqlTCHAR(P_signup_req_packet->user_id_); ;
	auto P_user_pw = ConvertCharToSqlTCHAR(P_signup_req_packet->user_pw_); ;

	UE_LOG(LogTemp, Warning, TEXT("[ProcessSignup_GameInstance] Id : %s / PW : %s\n"), P_user_id, P_user_pw);

	record.name = P_user_id;
	record.pass = P_user_pw;

	if (Fclient_connect_.IsBound() == true)	Fclient_connect_.Execute();

	if (odbc->AddSQL(record))
	{
		//odbc.Load();

		UE_LOG(LogTemp, Warning, TEXT("[ProcessSignup_GameInstance] (If) Id : %s / PW : %s\n"), P_user_id, P_user_pw);

		FSetMoveInfo login_res_packet;
		login_res_packet.packet_id = (int)PACKET_ID::SIGNIN_RESPONSE_SUCCESS;
		login_res_packet.packet_length = sizeof(login_res_packet);
		login_res_packet.client_id = client_index;

		SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ProcessSignup_GameInstance] (Else) Id : %s / PW : %s\n"), P_user_id, P_user_pw);

		FSetMoveInfo login_res_packet;
		login_res_packet.packet_id = (int)PACKET_ID::SIGNIN_RESPONSE_FAIL;
		login_res_packet.packet_length = sizeof(login_res_packet);
		login_res_packet.client_id = client_index;

		SendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
	}
}

void UTeemoGameInstance::ProcessChatting(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	//Defind defind;
	auto  P_login_req_packet = reinterpret_cast<Login_Send_Packet*>(P_packet);

	FSetMoveInfo login_res_packet;
	login_res_packet.packet_id = (int)PACKET_ID::CHAT_SEND_RESPONSE_SUCCESS;
	login_res_packet.packet_length = sizeof(login_res_packet);
	login_res_packet.client_id = client_index;
	strcpy_s(login_res_packet.user_id, sizeof(login_res_packet.user_id), P_login_req_packet->user_id);
	strcpy_s(login_res_packet.in_game_id, sizeof(login_res_packet.user_pw), signin_id);

	UE_LOG(LogTemp, Warning, TEXT("[Login.cpp2] Id : %s"), signin_id);
	UE_LOG(LogTemp, Warning, TEXT("[Login.cpp3] Id : %s"), *defind.CharArrayToFString(login_res_packet.in_game_id));

	UE_LOG(LogTemp, Warning, TEXT("[ProcessChatting Send_GameInstance] packet_id : %d, packet_length : %d packet_data %s\n"),
	login_res_packet.packet_id, login_res_packet.packet_length, *defind.CharArrayToFString(login_res_packet.user_id));

	BroadCastSendPacketFunc(client_index, sizeof(login_res_packet), (char*)&login_res_packet);
}

void UTeemoGameInstance::ProocessInGame(UINT32 client_index, UINT16 packet_size, char* P_packet)
{

	FSetMoveInfo move_info_packet;
	DefaultLocation -= FVector(-200.0f, 0.0f, 0.0f);
	move_info_packet.packet_id = (int)PACKET_ID::IN_GAME_SUCCESS;
	move_info_packet.packet_length = sizeof(move_info_packet);
	move_info_packet.client_id = client_index;
	move_info_packet.fvector_ = DefaultLocation;
	move_info_packet.frotator_ = FRotator(0.0f, 0.0f, 0.0f);
	UE_LOG(LogTemp, Warning, TEXT("[ProocessInGame_GameInstance] fvector_ : %s / frotator_ : %s\n"), *move_info_packet.fvector_.ToString(), *move_info_packet.frotator_.ToString());

	// 클라 접속 시 초기값을 서버 array에 저장
	client_info_array[client_index].client_id = client_index;
	client_info_array[client_index].fvector_ = DefaultLocation;
	client_info_array[client_index].frotator_ = FRotator(0.0f, 0.0f, 0.0f);;

	UE_LOG(LogTemp, Warning, TEXT("init start0 %s"), *client_info_array[client_index].fvector_.ToString());

	BroadCastSendPacketFunc(client_index, sizeof(move_info_packet), (char*)&move_info_packet);
}

void UTeemoGameInstance::ProocessPlayerMove(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	UE_LOG(LogTemp, Warning, TEXT("init start1"));
	auto  P_move_info_packet = reinterpret_cast<FSetMoveInfo*>(P_packet);
	FSetMoveInfo move_info_packet;
	move_info_packet.packet_id = (int)PACKET_ID::IN_GAME_MOVE_END;
	move_info_packet.packet_length = sizeof(move_info_packet);
	move_info_packet.client_id = client_index;
	move_info_packet.fvector_ = P_move_info_packet->fvector_;
	move_info_packet.frotator_ = P_move_info_packet->frotator_;

	// 움직임 발생시 새로운 값을 넣음 client index 움직임이 발생한 클라이언트 
	client_info_array[client_index].client_id = client_index;
	client_info_array[client_index].fvector_ = P_move_info_packet->fvector_;
	client_info_array[client_index].frotator_ = P_move_info_packet->frotator_;
	client_info_array[client_index].in_game_connect = 1;

	UE_LOG(LogTemp, Warning, TEXT("pakcet_test_move_2  packet_id : %d / fvector_ : %s / frotator_ : %s"), move_info_packet.packet_id, *move_info_packet.fvector_.ToString(), *move_info_packet.frotator_.ToString());

	UE_LOG(LogTemp, Warning, TEXT("[move_info_2_GameInstance] client : %d / fvector_ : %s / frotator_ : %s\n"), client_index, *move_info_packet.fvector_.ToString(), *move_info_packet.frotator_.ToString());

	BroadCastSendPacketFunc(client_index, sizeof(move_info_packet), (char*)&move_info_packet);
}

void UTeemoGameInstance::ProocessRemotePlayerInit(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	UE_LOG(LogTemp, Warning, TEXT("init start2"));

	for (int i = 0; i < client_max_size; i++)
	{			
		if (client_index != client_info_array[i].client_id && client_info_array[i].in_game_connect == 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("init start3  client_id : %d / fvector_ : %s / frotator_ : %s / in_game_connect %d"),
				client_info_array[i].client_id, *client_info_array[i].fvector_.ToString(), *client_info_array[i].frotator_.ToString(), client_info_array[i].in_game_connect);
			// 서버에 접속한 유저 알려주기


			FSetMoveInfo remote_teemo_init_packet;
			remote_teemo_init_packet.packet_id = (int)PACKET_ID::IN_GAME_REMOTE_RESPONSE;
			remote_teemo_init_packet.packet_length = sizeof(remote_teemo_init_packet);
			remote_teemo_init_packet.client_id = i;
			remote_teemo_init_packet.fvector_ = client_info_array[i].fvector_;
			remote_teemo_init_packet.frotator_ = client_info_array[i].frotator_;
			remote_teemo_init_packet.in_game_connect = client_info_array[i].in_game_connect;

			UE_LOG(LogTemp, Warning, TEXT("init start3_1  client_id : %d / fvector_ : %s / frotator_ : %s / in_game_connect %d packet_id %d"),
				remote_teemo_init_packet.client_id, *remote_teemo_init_packet.fvector_.ToString(), *remote_teemo_init_packet.frotator_.ToString(), remote_teemo_init_packet.in_game_connect
				, remote_teemo_init_packet.packet_id);

			SendPacketFunc(client_index, sizeof(remote_teemo_init_packet), (char*)&remote_teemo_init_packet);
		}
	}
}

void UTeemoGameInstance::ProocessInPlayerDamage(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	// 피격 대상 클라이언트에게 피격 사실 전송
	auto  P_move_info_packet = reinterpret_cast<FSetMoveInfo*>(P_packet);

	UE_LOG(LogTemp, Warning, TEXT("Damage_Client %d"),
		P_move_info_packet->client_id);

	FSetMoveInfo move_info_packet;
	move_info_packet.packet_id = (int)PACKET_ID::IN_GAME_MAGAGE_SUCCESS;

	SendPacketFunc(P_move_info_packet->client_id, sizeof(move_info_packet), (char*)&move_info_packet);
}

void UTeemoGameInstance::ProocessPacketTest(UINT32 client_index, UINT16 packet_size, char* P_packet)
{
	auto  P_move_info_packet = reinterpret_cast<FSetMoveInfo*>(P_packet);
	FSetMoveInfo move_info_packet;
	move_info_packet.packet_id = (int)PACKET_ID::PACKET_TEST_RECV;
	move_info_packet.packet_length = sizeof(move_info_packet);
	move_info_packet.client_id = client_index;
	move_info_packet.fvector_ = P_move_info_packet->fvector_;
	move_info_packet.frotator_ = P_move_info_packet->frotator_;

	UE_LOG(LogTemp, Warning, TEXT("PacketTest2_server_to_client  client : %d / fvector_ : %s / frotator_ : %s"),
	P_move_info_packet->packet_id, *P_move_info_packet->fvector_.ToString(), *P_move_info_packet->frotator_.ToString());

	SendPacketFunc(client_index, sizeof(move_info_packet), (char*)&move_info_packet);
}

void UTeemoGameInstance::ClearConnectionInfo(INT32 client_index)
{
	auto P_req_user = user_manager_->GetUserByConnIdx(client_index);

	if (P_req_user->GetDomainState() != User::DOMAIN_STATE::NONE)
	{
		user_manager_->DeleteUserInfo(P_req_user);
	}
}

// client
void UTeemoGameInstance::Server_Connect()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	/*----------------------
	SOCKET 생성
	-----------------------*/
	sock = socket(AF_INET, SOCK_STREAM, 0);

	short sData = 10000;
	short tData = 0x2710;
	short fData = 0x1027;

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");
	sa.sin_port = htons(10000);

	/*----------------------
		SOCKET 연결
		-----------------------*/
	int iRet = connect(sock, (sockaddr*)&sa, sizeof(sa));

	u_long iMode = TRUE;
	/*----------------------언리얼 엔진 vector deleting destructor'() 에러 원인이 뭐야 : End시 Thread에 join하고 끝내야함 안하면 이지랄남
	SOCKET 논블럭킹 설정  | ioctlsocket
	-----------------------*/
	ioctlsocket(sock, FIONBIO, &iMode);

	
	// 이때 만드는 쓰레드를 증가해보자 ...
	rece_thread = std::thread(&UTeemoGameInstance::receive_thread, this);

	client_check = true;
}

void UTeemoGameInstance::Client_Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("[Client_Shutdown]\n"));
	recevie_connected = false;

	rece_thread.join();
	//rece_queue_thread.join();
	//rece_queue_move_info_thread.join();

	closesocket(sock);
	WSACleanup();
}

void UTeemoGameInstance::Server_Packet_Send(const char* packet, int packet_size)
{
	UE_LOG(LogTemp, Warning, TEXT("[Server_Packet_Send] sendData : %s size : %d\n"), packet, packet_size);

	send(sock, packet, packet_size, 0);
}

void UTeemoGameInstance::Signin()
{	
	send(sock, (char*)&signin_packet, sizeof(signin_packet), 0);
}

void UTeemoGameInstance::receive_thread()
{
	//char client_buffer[1024];
	int result;

	while (recevie_connected)
	{
		// recv() return 최소1 최대 len
		result = recv(sock, client_buffer_, sizeof(client_buffer_), 0);
		// TODO 이곳에서 데이터 전송 받는거 확인하기
		if (result > 0)
		{
			FSetMoveInfo Login_data;
			// Login_Send_Packet 구조체 밀어버리고
			ZeroMemory(&Login_data, sizeof(Login_data));
			// Login_Send_Packet에 서버에서 온 buffer 내용 저장
			CopyMemory(&Login_data, client_buffer_, sizeof(Login_data));

			UE_LOG(LogTemp, Warning, TEXT("[client_to_server_receive_thread] packet id : %d data : %s\n"), Login_data.packet_id, *defind.CharArrayToFString(Login_data.user_id));

			// 로그인 채팅 관련 패킷 : 400이하
			if (Login_data.packet_id <= 400)
			{
				std::lock_guard<std::mutex> lock(received_data_mutex);

				// 받은 데이터 큐에 밀어 넣기
				receive_header_check_data_queue.push(&Login_data);
			}
			// 인게임 관련 패킷 400이상
			else if (Login_data.packet_id > 400 && client_check)
			{
				FSetMoveInfo in_game_move_data_;
				ZeroMemory(&in_game_move_data_, sizeof(in_game_move_data_));
				CopyMemory(&in_game_move_data_, client_buffer_, sizeof(in_game_move_data_));
				UE_LOG(LogTemp, Warning, TEXT("[client_to_server_receive_thread else] packet id : %d size : %d fvector : %s frotator : %s\n"),
					in_game_move_data_.packet_id, in_game_move_data_.packet_length, *in_game_move_data_.fvector_.ToString(), *in_game_move_data_.frotator_.ToString());

				std::lock_guard<std::mutex> lock(received_data_mutex);

				// 받은 데이터 큐에 밀어 넣기
				receive_ingame_moveinfo_data_queue.push(&in_game_move_data_);
			}
		}
	}
}

void UTeemoGameInstance::ClientTimerLoginAndChatProcessPacket()
{
	//UE_LOG(LogTemp, Warning, TEXT("ClientTimerLoginAndChatProcessPacket!"));

	// 대기열에 액세스하기 위한 잠금 획득
	std::lock_guard<std::mutex> lock(received_data_mutex);

	// 큐에 받은 데이터가 있는지 확인
	if (!receive_header_check_data_queue.empty()) {
		// 대기열에서 처음 받은 데이터 가져오기
		FSetMoveInfo* received_data = receive_header_check_data_queue.front();

		switch (received_data->packet_id)
		{
		case (UINT32)CLIENT_PACKET_ID::LOGIN_RESPONSE_SUCCESS:
			//::MessageBox(NULL, L"Signin_Success", L"SignIn", 0);
			UE_LOG(LogTemp, Warning, TEXT("[client_to_server_receive_switch 201] packet id : %d\n"), received_data->packet_id);
			// 캐릭터 선택창 델리게이트 달어야하지만 인게임 진입으로 일단 변경
			if (Fuc_in_game_connect.IsBound() == true) Fuc_in_game_connect.Execute(received_data->client_id);
			break;
		case (UINT32)CLIENT_PACKET_ID::LOGIN_RESPONSE_FAIL:
			//::MessageBox(NULL, L"Signin_Fail", L"SignIn", 0);
			UE_LOG(LogTemp, Warning, TEXT("[client_to_server_receive_switch 202] packet id : %d\n"), received_data->packet_id);
			break;
		case (UINT32)CLIENT_PACKET_ID::SIGNIN_RESPONSE_SUCCESS:
			//::MessageBox(NULL, L"Signup_Success", L"Signup", 0);
			UE_LOG(LogTemp, Warning, TEXT("[client_to_server_receive_switch 203] packet id : %d\n"), received_data->packet_id);
			break;
		case (UINT32)CLIENT_PACKET_ID::SIGNIN_RESPONSE_FAIL:
			//::MessageBox(NULL, L"Signup_Fail", L"Signup", 0);
			UE_LOG(LogTemp, Warning, TEXT("[client_to_server_receive_switch 204] packet id : %d\n"), received_data->packet_id);
			break;
		case (UINT32)CLIENT_PACKET_ID::CHAT_SEND_RESPONSE_SUCCESS:
			//::MessageBox(NULL, L"BroadCast Msg", L"Signup", 0);
			// TODO 여기에 델리게이트 달아서 챗 메세지 들어온거 확인하기                     
			UE_LOG(LogTemp, Warning, TEXT("[Login.cpp5] msg : %s id : %s "), *defind.CharArrayToFString(received_data->user_id), *defind.CharArrayToFString(received_data->in_game_id));
			//if (Fuc_boradcast_success.IsBound() == true) Fuc_boradcast_success.Execute(*defind.CharArrayToFString(received_data->user_id));
			if (Fuc_boradcast_two_success.IsBound() == true) Fuc_boradcast_two_success.Execute(*defind.CharArrayToFString(received_data->user_id), *defind.CharArrayToFString(received_data->in_game_id));
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("[client_to_server_receive_switch default] packet id : %d\n"), received_data->packet_id);
			break;
		}
		receive_header_check_data_queue.pop();
	}	
}

void UTeemoGameInstance::ClientTimerInGameProcessPacket()
{
	//UE_LOG(LogTemp, Warning, TEXT("ClientTimerInGameProcessPacket!"));

	//대기열에 액세스하기 위한 잠금 획득
	std::lock_guard<std::mutex> lock(received_data_mutex);

	// 큐에 받은 데이터가 있는지 확인
	if (!receive_ingame_moveinfo_data_queue.empty()) {
		// 대기열에서 처음 받은 데이터 가져오기
		FSetMoveInfo* received_ingmae_data_ = receive_ingame_moveinfo_data_queue.front();

		switch (received_ingmae_data_->packet_id)
		{
		case (UINT32)CLIENT_PACKET_ID::IN_GAME_SUCCESS:
			UE_LOG(LogTemp, Warning, TEXT("[402] packet index : %d data : %s, index : %d\n"), received_ingmae_data_->client_id, *received_ingmae_data_->fvector_.ToString(), client_index_);
			// 캐릭터 선택창 델리게이트 달어야하지만 인게임 진입으로 일단 변경
			if (received_ingmae_data_->client_id == client_index_)
			{
				// 실제 플레이어 초기 위치 세팅
				if (Fuc_in_game_init.IsBound() == true) Fuc_in_game_init.Execute(*received_ingmae_data_);
			}		
			break;
		case (UINT32)CLIENT_PACKET_ID::IN_GAME_MOVE_END:
			UE_LOG(LogTemp, Warning, TEXT("pakcet_test_move_3"));
			UE_LOG(LogTemp, Warning, TEXT("[move_info_3] packet id : %d vector : %s rotator : %s\n"),
				received_ingmae_data_->packet_id, *received_ingmae_data_->fvector_.ToString(), *received_ingmae_data_->frotator_.ToString());
			// 델리게이트 달아서 매니저로 보내자 일단 본인이 보낸 브로드캐스트면 무시		
			//if (Fun_move_info_.IsBound() == true) Fun_move_info_.Execute(*received_data);
			// 리모트 티모 움직일 좌표의 델리게이트
			if (Fun_move_info_remote_.IsBound() == true) Fun_move_info_remote_.Execute(*received_ingmae_data_);
			break;
		case (UINT32)CLIENT_PACKET_ID::IN_GAME_MAGAGE_SUCCESS:
			UE_LOG(LogTemp, Warning, TEXT("Damage_IN_GAME_MAGAGE_SUCCESS"));
			// 실제 플레이어 초기 위치 세팅
			if (Fuc_Player_Damage.IsBound() == true)	Fuc_Player_Damage.Execute();
			/*UE_LOG(LogTemp, Warning, TEXT("PacketTest3_client_end packet id : %d vector : %s rotator : %s\n"),
				received_ingmae_data_->packet_id, *received_ingmae_data_->fvector_.ToString(), *received_ingmae_data_->frotator_.ToString());*/
			// 델리게이트 달아서 매니저로 보내자 일단
			//if (Fun_move_info_.IsBound() == true) Fun_move_info_.Execute(*received_ingmae_data_);
			break;
		case (UINT32)CLIENT_PACKET_ID::IN_GAME_REMOTE_RESPONSE: // 이미 접속한 유저 리모트로 그리기
			UE_LOG(LogTemp, Warning, TEXT("[init start3_2] client_id : %d vector : %s rotator : %s\n"),
				received_ingmae_data_->client_id, *received_ingmae_data_->fvector_.ToString(), *received_ingmae_data_->frotator_.ToString());
			if (Fun_move_info_remote_init_.IsBound() == true) Fun_move_info_remote_init_.Execute(*received_ingmae_data_);
			break;

			
		case (UINT32)CLIENT_PACKET_ID::PACKET_TEST_RECV:
			UE_LOG(LogTemp, Warning, TEXT("PacketTest3_client_end packet id : %d vector : %s rotator : %s\n"),
				received_ingmae_data_->packet_id, *received_ingmae_data_->fvector_.ToString(), *received_ingmae_data_->frotator_.ToString());
			// 델리게이트 달아서 매니저로 보내자 일단
			//if (Fun_move_info_.IsBound() == true) Fun_move_info_.Execute(*received_ingmae_data_);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("[client_to_server_receive_switch default] packet id : %d\n"), received_ingmae_data_->packet_id);
			break;
		}
		receive_ingame_moveinfo_data_queue.pop();
	}

	if (server_client_check)
	{
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.ClearTimer(client_timer_login_handle_);
		timerManager.ClearTimer(client_timer_ingame_handle_);
	}
}

void UTeemoGameInstance::receiveData(int threadId)
{
	// 여기에서 수신 논리를 수행하십시오.
	std::cout << "Thread " << threadId << " receiving data..." << std::endl;

	// 수신 데이터 시뮬레이션
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::cout << "Thread " << threadId << " received data." << std::endl;
}

int UTeemoGameInstance::recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				break;
			}
		} 
		else if(received == 0)
			break;

		//if(received == SOCKET_ERROR)
		//	return SOCKET_ERROR;
		//else if(received == 0)
		//	break;

		left -= received;
		ptr += received;
	}
	return (len - left);
}

void UTeemoGameInstance::InGameAccept()
{
	UE_LOG(LogTemp, Warning, TEXT("[InGameAccept]\n"));

	Login_Send_Packet login_send_packet;
	login_send_packet.packet_id = (int)CLIENT_PACKET_ID::IN_GAME_REQUEST;
	login_send_packet.packet_length = sizeof(login_send_packet);

	Server_Packet_Send((char*)&login_send_packet, login_send_packet.packet_length);
}
// client end