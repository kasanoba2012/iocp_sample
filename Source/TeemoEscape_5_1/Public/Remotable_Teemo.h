// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGamePacket.h"
#include "GameFramework/Character.h"
#include "Remotable_Teemo.generated.h"

UCLASS()
class TEEMOESCAPE_5_1_API ARemotable_Teemo : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARemotable_Teemo();

	// 매개변수가 있는 사용자 정의 생성자
	ARemotable_Teemo(const FObjectInitializer& ObjectInitializer, int32 client_index);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// 총알 공장
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// 총알 파편 효과 공장
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;

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

	// 스나이퍼 조준 중인지 여부
	bool bSniperAim = false;

	// 달리기 이벤트 처리 함수
	void InputRun();

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* bulletSound;

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* hitSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* dead_sound;

	void SoundHit();
	void SoundDead();

	UFUNCTION()
	void SetPositionInit(FSetMoveInfo move_data);


	UFUNCTION()
	void RemoteTeemoMoveChange(FSetMoveInfo move_data);

	FVector NewLocation;
	FRotator NewRotation;

	// 소유 액터
	UPROPERTY()
	class ARemotable_Teemo* me;

	UCapsuleComponent* CapsuleComponent;

	int32 remote_teemo_client_index;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



public:
	// 걷기속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 200;
	
	// 달리기 속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 600;
	
	// 이동방향
	FVector direction;

	//// 현재 체력
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	//int32 hp = 3;

	////초기 HP 값
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	//int32 initialHp = 10;

	//피격 당했을때 처리
	UFUNCTION(BlueprintCallable, Category = Health)
	void OnHitEvent();

	// 점프 입력 이벤트 처리함수
	void InputJump();

	// 총 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	int32 remote_client;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTeemoGameInstance* teemo_game_instance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UClient_To_Server* teemo_game_client;

	public:
		// 적 AI 관리 컴포넌트 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	class UTeemo_Remote_FSM* fsm;
};
