// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Client_Packet.h"


#include "CoreMinimal.h"
#include "InGamePacket.h"
#include "GameFramework/Character.h"
#include "Teemo.generated.h"

UCLASS()
class TEEMOESCAPE_5_1_API ATeemo : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ATeemo();

public:
	uint16 npc_index_;
	uint16 client_index;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 카메라 할당
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* tpsCamComp;

	// 총알 공장
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// 총알 발사 처리 함수
	void InputFire();

	// 스나이퍼건 스태틱 메시 추가
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;

	// 유탄총 사용 중인지 여부
	bool bUsingGrenadeGun = true;
	// 유탄총으로 변경
	void ChangeToGrenadeGun();
	// 스나이퍼건 변경
	void ChangeToSniperGun();
	// 스나이퍼 조준
	void SniperAim();
	// 스나이퍼 조준 중인지 여부
	bool bSniperAim = false;

	// 스나이퍼 UI 위젯 공장
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;
	// 스나이퍼 UI 위젯 인스턴스
	class UUserWidget* _sniperUI;

	// 채팅
	UPROPERTY(EditAnywhere, Category = Chtting_Chioce)
	TSubclassOf<class UUserWidget> chattingUIFactory;

	// 총알 파편 효과 공장
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;

	// 일반 조준 크로스헤어 UI위젯
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	// 크로스헤어 인스턴스
	class UUserWidget* _crosshairUI;

	// 채팅 위젯 인스턴스
	class UUserWidget* _chattingUI;

	// 채팅 위젯 인스턴스
	class UUserWidget* _chattingUI2;

	// 달리기 이벤트 처리 함수
	void InputRun();

	// 카메라 셰이크 블루프린트 저장 변수
	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	class USoundBase* bulletSound;

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* ingame_sound;

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* speed_sound;

	// 타이머 함수
	FTimerHandle TimerHandle;
	void OnTimerComplete();


	UFUNCTION()
	void SetPositionInit(FSetMoveInfo move_data);

	UFUNCTION()
	void RemoteSetPositionInit(FSetMoveInfo move_data);

private:
	class AMyPlayerController* PlayerController;  // Reference to the player controller

public:
	// 좌우 회전 입력 처리
	void Turn(float value);
	// 상하 회전 입력 처리
	void LookUp(float value);
public:
	// 걷기속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 200;
	// 달리기 속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 600;
	// 이동방향
	FVector direction;
	
	// 이전 위치
	FVector OldLocation;
	FRotator OldRotator;

	// 현재 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Health)
	int32 hp;

	//초기 HP 값
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 initialHp = 3;

	//피격 당했을때 처리
	UFUNCTION(BlueprintCallable, Category = Health)
	void OnHitEvent();

	// 인게임 접속 확인
	bool in_game_check_ = false;

	// 좌우 이동 입력 이벤트 처리함수
	void InputHorizontal(float value);

	// 상하 이동 입력 이벤트 처리함수
	void InputVertical(float value);

	// 점프 입력 이벤트 처리함수
	void InputJump();

	// 플레이어 이동처리
	void Move();

	// 총 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTeemoGameInstance* teemo_game_instance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UClient_To_Server* teemo_game_client;

	// AEnemy 타입의 블루프린트 할당받을 변수
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class ARemotable_Teemo> remote_teemo_factory;

	// 리모트 티모 생성
	void CreateRemoteTeemo();

private:
	void SpawnCube();
	void SpawnCharacter();
	void Spawn();
	void ForwordCharacter();
};

