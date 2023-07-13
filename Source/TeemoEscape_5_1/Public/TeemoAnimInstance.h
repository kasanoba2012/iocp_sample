// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TeemoAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API UTeemoAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 플레이어 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float speed = 0;

	// 매 프레임 갱신 되는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 플레이어가 공중에 있는지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;

	// 재생할 공격 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* attackAnimMontage;

	// 피격 애니메이션 재생 함수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* DamageMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* DeadMontage;

	// 공격 애니메이션 재생 함수
	void PlayAttackAnim();

	// 피격 애니메이션 재생 함수
	void PlayDamageAnim();

	// 사망 애니메이션 재생 함수
	void PlayDeadAnim();
};
