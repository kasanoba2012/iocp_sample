// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Teemo_FSM.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEEMOESCAPE_5_1_API UTeemo_FSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTeemo_FSM();

public:
	// 타깃
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATeemo* target;

	// 소유 액터
	UPROPERTY()
	class ARemotable_Teemo* me;

	// 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 hp = 3;

public:
	// 피격 알림 이벤트 함수
	void OnDamageProcess();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
