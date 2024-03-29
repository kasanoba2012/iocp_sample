// Fill out your copyright notice in the Description page of Project Settings.


#include "DrMundoFSM.h"
#include "Teemo.h"
#include "DrMundo.h"
#include <Kismet/GameplayStatics.h>
#include "TeemoEscape_5_1.h"
#include <Components/CapsuleComponent.h>

// Sets default values for this component's properties
UDrMundoFSM::UDrMundoFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDrMundoFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// 월드에서 ATPSPlayer 타깃 찾아오기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATeemo::StaticClass());
	// ATPSPlayer 타입으로 캐스팅
	target = Cast<ATeemo>(actor);
	// 소유 객체 가져오기
	me = Cast<ADrMundo>(GetOwner());
	
}


// Called every frame
void UDrMundoFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (mState)
	{
	case EDrMundoState::Idle:
		IdleState();
		break;
	case EDrMundoState::Move:
		MoveState();
		break;
	case EDrMundoState::Attack:
		AttackState();
		break;
	case EDrMundoState::Damage:
		DamageState();
		break;
	case EDrMundoState::Die:
		DieState();
		break;
	}
}

void UDrMundoFSM::IdleState()
{
	// 1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 만약 경과 시간이 대기 시간을 초과했다면
	if (currentTime > idleDelayTime)
	{
		// 3. 이동 상태로 전환하고 싶다.
		mState = EDrMundoState::Move;
		// 경과 시간 초기화 
		currentTime = 0;
	}
}

void UDrMundoFSM::MoveState()
{
	// 1. 타깃 목적지가 필요하다.
	FVector destination = target->GetActorLocation();
	// 2. 방향이 필요하다.
	FVector dir = destination - me->GetActorLocation();
	// 3. 방향으로 이동하고 싶다.
	me->AddMovementInput(dir.GetSafeNormal());

	// 타깃과 가까워지면 공격 상태로 전환하고 싶다.
	// 1. 만약 거리가 공격 범위 안에 들어오면
	if (dir.Size() < attackRange)
	{
		// 2. 공격 상태로 전환하고 싶다.
		mState = EDrMundoState::Attack;
	}
}

void UDrMundoFSM::AttackState()
{
	// 목표: 일정 시간에 한 번씩 공격하고 싶다.
	// 1. 시간이 흘러야 한다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 공격 시간이 됐으니까
	if (currentTime > attackDelayTime)
	{
		// 3. 공격하고 싶다.
		UE_LOG(LogTemp, Warning, TEXT("Attack!!"));
		//PRINT_LOG(TEXT("Attack!!!!!"));
		// 경과 시간 초기화
		currentTime = 0;
	}

	// 목표: 타깃이 공격 범위를 벗어나면 상태를 이동으로 전환하고 싶다.
	// 1. 타깃과의 거리가 필요하다.
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	// 2. 타깃과의 거리가 공격 범위를 벗어났으니까
	if (distance > attackRange)
	{
		// 3. 상태를 이동으로 전환하고 싶다.
		mState = EDrMundoState::Move;
	}
}

void UDrMundoFSM::DamageState()
{
	// 1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 만약 경과 시간이 대기 시간을 초과했다면
	if (currentTime > damageDelayTime)
	{
		// 3. 대기 상태로 전환하고 싶다.
		mState = EDrMundoState::Idle;
		// 경과 시간 초기화 
		currentTime = 0;
	}
}

void UDrMundoFSM::DieState()
{
	// 계속 아래로 내려가고 싶다.
	// 등속운동 공식 P = P0 + vt
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);

	// 1. 만약 2미터 이상 내려왔다면
	if (P.Z < -200.0f)
	{
		// 2. 제거시킨다.
		me->Destroy();
	}
}

void UDrMundoFSM::OnDamageProcess()
{
	// 체력 감소
	hp--;

	// 만약 체력이 남아있다면
	if (hp > 0)
	{
		// 상태를 피격으로 전환
		mState = EDrMundoState::Damage;
	}
	// 그렇지 않다면
	else
	{
		// 상태를 죽음으로 전환
		mState = EDrMundoState::Die;
		// 캡슐 충돌체 비활성화
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

