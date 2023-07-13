// Fill out your copyright notice in the Description page of Project Settings.


#include "Remotable_Teemo.h"
#include "Bullet.h" // 총알 컴포넌트
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "EnemyFSM.h"
#include "Enemy.h"
#include "Teemo_Remote_FSM.h"
#include "TeemoGameInstance.h"
#include "Client_To_Server.h"
#include "TeemoAnimInstance.h"

// Sets default values
ARemotable_Teemo::ARemotable_Teemo()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> Remote_TeemoMesh(TEXT("SkeletalMesh'/Game/Resource/Teemoo/Mesh_Animaiton/Teemo.Teemo'"));
	// 1-1 데이터 로드 성공
	if (Remote_TeemoMesh.Succeeded())
	{
		// 1-2 데이터 할당
		GetMesh()->SetSkeletalMesh(Remote_TeemoMesh.Object);
		// 1-3 메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		// 1-4 메시 크기 설정
		GetMesh()->SetRelativeScale3D(FVector(1.0f));
	}

	// Remote_Teemo_FSM 컴포넌트 추가
	fsm = CreateDefaultSubobject<UTeemo_Remote_FSM>(TEXT("TeemoRemoteFSM"));

	//총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/sound/Teemo/teemo_attack.teemo_attack'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
	
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

ARemotable_Teemo::ARemotable_Teemo(const FObjectInitializer& ObjectInitializer, int32 client_index)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 리모트 티모 index 할당
	//remote_teemo_client_index = client_index;

	// 1. 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> Remote_TeemoMesh(TEXT("SkeletalMesh'/Game/Resource/Teemoo/Mesh_Animaiton/Teemo.Teemo'"));
	// 1-1 데이터 로드 성공
	if (Remote_TeemoMesh.Succeeded())
	{
		// 1-2 데이터 할당
		GetMesh()->SetSkeletalMesh(Remote_TeemoMesh.Object);
		// 1-3 메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		// 1-4 메시 크기 설정
		GetMesh()->SetRelativeScale3D(FVector(1.0f));
	}

	// Remote_Teemo_FSM 컴포넌트 추가
	fsm = CreateDefaultSubobject<UTeemo_Remote_FSM>(TEXT("TeemoRemoteFSM"));

	//총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/sound/Teemo/teemo_attack.teemo_attack'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}

	// 히트 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> temp_Sound(TEXT("/Script/Engine.SoundWave'/Game/sound/Teemo/teemo_hit.teemo_hit'"));
	if (temp_Sound.Succeeded())
	{
		hitSound = temp_Sound.Object;
	}

	// 히트 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> temp_Sound2(TEXT("/Script/Engine.SoundWave'/Game/sound/Teemo/teemo_hit.teemo_hit'"));
	if (temp_Sound.Succeeded())
	{
		dead_sound = temp_Sound.Object;
	}

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

//ARemotable_Teemo::ARemotable_Teemo(int32 client_index)
//{
//	
//}

// Called when the game starts or when spawned
void ARemotable_Teemo::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Remotable_Teemo BeginPlay()"));

	// 소유 액터 가져오기
	//me = Cast<ARemotable_Teemo>(GetOwner());

	NewLocation = FVector(-1000, -400, 88); // Set the desired new location for the actor
	NewRotation = FRotator(0);

	//초기 hp 세팅
	//hp = initialHp;

	// 초기 속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	// 기본으로 스나이퍼건 설정
	ChangeToSniperGun();

	teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());

	if (teemo_game_instance != nullptr)
	{
		teemo_game_instance->remote_client_index += 1;
		remote_client = teemo_game_instance->remote_client_index;

		UE_LOG(LogTemp, Warning, TEXT("Remotable_Teemo Bigen SetPositionInit %d"), teemo_game_instance->remote_client_index);
		// 초기값 설정 델리게이트
		//teemo_game_instance->Fuc_in_game_remote_init.BindUFunction(this, FName("SetPositionInit"));

		//teemo_game_instance->Fun_move_info_remote_.BindUFunction(this, FName("RemoteTeemoMoveChange"));
	}
}

// Called every frame
void ARemotable_Teemo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARemotable_Teemo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARemotable_Teemo::InputFire()
{
}

void ARemotable_Teemo::ChangeToGrenadeGun()
{
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

void ARemotable_Teemo::ChangeToSniperGun()
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

void ARemotable_Teemo::InputRun()
{
	auto movement = GetCharacterMovement();
	// 현재 달리기 모드라면
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		// 걷기 속도로 전환
		movement->MaxWalkSpeed = walkSpeed;
	}
	else {
		movement->MaxWalkSpeed = runSpeed;
	}
}

void ARemotable_Teemo::SoundHit()
{
	UGameplayStatics::PlaySound2D(GetWorld(), hitSound);
}

void ARemotable_Teemo::SoundDead()
{
	UGameplayStatics::PlaySound2D(GetWorld(), dead_sound);
}

void ARemotable_Teemo::SetPositionInit(FSetMoveInfo move_data)
{
	::MessageBox(NULL, L"SetPositionInit_Remote", L"SignIn", 0);
	// 월드에 접속한 플레이어의 위치값 받아서 스폰 시키기

	UE_LOG(LogTemp, Warning, TEXT("SetPositionInit client id : %d, length : %d vector : %s"),
		move_data.client_id, move_data.packet_length, *move_data.fvector_.ToString());
}

void ARemotable_Teemo::RemoteTeemoMoveChange(FSetMoveInfo move_data)
{
	UE_LOG(LogTemp, Warning, TEXT("remote_teemo_move_change client id : %d, length : %d vector : %s"),
		move_data.client_id, move_data.packet_length, *move_data.fvector_.ToString());

	// TODO 여기 로직을 변경해야되네 생성순으로 
	if (move_data.client_id != teemo_game_instance->client_index_)
	{
		UE_LOG(LogTemp, Warning, TEXT("move_to_remote"));
		// 특정 인덱스의 리모트 티모를 어떻게 움직일까 ?
		if (remote_teemo_client_index == 4)
		{
			SetActorLocationAndRotation(move_data.fvector_, move_data.frotator_);
		}		
	}
}

void ARemotable_Teemo::InputJump()
{
	Jump();
}

void ARemotable_Teemo::OnHitEvent()
{
	//UE_LOG(LogTemp, Warning, TEXT("Remote_OnHitEvent : %d"), hp);
	//hp--;
	//hp--;
	//UE_LOG(LogTemp, Warning, TEXT("Player Damaged : %d"), hp);
	//if (hp <= 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Player Dead"));
	//}
}


