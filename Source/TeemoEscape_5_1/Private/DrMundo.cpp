// Fill out your copyright notice in the Description page of Project Settings.


#include "DrMundo.h"
#include "DrMundoFSM.h"

// Sets default values
ADrMundo::ADrMundo()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> DrMundoMesh(TEXT("SkeletalMesh'/Game/resource/DrMundo/DrMundo_2.DrMundo_2'"));
	// 1-1 데이터 로드 성공
	if (DrMundoMesh.Succeeded())
	{
		// 1-2 데이터 할당
		GetMesh()->SetSkeletalMesh(DrMundoMesh.Object);
		// 1-3 메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, 0, 0));
		// 1-4 메시 크기 설정
		GetMesh()->SetRelativeScale3D(FVector(1.0f));
	}
	// DrMundoFSM 컴포넌트 추가
	fsm = CreateDefaultSubobject<UDrMundoFSM>(TEXT("FSM"));

	// 애니메이션 블루프린트 할당하기
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Animaitons/ABP_Dr_Mundo.ABP_Dr_Mundo_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}
}

// Called when the game starts or when spawned
void ADrMundo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADrMundo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADrMundo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

