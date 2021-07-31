// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWDelegate);

UCLASS()
class PATCHERDEMO_API AMyActor : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void DoResetPose();

	void RotateCamera(float speedAroundUpAxis);

	UFUNCTION(BlueprintCallable)
	void SavePoseTransforms( USkeletalMeshComponent* a,  USkeletalMeshComponent* b);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent * BodyMesh;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent * BodyMeshOther;
	
	UPROPERTY(EditAnywhere)
	UCameraComponent * CamComp;

	UPROPERTY(EditAnywhere)
	float CamRotSpeed=1;

};
