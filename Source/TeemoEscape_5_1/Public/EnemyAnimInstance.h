// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category=FSM)
	EEnemyState animState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	bool bAttackPlay = false;

	// 공격 애니메이션이 끝나는 이벤트 함수
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
	void OnEndAttakAnimation();

	// 피격 애니메이션 재생 함수
	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void PlayDamageAnim(FName sectionName);

	// 죽음 상태 애니메이션 종료 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	bool bDieDone = false;

};
