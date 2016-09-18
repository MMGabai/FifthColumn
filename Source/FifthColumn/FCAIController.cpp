//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"
#include "BehaviorTree/BlackboardComponent.h"

AFCAIController::AFCAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));
	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));

	CurrentState = AIState::OnPatrol;

	PatrolPointIteration = 0;
	NoiseCountdown = 0;
	SpottedCountdown = 0;
}

void AFCAIController::Possess(APawn* InPawn) 
{
	Super::Possess(InPawn);

	ANPC* NPC = Cast<ANPC>(InPawn);
	UBehaviorTree* BehaviorTree = NPC->NPCBehavior;

	// start behavior
	if (BehaviorTree)
	{
		UBlackboardData &BBAsset = *BehaviorTree->BlackboardAsset;
		BlackboardComp->InitializeBlackboard(BBAsset);

		BehaviorComp->StartTree(*BehaviorTree);
	}
}

void AFCAIController::NotifyActorBeginOverlap(class AActor* Other) 
{
	Super::NotifyActorBeginOverlap(Other);
}

void AFCAIController::NextPatrolPoint() 
{
	if (CurrentState == AIState::OnPatrol)
	{
		if (PatrolPointIteration < PatrolPointLocations.Num() - 1)
			PatrolPointIteration++;
		else
			PatrolPointIteration = 0;
	}
}

//determine what is the current character of interest
class AFCCharacter* AFCAIController::GetTarget() const 
{
	if (BlackboardComp)
		return Cast<AFCCharacter>(BlackboardComp->GetValueAsObject("Target"));

	return NULL;
}

//determine who the current enemy is
class AFCCharacter* AFCAIController::GetEnemy() const 
{
	if (BlackboardComp)
		return Cast<AFCCharacter>(BlackboardComp->GetValueAsObject("Enemy"));

	return NULL;
}

void AFCAIController::LookForCharacters() 
{
	APawn* MyBot = GetPawn();
	if (MyBot == NULL || GetEnemy() != nullptr)
		return;

	const FVector MyLoc = MyBot->GetActorLocation();
	const FRotator MyRotation = MyBot->GetActorRotation();
	AActor* BestMatch = NULL;
	TArray<APawn> IgnoreList;

	for (int32 i = 0; i < GetLevel()->Actors.Num(); i++) 
	{
		APawn* TestActor = Cast<APawn>(GetLevel()->Actors[i]);
		AFCCharacter* TestCharacter = Cast<AFCCharacter>(TestActor);

		if (TestActor != nullptr)
		{
			const float DistSq = (TestActor->GetActorLocation() - MyLoc).SizeSquared();

			float Opposite;
			float Adjacent;

			if (MyRotation.Euler().Z > 315.f && MyRotation.Euler().Z < 45.f)
			{
				Adjacent = FMath::Abs(TestActor->GetActorLocation().Y - MyLoc.Y);
				Opposite = FMath::Abs(TestActor->GetActorLocation().X - MyLoc.X);
			}
			else if (MyRotation.Euler().Z > 270.f && MyRotation.Euler().Z < 90.f)
			{
				Opposite = FMath::Abs(TestActor->GetActorLocation().Y - MyLoc.Y);
				Adjacent = FMath::Abs(TestActor->GetActorLocation().X - MyLoc.X);
			}

			const float Degrees = FMath::Tan(Opposite / Adjacent)
				- MyRotation.Euler().Z;
			
			if (Degrees > -90 && Degrees < 90 && LineOfSightTo(TestActor) && DistSq < 5000000.0f)
			{
				DetermineEnemy(TestActor);
				AFCCharacter* Enemy = GetEnemy();

				if (Enemy != NULL && Cast<AFCCharacter>(MyBot)->CurrentWeapon && Enemy->IsAlive())
				{
					BestMatch = TestActor;

					SpottedCountdown += 5;
					if (SpottedCountdown > 10)
						CurrentState = AIState::Alerted;

					if (SpottedCountdown > 25)
						CurrentState = AIState::Engaged; //Enemy seen

					break;
				}
				else if (TestCharacter && !(TestCharacter->IsAlive()) && CurrentState == AIState::OnPatrol) //alert on watching corpses
					CurrentState = AIState::Alerted;
				else
				{
					//cooldown
					if (SpottedCountdown != 0)
						SpottedCountdown--;

					if (SpottedCountdown == 0)
						CurrentState = AIState::OnPatrol;
				}
			}
		}
	}

	BestMatch ? MarkInterest(BestMatch) : NULL;
}

void AFCAIController::ListenToDisturbances() 
{
	APawn* MyBot = GetPawn();
	if (MyBot == NULL)
		return;

	const FVector MyLoc = MyBot->GetActorLocation();
	AActor* BestMatch = NULL;
	float BestDistSq = 100000.0f;
	FVector Loc = FVector::ZeroVector;

	if (CurrentState == AIState::OnPatrol) 
	{
		TTransArray<AActor*> It = GetLevel()->Actors;

		for (int32 i = 0; i < GetLevel()->Actors.Num(); i++) 
		{
			ADisturbanceSource* Sound = Cast<ADisturbanceSource>(It[i]);

			if (Sound)
			{
				int32 Loudness = Sound->GetLoudness();
				const float DistSq = (Sound->GetActorLocation() - MyLoc).SizeSquared();
				LongestSndDistance = DistSq;

				if (DistSq < BestDistSq * (Loudness * 2.5f))
				{
					NoiseCountdown += Loudness;
					BestMatch = Sound;
					Loc = Sound->GetActorLocation();
				}
			}
		}

		//careful, something might be there
		if (NoiseCountdown >= 10) 
		{
			CurrentState = AIState::Alerted;
			BehaviorComp->GetBlackboardComponent()->SetValueAsVector("Destination", Loc);
		}

		//noise countdown reached critical threshold, hostiles are definitely there
		if (NoiseCountdown >= 25) 
		{
			CurrentState = AIState::Engaged;
			BehaviorComp->GetBlackboardComponent()->SetValueAsVector("Destination", Loc);
		}

		if (BestMatch) 
		{
			BlackboardComp->SetValueAsObject("Sound", BestMatch);
			MarkInterest(BestMatch);
		}
	}

	//cooldown
	if (NoiseCountdown == 0) 
	{
		CurrentState = AIState::OnPatrol;
		BlackboardComp->SetValueAsObject("Enemy", NULL);
	}
	else NoiseCountdown--;
}

void AFCAIController::HoldPosition() 
{
	UBehaviorTreeComponent* MyComp = BehaviorComp;
	APawn* MyBot = Super::GetPawn();

	MyComp->GetBlackboardComponent()->SetValueAsVector("Destination", MyBot->GetActorLocation());
}

void AFCAIController::FindPointNearTarget()
{
	UBehaviorTreeComponent* MyComp = BehaviorComp;	
	APawn* MyBot = Super::GetPawn();
	AFCCharacter* Target = GetTarget();

	if (Target && MyBot) 
	{
		const float SearchRadius = 100.0f;
		const FVector SearchOrigin = Target->GetActorLocation() + 600.0f * (MyBot->GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
		FVector Loc = UNavigationSystem::GetRandomReachablePointInRadius(MyBot, SearchOrigin, SearchRadius);
		Loc != FVector::ZeroVector ? MyComp->GetBlackboardComponent()->SetValueAsVector("Destination", Loc) :
			MyComp->GetBlackboardComponent()->SetValueAsVector("Destination", MyBot->GetActorLocation());
	}
}

void AFCAIController::MarkInterest(class AActor* InActor) 
{
	if (BlackboardComp)
	{
		CurrentInterest = InActor;
		BlackboardComp->SetValueAsObject("Target", InActor);
		SetFocus(InActor);
		if (Cast<APawn>(InActor))
			DetermineEnemy(Cast<APawn>(InActor));
	}
}

void AFCAIController::DetermineEnemy(class APawn* InPawn) 
{
	if (BlackboardComp && Cast<ANPC>(GetPawn())->IsEnemyFor(InPawn->GetController()))
	{
		BlackboardComp->SetValueAsObject("Enemy", InPawn);
		HoldPosition(); //Enemy detected - hold your position in the hope of ambushing the enemy
	}
}

void AFCAIController::ShootEnemy()
{
	ANPC* MyBot = Cast<ANPC>(GetPawn());
	AFCCharacter* Enemy = GetEnemy();
	
	CurrentState = AIState::Engaged;
	MarkInterest(Enemy);

	if (Enemy == GetTarget() && Enemy->IsAlive())
	{
		//TODO: Check for Friendly Fire
		MyBot->StartWeaponFire();

		//TODO: Make sure it fires in salvos
	}
	else 
	{
		MyBot->StopWeaponFire();
		CurrentState = AIState::Alerted; //the danger may not have completely subsided
	}
}

//this will sent signals to nearby friendlies, telling them to become hostile to the player, overriding faction disposition
void AFCAIController::AlertFriendlies() 
{

}

void AFCAIController::Patrol() 
{
	APawn* MyBot = Super::GetPawn();
	FVector Loc = FVector::ZeroVector;

	if (PatrolPoints.Num() > 0 && NoiseCountdown < 25 && SpottedCountdown < 25) 
		Loc = PatrolPointLocations[PatrolPointIteration];

	if (Loc != FVector::ZeroVector)
		BehaviorComp->GetBlackboardComponent()->SetValueAsVector("Destination", Loc);

	return;
}

//Look for patrol points
void AFCAIController::SetPatrolPointLocations()
{
	APawn* MyBot = Super::GetPawn();

	if (AttachedActors.Num() < 1)
		MyBot->GetAttachedActors(AttachedActors);

	if (PatrolPoints.Num() < 1)
		for (int i = 0; i < AttachedActors.Num(); i++)
		{
		AFCPatrolPoint* PatrolPoint = Cast<AFCPatrolPoint>(AttachedActors[i]);

		if (PatrolPoint)
			PatrolPoints.Add(PatrolPoint);
		}

	for (int i = 0; i < PatrolPoints.Num(); i++)
		PatrolPointLocations.Add(AttachedActors[i]->GetActorLocation());

	if (PatrolPoints.Num() == 0)
		CurrentState = AIState::Idle;
}

bool AFCAIController::IsPatrolling() const
{
	return CurrentState == AIState::OnPatrol;
}

bool AFCAIController::IsEngagingEnemy() const 
{
	return CurrentState == AIState::Engaged;
}

//This is to induce the character to switch weapons
void AFCAIController::SwitchWeapon()
{
	ANPC* Character = Cast<ANPC>(GetPawn());

	//Grab service rifle
	if (Character->Inventory[1] && Character->GetCurrentWeapon() != Character->Inventory[1])
		Character->ChangeWeapon();
}
