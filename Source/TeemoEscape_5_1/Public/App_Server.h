// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IocpNetServer.h"
#include "Packet.h"
//#include "TeemoGameInstance.h"

#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <memory>

#include "UObject/NoExportTypes.h"
#include "App_Server.generated.h"

const UINT16 SERVER_PORT2 = 10000;
const UINT16 MAX_CLIENT2 = 5;		//총 접속할수 있는 클라이언트 수
const int SLEEP_TIME2 = 3000;
const UINT32 MAX_IO_WORKER_THREAD2 = 4;

/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API UApp_Server : public UObject, public IocpNetServer
{
	GENERATED_BODY()
public:
	UApp_Server();
	virtual~UApp_Server();
public:
	virtual void OnConnect(const UINT32 client_index) override;

	virtual void OnClose(const UINT32 client_index_) override;

	virtual void OnReceive(const UINT32 client_index, const UINT32 size, char* P_recv_data) override;

	void Run(const UINT32 max_client);
	
	void End();

	void ServerStart();

public:
	bool server_check_ = false;

public:
	UPROPERTY(EditAnywhere)
	class UTeemoGameInstance* P_packet_manager_;
};