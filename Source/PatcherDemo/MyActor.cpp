// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
#include "Kismet/GameplayStatics.h"
#include "VictoryBPFunctionLibrary.h"

// Sets default values
AMyActor::AMyActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	RootComponent=CreateDefaultSubobject<USceneComponent>("Root");	
	RootComponent->SetMobility(EComponentMobility::Movable);

	CamComp=CreateDefaultSubobject<UCameraComponent>("Cam");
	CamComp->SetMobility(EComponentMobility::Movable);
	CamComp->SetRelativeLocation(FVector(0,-200,200));
	CamComp->SetRelativeRotation(FRotator(0,0,90));
	
	BodyMesh=CreateDefaultSubobject<USkeletalMeshComponent>("BodyMesh");
	BodyMesh->SetMobility(EComponentMobility::Movable);
	BodyMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	BodyMesh->SetMorphTarget("Gou Luk",1);
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	
	PlayerInputComponent->BindAction("ResetPose",IE_Pressed,this,&AMyActor::DoResetPose);
	PlayerInputComponent->BindAction("ResetPose",IE_Pressed,this,&AMyActor::DoResetPose);
	PlayerInputComponent->BindAxis("Turn",this,&AMyActor::RotateCamera);	
}

void AMyActor::DoResetPose()
{

	GEngine->AddOnScreenDebugMessage(-1,15,FColor::Blue,"Resetting!");

	if(!IsValid(BodyMeshOther))
	{
		UE_LOG(LogTemp,Warning,TEXT("Please assign something to your BodyMeshOther property!"));
		return;
	}
	BodyMesh->SetRefPoseOverride(BodyMeshOther->GetBoneSpaceTransforms());
}

void AMyActor::RotateCamera(float speedAroundUpAxis)
{
	
	GEngine->AddOnScreenDebugMessage(-1,15,FColor::Blue,FString::SanitizeFloat(speedAroundUpAxis));
	BodyMesh->AddLocalRotation(FRotator(0,speedAroundUpAxis*CamRotSpeed,0));
	
}

void AMyActor::SavePoseTransforms( USkeletalMeshComponent* a,  USkeletalMeshComponent* b)
{
	const FString RootDir=UVictoryBPFunctionLibrary::VictoryPaths__GameRootDirectory();

	FString sa,sb;

	TArray<FTransform> trA=a-> GetBoneSpaceTransforms();
	TArray<FTransform> trB=b-> GetBoneSpaceTransforms();

	for(FTransform tr:trA)
	{

		sa=sa.Append(tr.GetLocation().ToString()+" , \n" );
		
	}
	
	for(FTransform tr:trB)
	{

		sb=sb.Append(tr.GetLocation().ToString()+" , \n" );
		
	}
	
	UVictoryBPFunctionLibrary::FileIO__SaveStringTextToFile(RootDir,"a.txt",sa,true);
	UVictoryBPFunctionLibrary::FileIO__SaveStringTextToFile(RootDir,"b.txt",sb,true);

	
}

