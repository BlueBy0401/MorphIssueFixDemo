// Copyright Epic Games, Inc. All Rights Reserved.

#include "FormatCHelpersBPLibrary.h"
#include "FormatCHelpers.h"

UFormatCHelpersBPLibrary::UFormatCHelpersBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void UFormatCHelpersBPLibrary::ResetRefPoseOnSkeletalMesh(USkeletalMeshComponent* Mesh,const FBoneRefPoseTransforms& Transforms)
{
	if (!Mesh)return;

	Mesh->SetRefPoseOverride(Transforms.BoneTransforms);
	/*
	
	UPhysicsAsset* PhysAsset = Mesh->GetPhysicsAsset();

	TArray<FBodyInstance*> HighLevelBodyInstances;
	TArray<FConstraintInstance*> HighLevelConstraintInstances;
	auto* world=Mesh->GetWorld();
	FPhysScene* PhysScene_PhysX = world->GetPhysicsScene();
	PrintToScreen("works",15);
	PrintToScreen("works",15);
	Mesh->InstantiatePhysicsAsset(*PhysAsset,FVector(1.0),HighLevelBodyInstances, HighLevelConstraintInstances, PhysScene_PhysX, Mesh, INDEX_NONE, FPhysicsAggregateHandle());*/
	
}

void UFormatCHelpersBPLibrary::ResetRefPoseOnSkeletalMeshFromOtherMes(USkeletalMeshComponent* Mesh,
	USkeletalMeshComponent* OtherMesh)
{

	if (!Mesh||!OtherMesh)return;

	//Mesh->SetRefPoseOverride(OtherMesh->SkeletalMesh->Skeleton->GetReferenceSkeleton().vir;//SkeletalMesh->Skeleton->GetReferenceSkeleton().GetRefBonePose());//
	Mesh->SetRefPoseOverride(OtherMesh->GetBoneSpaceTransforms());
}

void UFormatCHelpersBPLibrary::GetRefPoseOnSkeletalMesh(USkeletalMeshComponent* Mesh, TArray<FTransform>& Transforms)
{
	if(!IsValid(Mesh))
	{

		return;
		
	}	
	//Transforms=Mesh->SkeletalMesh->Skeleton->GetReferenceSkeleton().GetRefBonePose();//
	Transforms = Mesh->GetBoneSpaceTransforms();
}

void UFormatCHelpersBPLibrary::GetBoneTransformsMap(USkeletalMeshComponent* Mesh, FName BoneName,TMap<FName, FTransform>& Transforms,ERelativeTransformSpace TransformSpace)
{

	TArray<FName> Names;
	Mesh->GetBoneNames(Names);

	TArray<FTransform> BoneSpaceTransforms=Mesh->GetBoneSpaceTransforms();
	
	if(BoneSpaceTransforms.Num()!=Names.Num())
	{
		UE_LOG(LogTemp,Warning,TEXT("Names and transforms array don't equal"));
		return;
	}
	
	int index=0;
	for(FName name : Names)
	{	
		Transforms.Add(name,Mesh->GetSocketTransform(name,RTS_Component));//GetBoneTransform(Mesh->GetBoneIndex(name)));//BoneSpaceTransforms[index]);
		index++;		
	}
	
}
