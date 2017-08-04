//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"
#include "AIController.h"
#include "FCPatrolPoint.h"
#include "FCAIController.generated.h"

enum class AIState { Idle, OnPatrol, Alerted, Engaged };

UCLASS()
class FIFTHCOLUMN_API AFCAIController : public AAIController
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(transient)
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorComp;

	virtual void Possess(class APawn* InPawn) override;

	void NotifyActorBeginOverlap(class AActor* Other) override;

	//set next patrol point
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void NextPatrolPoint();

	//get blackboard variables
	UFUNCTION(BlueprintCallable, Category = Behavior)
		class AFCCharacter* GetTarget() const;

	UFUNCTION(BlueprintCallable, Category = Behavior)
		class AFCCharacter* GetEnemy() const;

	//listen to sounds
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void ListenToDisturbances();

	//look for characters
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void LookForCharacters();

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void FindPointNearTarget();

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void HoldPosition();

	//mark interest
	void MarkInterest(class AActor* InActor);

	//determine character is an enemy
	void DetermineEnemy(class APawn* InPawn);

	//shoot at enemy
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void ShootEnemy();

	//alert friendlies
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void AlertFriendlies();

	//patrols
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void Patrol();

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void SetPatrolPointLocations();

	UFUNCTION(BlueprintCallable, Category = Behavior)
		bool IsPatrolling() const;

	UFUNCTION(BlueprintCallable, Category = Behavior)
		bool IsEngagingEnemy() const;
	
	//flee

	//switch weapon
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void SwitchWeapon();

protected:

	//current AI state
	AIState CurrentState;

	int32 SpottedCountdown;

	int32 NoiseCountdown;

	int32 LongestSndDistance;

	FBlackboard::FKey Destination;

	AActor* CurrentInterest;

	//A list of all actors attached to the character, used for patrol points
	TArray<AActor*> AttachedActors;
	TArray<AFCPatrolPoint*> PatrolPoints;
	TArray<FVector> PatrolPointLocations;
	int32 PatrolPointIteration;
};
