// Fill out your copyright notice in the Description page of Project Settings.


#include "Teemo_FSM.h"
#include "Remotable_Teemo.h"
#include "Teemo.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>

// Sets default values for this component's properties
UTeemo_FSM::UTeemo_FSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UTeemo_FSM::OnDamageProcess()
{
	// 체력감소
	hp--;
	UE_LOG(LogTemp, Warning, TEXT("Teemo_FSM %d"), hp);

	if (hp == 0)
	{	
		// 사망처리
		me->Destroy();
	}
	
}

// Called when the game starts
void UTeemo_FSM::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// 소유 객체 가져오기
	me = Cast<ARemotable_Teemo>(GetOwner());
	
}


// Called every frame
void UTeemo_FSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

