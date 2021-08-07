// All rights reserved

#pragma once

#include "CoreMinimal.h"

#include "Engine/World.h"
#include "SkeletalRenderPublic.h"
#include "Animation/MorphTarget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "WIScalableCapsule.h"
#include "../../../../FormatCHelpers/Source/FormatCHelpers/Public/FormatCHelpersBPLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "RagdollMorpherBPLibrary.generated.h"


USTRUCT(BlueprintType)
struct FPhatCapsule
{
	GENERATED_USTRUCT_BODY()
public:
	FPhatCapsule(){}
	FPhatCapsule(FVector c,FRotator r,float ra,float le):Center(c),Rotation(r),Radius(ra),Length(le){}
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector Center;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRotator Rotation;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Radius;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Length;
};


USTRUCT(BlueprintType)
struct FPhatBody
{
	GENERATED_USTRUCT_BODY()
public:
	FPhatBody(){}
	FPhatBody(FName n, TArray<FPhatCapsule> c):BoneName(n),Capsules(c){}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FPhatCapsule> Capsules;
};



UCLASS(BlueprintType)
class URagdollBodySetupData : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	FString ParseToString()
	{
		FString dataAsString;
		for (auto transform : capsuleTransforms)
		{
			dataAsString.Append(transform.ToString()+"|BSD|");
		}
		return  dataAsString;
	}

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FTransform> capsuleTransforms;
};

//----------------------------------------------------------------------------------------------------

UCLASS(BlueprintType)
class URagdollSetupData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, URagdollBodySetupData*> bodyData;

	UPROPERTY(BlueprintReadWrite)
	TArray<FTransform> constraintFrame1Data;

	
	UPROPERTY(BlueprintReadWrite)
	TArray<FTransform> constraintFrame2Data;
	
	UFUNCTION(BlueprintCallable)
	TArray<FString> ParseFrame1ToString()
	{
		TArray<FString> dataAsString;
		for (const auto transform : constraintFrame1Data)
		{
			dataAsString.Add(transform.ToString());
		}
		return  dataAsString;
	}
	
	UFUNCTION(BlueprintCallable)
	TArray<FString> ParseFrame2ToString()
	{
		TArray<FString> dataAsString;
		for (const auto transform : constraintFrame2Data)
		{
			dataAsString.Add(transform.ToString());
		}
		return  dataAsString;
	}

	UFUNCTION(BlueprintCallable)
	TArray<FString> ParseBodyDataToString()
	{
		TArray<FString> dataAsString;
		for (const auto bodysetupdata : bodyData)
		{
			dataAsString.Add(bodysetupdata.Key+" : "+bodysetupdata.Value->ParseToString());
		}
		return  dataAsString;
	}
};


//----------------------------------------------------------------------------------------------------

UCLASS()
class URagdollMorpherBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="RagdollMorpher")
	static void SetBodiesOnSkeletalMeshPhat(USkeletalMeshComponent* From, const FTransform By,const FName BoneName)
	{
		if ((!IsValid(From)) || !IsValid(From->PhysicsAssetOverride))return;

		UPhysicsAsset* phat = From->PhysicsAssetOverride;

		TArray<USkeletalBodySetup*> setups = phat->SkeletalBodySetups;
		TArray<UPhysicsConstraintTemplate*> constraints = phat->ConstraintSetup;

		for (int i = 0; i < setups.Num(); i++)
		{
			USkeletalBodySetup* setupinstance = setups[i];
			const FString bodyname = setupinstance->BoneName.ToString();

			if (bodyname.Compare(BoneName.ToString()))continue;

			TArray<int32> ConstraintIndices;

			phat->BodyFindConstraints(i, ConstraintIndices);

			if (i < constraints.Num())
			{
				MoveConstraintBy(constraints[i], By);
			}

			for (int j = 0; j < setupinstance->AggGeom.SphylElems.Num(); j++)
			{
				TransformCapsuleBy(&setupinstance->AggGeom.SphylElems[j], By);
			}
		}


		UWorld* world = GEngine->GetWorldFromContextObject(From, EGetWorldErrorMode::LogAndReturnNull);
		FPhysScene* PhysScene_PhysX = world->GetPhysicsScene();

		//From-> PhysicsAssetOverride->RefreshPhysicsAssetChange();  //slower than TermArticulated + InitArticulated, but works
		From->TermArticulated();
		From->InitArticulated(PhysScene_PhysX);
	}
	
	UFUNCTION(BlueprintCallable, Category="RagdollMorpher")
	static void SetBodiesOnSkeletalMeshPhatFromArray(USkeletalMeshComponent* From, const FTransform By,const TArray<FName> BoneNames);
	
	UFUNCTION(BlueprintCallable, Category="RagdollMorpher")
	static void SetBodiesOnSkeletalMeshPhatFromArrayTo(USkeletalMeshComponent* From, const TMap<FName,FTransform> Transforms);
	
	UFUNCTION(BlueprintCallable, Category="RagdollMorpher")	
	static void ReInitPHAT(USkeletalMeshComponent* From){

		UWorld* world = GetWorld(From);
		FPhysScene* PhysScene_PhysX = world->GetPhysicsScene();

		//From-> PhysicsAssetOverride->RefreshPhysicsAssetChange();  //slower than TermArticulated + InitArticulated, but works
		From->TermArticulated();
		From->InitArticulated(PhysScene_PhysX);//magic
	}

	UFUNCTION(BlueprintCallable, Category="RagdollMorpher")	
	static void GetPHATBodyTransforms(USkeletalMeshComponent* From, UPhysicsAsset* FromPhat, TArray<FPhatBody> & BodyTransforms, TArray<UPhysicsConstraintTemplate*>&Constraints);

	UFUNCTION(BlueprintCallable, Category="RagdollMorpher")	
	static void SetPHATBodyTransforms(USkeletalMeshComponent* From, UPhysicsAsset* FromPhat, TArray<FPhatBody>  BodyTransforms, TArray<UPhysicsConstraintTemplate*>Constraints);

	UFUNCTION(BlueprintCallable, Category="RagdollMorpher")
	static void UpdateDebugActors(const USkeletalMeshComponent* From,const TArray<FName> BodiesToDraw,TArray<TScriptInterface<IWIScalableCapsule>> DebugCapsules,const bool Enabled,const bool DrawAllBodies,FColor DebugColor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RagdollMorpher")
	static void GetSimulatedBonesFromSkeletalMesh(USkeletalMeshComponent* From,TArray<FName>& simulatedBodyNames)
	{
		if ((!IsValid(From)) || !IsValid(From->PhysicsAssetOverride))return;

		TArray<USkeletalBodySetup*> setups = From->PhysicsAssetOverride->SkeletalBodySetups;

		for (int i = 0; i < setups.Num(); i++)
		{
			simulatedBodyNames.Add(setups[i]->BoneName);
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RagdollMorpher")
		static void GetSimulatedBonesFromPHAT(UPhysicsAsset* From, TArray<FName>& simulatedBodyNames)
	{
		if (!IsValid(From))return;

		TArray<USkeletalBodySetup*> setups = From->SkeletalBodySetups;

		for (int i = 0; i < setups.Num(); i++)
		{
			simulatedBodyNames.Add(setups[i]->BoneName);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "RagdollMorpher")
	static void ConnectConstraintToSkeletalMeshActorPhysicsAsset(UPhysicsConstraintComponent* Constraint,USkeletalMeshComponent* Mesh,FName BoneName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RagdollMorpher")
	static URagdollSetupData* GetPhysicsBodySetupDataFromRagdoll(const USkeletalMeshComponent* Mesh);

	UFUNCTION(BlueprintCallable, Category = "RagdollMorpher")
	static bool SetPhysicsBodySetupDataFromRagdoll(USkeletalMeshComponent* Mesh,const URagdollSetupData* Data);

	UFUNCTION(BlueprintCallable, Category = "RagdollMorpher")
	static void CopyPhysicsBodiesBetweenPhysicsAssets(const UPhysicsAsset * From,UPhysicsAsset * To);

	


private
:
	static void MoveConstraintBy(UPhysicsConstraintTemplate* constraint,FTransform by)
	{
		auto pos1 = constraint->DefaultInstance.Pos1;
		auto pos2 = constraint->DefaultInstance.Pos2;
		constraint->DefaultInstance.SetRefPosition(EConstraintFrame::Frame1, by.GetLocation() + pos1);
		constraint->DefaultInstance.SetRefPosition(EConstraintFrame::Frame2, by.GetLocation() + pos2);
	}
	
	static void MoveConstraintTo(UPhysicsConstraintTemplate* constraint,FTransform by)
	{
		auto pos1 = constraint->DefaultInstance.Pos1;
		auto pos2 = constraint->DefaultInstance.Pos2;
		constraint->DefaultInstance.SetRefPosition(EConstraintFrame::Frame1, by.GetLocation());
		constraint->DefaultInstance.SetRefPosition(EConstraintFrame::Frame2, by.GetLocation());
	}

	static void TransformCapsuleBy(FKSphylElem* elem, FTransform by)
	{
		const auto rot = by.GetRotation().Euler();
		elem->Length += by.GetScale3D().X;
		elem->Radius += by.GetScale3D().Y;
		elem->Center += by.GetLocation();
		elem->Rotation.Add(rot.FVector::X, rot.FVector::Y, rot.FVector::Z);
	}

	static void TransformCapsuleTo(FKSphylElem* elem,FTransform by)
{
	const auto rot = by.GetRotation().Euler();
	//elem->Length = by.GetScale3D().X*10;
	//elem->Radius = by.GetScale3D().Y*10;
	elem->Center = by.GetLocation();
	elem->Rotation.Add(rot.FVector::X, rot.FVector::Y, rot.FVector::Z);
}

	static UWorld* GetWorld(const UObject* Context)
	{
		return GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::LogAndReturnNull);
	}

	static void TransformCapsuleWithInterface(FKSphylElem* shape, TScriptInterface<IWIScalableCapsule> capsuleInterface, const FTransform By);

	static void Print(FString toPrint,float time = 5,FColor color = FColor::White)
	{
		UFormatCHelpersBPLibrary::PrintToScreen(toPrint, time, color);
	}
};
