// Fill out your copyright notice in the Description page of Project Settings.


#include "TeemoAnimInstance.h"
#include "Teemo.h"
#include <GameFramework/CharacterMovementComponent.h>

void UTeemoAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 플레이어의 이동 속도를 가져와 speed에 할당하고 싶다.
	// 1. 소유 폰 얻어 오기
	auto ownerPwan = TryGetPawnOwner();
	// 2. 플레이어로 캐스팅 하기
	auto player = Cast<ATeemo>(ownerPwan);
	// 캐스팅 성공시
	if (player)
	{
		// 3. 이동 속도가 필요
		FVector velocity = player->GetVelocity();
		// 4. 플레이어의 전방 벡터가 필요
		FVector forwardVector = player->GetActorForwardVector();
		// 5. speed에 값 내적 
		speed = FVector::DotProduct(forwardVector, velocity);

		// 플레이어 현재 공중 있는지 여부 기억
		auto movement = player->GetCharacterMovement();
		isInAir = movement->IsFalling();
	}
}

void UTeemoAnimInstance::PlayAttackAnim()
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayAttackAnim"));
	Montage_Play(attackAnimMontage);
}

void UTeemoAnimInstance::PlayDamageAnim()
{
	Montage_Play(DamageMontage);
}

void UTeemoAnimInstance::PlayDeadAnim()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayDead"));
	Montage_Play(DeadMontage);
}
