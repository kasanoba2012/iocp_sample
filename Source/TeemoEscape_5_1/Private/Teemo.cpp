// Fill out your copyright notice in the Description page of Project Settings.


#include "Teemo.h"
#include <GameFramework/SpringArmComponent.h> // 스프링 암 카메라 컴포넌트
#include <Camera/CameraComponent.h> // 카메라 컴포넌트
#include "Bullet.h" // 총알 컴포넌트
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "EnemyFSM.h"
#include "MoveTestActor.h"
#include "Enemy.h"
#include "DrMundoFSM.h"
#include "TeemoGameInstance.h"
#include "Client_To_Server.h"
#include "TeemoAnimInstance.h"
#include "Teemo_Remote_FSM.h"
#include "Teemo_FSM.h"
#include "Remotable_Teemo.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATeemo::ATeemo()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TeemoMesh(TEXT("SkeletalMesh'/Game/Resource/Teemoo/Mesh_Animaiton/Teemo.Teemo'"));
	// 1-1 데이터 로드 성공
	if (TeemoMesh.Succeeded())
	{
		// 1-2 데이터 할당
		GetMesh()->SetSkeletalMesh(TeemoMesh.Object);
		// 1-3 메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		// 1-4 메시 크기 설정
		GetMesh()->SetRelativeScale3D(FVector(1.0f));
	}

	//총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/sound/Teemo/teemo_attack.teemo_attack'"));
	//ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/sound/Teemo/teemo_123.teemo_123'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}

	// 히트 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> temp_Sound2(TEXT("/Script/Engine.SoundWave'/Game/sound/Teemo/teemo_run.teemo_run'"));
	if (temp_Sound2.Succeeded())
	{
		speed_sound = temp_Sound2.Object;
	}

	// 히트 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> temp_Sound(TEXT("/Script/Engine.SoundWave'/Game/sound/Teemo/teemo_123.teemo_123'"));
	if (temp_Sound.Succeeded())
	{
		ingame_sound = temp_Sound.Object;
	}

	// 3. TPS 카메라 할당
	// 3-1 SpingArm 컴포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;

	// 3-2 Camera 컴포넌트 붙이기
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;

	// 총 스켈레탈 메시
	gunMeshComp = CreateAbstractDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/Resource/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	if (TempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-12, 22, 20));
	}

	// 5. 스나이퍼건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	// 5-1 부모 컴포너트를 Mesh로 설정
	sniperGunComp->SetupAttachment(GetMesh());
	// 5-2 스새틱 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/Resource/SniperGun/sniper1.sniper1'"));
	if (TempSniperMesh.Succeeded())
	{
		// 5-4 스태틱 메시 데이터 할당
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		// 5-5 위치 조정
		sniperGunComp->SetRelativeLocation(FVector(-22, 55, 20));
		// 5-6 크기 조정
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}
}

// Called when the game starts or when spawned
void ATeemo::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Teemo BeginPlay()"));

	//초기 hp 세팅
	hp = initialHp;

	// 초기 속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	// 1. 스나이퍼 UI 위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. 일반 조준 UI 크로스헤어 인스턴스 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. 일반 조준 UI등록
	_crosshairUI->AddToViewport();
	
	// 4. 채팅창 등록
	_chattingUI = CreateWidget(GetWorld(), chattingUIFactory);

	_chattingUI->AddToViewport();

	// 기본으로 스나이퍼건 설정
	ChangeToSniperGun();

	//client_connect = NewObject<UClient_To_Server>(this, TEXT("client_to_server"));
	//SetActorLocation(FVector(500.0f, 0.0f, 0.0f));

	teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());

	FVector CurrentLocation = GetActorLocation();

	//teemo_game_client = NewObject<UClient_To_Server>(this, TEXT("client_to_server"));

	teemo_game_instance->current_location_array.Add(CurrentLocation);

	teemo_game_instance->index_num += 1;

	if (teemo_game_instance != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("TeemoPlayer Bigen SetPositionInit"));
		teemo_game_instance->Fuc_in_game_init.BindUFunction(this, FName("SetPositionInit"));
		teemo_game_instance->Fuc_Player_Damage.BindUFunction(this, FName("OnHitEvent"));
	}

	UE_LOG(LogTemp, Warning, TEXT("TeemoPlayer Location : %s, array size : %d"), *CurrentLocation.ToString(), teemo_game_instance->current_location_array.Num());
}

// Called every frame
void ATeemo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();

	// 현재 teemo 회전 값
	FRotator CurrentRotation = GetActorRotation();
	
	// 현재 teemo 위치 값
	FVector CurrentLocation = GetActorLocation();

	// 움직이면 없으면 틱 작동 안함 CurrentLocation : 최근 위치 / OldLocation 이전 위치
	if (teemo_game_instance->timer_destroy_sw && in_game_check_)
	{
		 if (OldLocation != CurrentLocation || OldRotator != CurrentRotation)
		 {
			 SetMoveInfo login_send_packet;
			 login_send_packet.packet_id = (int)CLIENT_PACKET_ID::IN_GAME_MOVE_START;
			 login_send_packet.packet_length = sizeof(login_send_packet);
			 login_send_packet.fvector_ = CurrentLocation;
			 login_send_packet.frotator_ = CurrentRotation;
			 OldLocation = CurrentLocation;
			 OldRotator = CurrentRotation;

			 teemo_game_instance->Server_Packet_Send((char*)&login_send_packet, login_send_packet.packet_length);
		 }
	}
}

// Called to bind functionality to input
void ATeemo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATeemo::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATeemo::LookUp);

	// 좌우 입력 이벤트 처리함수 바인딩
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATeemo::InputHorizontal);
	// 상하 입력 이벤트 처리함수 바인딩
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATeemo::InputVertical);
	// 점프 입력 이벤트 처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATeemo::InputJump);

	// 총알 발사 이벤트 처리 함수
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATeemo::InputFire);
	// 총 교체 이벤트 처리 함수 바인딩
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATeemo::ChangeToGrenadeGun);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATeemo::ChangeToSniperGun);

	// 스나이퍼 조준 모드 이벤트 처리 함수 바인딩
	PlayerInputComponent->BindAction(TEXT("SniperMode"), IE_Pressed, this, &ATeemo::SniperAim);
	PlayerInputComponent->BindAction(TEXT("SniperMode"), IE_Released, this, &ATeemo::SniperAim);

	// 달리기 입력 이벤트 처리 함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATeemo::InputRun);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATeemo::InputRun);

	// 박스 소환 B키
	PlayerInputComponent->BindAction("SpawnCube", IE_Pressed, this, &ATeemo::SpawnCube);

	// 박스 소환 V키
	PlayerInputComponent->BindAction("SpawnCharacter", IE_Pressed, this, &ATeemo::SpawnCharacter);

	// 박스 소환 C키
	PlayerInputComponent->BindAction("Spawn", IE_Pressed, this, &ATeemo::CreateRemoteTeemo);

	// 전진 Q키
	PlayerInputComponent->BindAction("Forword", IE_Pressed, this, &ATeemo::ForwordCharacter);
}

// 유탄총 변경
void ATeemo::ChangeToGrenadeGun()
{
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

// 스나이퍼건 변경
void ATeemo::ChangeToSniperGun()
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

void ATeemo::SniperAim()
{
	// 스나이퍼건 모드가 아니라면 처리하지 않음
	if (bUsingGrenadeGun)
	{
		return;
	}
	// Pressed 입력 처리
	if (bSniperAim == false)
	{
		// 1. 스나이퍼 조준 모드 활성
		bSniperAim = true;
		// 2. 스나이퍼 조준 UI 등록
		_sniperUI->AddToViewport();
		// 3. 카메라 시야각 Field Of View 설정
		tpsCamComp->SetFieldOfView(45.0f);
		// 4. 일반 조준 UI 제거
		_crosshairUI->RemoveFromParent();
	}
	else
	{
		bSniperAim = false;
		_sniperUI->RemoveFromParent();
		tpsCamComp->SetFieldOfView(90.0f);
		_crosshairUI->AddToViewport();
	}
}

void ATeemo::InputRun()
{
	auto movement = GetCharacterMovement();
	// 현재 달리기 모드라면
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		// 걷기 속도로 전환
		movement->MaxWalkSpeed = walkSpeed;
	}
	else {
		UGameplayStatics::PlaySound2D(GetWorld(), speed_sound, 1.0f, 1.0f, 0.0f);
		movement->MaxWalkSpeed = runSpeed;
	}
}

void ATeemo::OnTimerComplete()
{
	UE_LOG(LogTemp, Warning, TEXT("OnTimerComplete"));

	TimerHandle.Invalidate();
}

// 접속 클라이언트 초기값 설정
void ATeemo::SetPositionInit(FSetMoveInfo move_data)
{
	//::MessageBox(NULL, L"SetPositionInit", L"SignIn", 0);
	UGameplayStatics::PlaySound2D(GetWorld(), ingame_sound);
	UE_LOG(LogTemp, Warning, TEXT("%d %d SetMoveInfo fvector : %s SetMoveInfo : %s"), 
	move_data.packet_id, move_data.client_id, *move_data.fvector_.ToString(), *move_data.frotator_.ToString());

	SetActorLocationAndRotation(move_data.fvector_, move_data.frotator_);

	OldLocation = move_data.fvector_;

	teemo_game_instance->in_game_ = true;

	client_index = move_data.client_id; // 서버의 인덱스와 클라 인덱스 통일

	in_game_check_ = true;

	// 전체 클라한테 
	if (in_game_check_ && teemo_game_instance->in_game_)
	{
		SetMoveInfo login_send_packet;
		login_send_packet.packet_id = (int)CLIENT_PACKET_ID::IN_GAME_MOVE_START;
		login_send_packet.packet_length = sizeof(login_send_packet);
		login_send_packet.fvector_ = OldLocation;

		teemo_game_instance->Server_Packet_Send((char*)&login_send_packet, login_send_packet.packet_length);
	}

	// 전체 클라한테 
	if (in_game_check_ && teemo_game_instance->in_game_)
	{
		SetMoveInfo login_send_packet;
		login_send_packet.packet_id = (int)CLIENT_PACKET_ID::IN_GAME_REMOTE_REQUEST;
		login_send_packet.packet_length = sizeof(login_send_packet);

		teemo_game_instance->Server_Packet_Send((char*)&login_send_packet, login_send_packet.packet_length);
	}


}

void ATeemo::RemoteSetPositionInit(FSetMoveInfo move_data)
{
	::MessageBox(NULL, L"SetPositionInit11123", L"SignIn", 0);
	//CreateRemoteTeemo();
}

void ATeemo::Turn(float value)
{
	AddControllerYawInput(value);
}

void ATeemo::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void ATeemo::OnHitEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("Damage_Player"));
	auto anim = Cast<UTeemoAnimInstance>(GetMesh()->GetAnimInstance());
	anim->PlayDamageAnim();
	hp--;
	if (hp <= 0)
	{
		anim->PlayDeadAnim();
		hp = 3;
		UE_LOG(LogTemp, Warning, TEXT("Damage_Player_Dead"));
	}
}

// 좌우입력 이벤트 처리함수
void ATeemo::InputHorizontal(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Player Move Data InputHorizontal : %f / %f"), value, direction.Y);
	direction.Y = value;
}

void ATeemo::InputVertical(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Player Move Data InputVertical : %f"), value);
	direction.X = value;
}

void ATeemo::InputJump()
{
	Jump();
}

void ATeemo::Move()
{
	// 플레이어 이동 처리
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void ATeemo::CreateRemoteTeemo()
{
	FVector NewLocation = FVector(-600, 0, 300); // Set the desired new location for the actor
	FRotator NewRotation = FRotator(0);

	int32 index = 4;
	// 초기 위치 값 받아서 생성하는데 클라이언트 인덱스 값도 같이 넣어주기

	GetWorld()->SpawnActor<ARemotable_Teemo>(remote_teemo_factory, NewLocation, NewRotation);
}

void ATeemo::SpawnCube()
{
	UE_LOG(LogTemp, Warning, TEXT("Log SpawnCube"));

	// 박스 스폰
	// 스폰할 위치 지정
	FVector SpawnLocation = GetActorLocation() + FVector(300.f, 0.f, 0.f);
	FRotator SpawnRotation = GetActorRotation();

	// 월드에 스폰 생성
	GetWorld()->SpawnActor<AMoveTestActor>(AMoveTestActor::StaticClass(), SpawnLocation, SpawnRotation);
}

void ATeemo::SpawnCharacter()
{
	UE_LOG(LogTemp, Warning, TEXT("Log SpawnCharacter"));

	AEnemy* SpawnedActor = GetWorld()->SpawnActor<AEnemy>(FVector(-1000, 300, -88), FRotator(0));

	if (SpawnedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Log SpawnCube"));
		FVector NewLocation = FVector(-1000, 300, 300); // Set the desired new location for the actor
		FRotator NewRotation = FRotator(0);
		SpawnedActor->SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}

void ATeemo::Spawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Log SpawnCharacter"));
}

void ATeemo::ForwordCharacter()
{
	FVector CurrentLocation = GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("move_info_0 : %s"), *CurrentLocation.ToString());

	for(int i = 0; i < teemo_game_instance->current_location_array.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("move_info_index %d : %s"), i, *teemo_game_instance->current_location_array[i].ToString());
	}

	SetMoveInfo login_send_packet;
	login_send_packet.packet_id = (int)CLIENT_PACKET_ID::IN_GAME_MOVE_START;
	login_send_packet.packet_length = sizeof(login_send_packet);
	login_send_packet.fvector_ = CurrentLocation;
	teemo_game_instance->Server_Packet_Send((char*)&login_send_packet, login_send_packet.packet_length);
}

void ATeemo::InputFire()
{
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	// 카메라 세이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	if (bUsingGrenadeGun)
	{
		// 공격 애니메이션 재생
		auto anim = Cast<UTeemoAnimInstance>(GetMesh()->GetAnimInstance());
		anim->PlayAttackAnim();

		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else
	{
		// 공격 애니메이션 재생
		auto anim = Cast<UTeemoAnimInstance>(GetMesh()->GetAnimInstance());
		anim->PlayAttackAnim();

		// LineTrace의 시작위치
		FVector startPos = tpsCamComp->GetComponentLocation();
		// LineTrace의 종료위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// LineTrace 의 충돌 정보를 담을 변수
		FHitResult hitInfo;
		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;
		// 자기 자신 충돌은 제외
		params.AddIgnoredActor(this);
		
		float AttackRange = 1000.f;

		// Channel 필터를 이요한 LineTrace 충돌 검춤
		bool bHit = GetWorld()->LineTraceSingleByChannel(OUT hitInfo, startPos, endPos, ECC_Visibility, params);

		//디버깅을 위해 라인 트레이스를 그립니다.
		DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 1.0f, 0, 1.0f);

		// LineTrace가 부딪혔을때
		if (bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit1 : %s"), hitInfo.GetActor());
			// 충돌처리
			FTransform bulletTrans;
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// 총알 파편 효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			auto hitComp = hitInfo.GetComponent();
			// 1. 만약 컴포넌트에 물리가 적용되어 있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 2. 날려버릴 힘과 방향 필요
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;
				// 3. 그 방향으로 날려버리고싶다.
				hitComp->AddForce(force);
			}

			// 부딪힌 대상이 적인지 판단 CreateDefaultSubobject()생성된 문자열을 GetDefaultSubobjectByName으로 찾음
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("EnemyFSM"));
			if (enemy)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit2 : %s"), hitInfo.GetActor());
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
			// 리모트 티모 피격 판정
			auto teemo = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("TeemoRemoteFSM"));
			if (teemo)
			{
				FVector CollisionLocation = hitInfo.Location;
				UE_LOG(LogTemp, Warning, TEXT("Hit3 : %s"), *CollisionLocation.ToString());
				auto remote_teemo = Cast<UTeemo_Remote_FSM>(teemo);
				remote_teemo->OnDamageProcess();
			}
		}
	}
}