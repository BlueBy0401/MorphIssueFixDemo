// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Animation/MorphTarget.h"
#include "FormatCHelpersBPLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE(FWDelegate);

USTRUCT(BlueprintType)
struct FBoneRefPoseTransforms
{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
		TArray<FTransform> BoneTransforms;

};

UENUM(BlueprintType)
enum class EPackagingStatus:uint8 {

	Editor,
	DevBuild,
	TestBuild,
	ShippingBuild,
	Other,
};

/*
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu.
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UFormatCHelpersBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers")
	static void PrintFNameArray(
		FString prefix,
		TArray<FName> NamesToPrint,
		float time = 15,
		FColor color = FColor::Blue,
		bool enabled = true)
	{
		if (!enabled)return;
		PrintToScreen(prefix, time, color);
		if (NamesToPrint.IsValidIndex(0))
			for (auto element : NamesToPrint)
			{
				PrintToScreen(element.ToString(), time, color);
			}
	}

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers")
		static void PrintFStringArray(
			FString prefix,
			TArray<FString> StringsToPrint,
			float time = 15,
			FColor color = FColor::Blue,
			bool enabled = true)
	{
		if (!enabled)return;
		if (StringsToPrint.IsValidIndex(0))
			for (auto element : StringsToPrint)
			{
				PrintToScreen(element, time, color);
			}
	}

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers")
		static void PrintIntArray(
			FString prefix,
			TArray<int> IntsToPrint,
			float time = 15,
			FColor color = FColor::Blue,
			bool enabled = true)
	{
		if (!enabled)return;
		if (IntsToPrint.IsValidIndex(0))
			for (auto element : IntsToPrint)
			{
				if (element)
					PrintToScreen(FString::FromInt(element), time, color);
			}
	}

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers")
		static void PrintFloatArray(
			FString prefix,
			TArray<float> FloatsToPrint,
			float time = 15,
			FColor color = FColor::Blue,
			bool enabled = true)
	{
		if (!enabled)return;
		if (FloatsToPrint.IsValidIndex(0))
			for (auto element : FloatsToPrint)
			{
				if (element)
					PrintToScreen(FString::SanitizeFloat(element), time, color);
			}
	}

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers")
		static void PrintObjectArray(FString prefix,TArray<UObject*> ObjectsToPrint,float time = 15,FColor color = FColor::Blue,bool enabled = true)
	{
		if (!enabled)return;
		for (auto element : ObjectsToPrint)
		{
			if (IsValid(element))
				PrintToScreen(element->GetName(), time, color);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers")
		static void HideActors(TArray<UObject*> Actors,bool Show,bool Enabled = true)
	{
		if (!Enabled)return;
		for (auto object : Actors)
		{
			AActor* actor = (AActor*)object;
			if (IsValid(actor))
				actor->SetActorHiddenInGame(!Show);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers")
		static void TransformActors(TArray<UObject*> Actors,bool SimulatePhysics,bool Enabled)
	{
		if (!Enabled)return;
		for (auto object : Actors)
		{
			AActor* actor = (AActor*)object;
			//if (IsValid(actor))
			//actor->SetActorHiddenInGame(!Show);
		}
	}
/*
	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers")
		static void ExecuteAsync(FWDelegate ToExecute, FWDelegate OnProgress, FWDelegate OnFinished)
	{
		// Schedule a thread                               // Pass in our parameters to the lambda expression
		AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [ToExecute, OnProgress, OnFinished]()
			{
				ToExecute.ExecuteIfBound();
				AsyncTask(ENamedThreads::GameThread, [OnFinished]()
					{
						// We execute the delegate along with the param
						OnFinished.ExecuteIfBound();
					});
			});
	}
*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|Map")
		static bool MapsAreIdentical(const TMap<UStruct*, UStruct*>& a, const TMap<UStruct*, UStruct*>& b)
	{
		return true;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|String")
		static bool ToTransform(FString Source, FTransform& Return)
	{
		Return = FTransform();

		if (Source.Len() == 0)
		{
			return false;
		}

		auto srcAsArray = TArray<FString>();
		FString delim = "|";
		Source.ParseIntoArray(srcAsArray, *delim, false);

		if (srcAsArray.Num() != 3)return false;

		auto loc = FVector();
		auto rot = FRotator();
		auto sc = FVector();


		auto dataarray = TArray<FString>();
		FString delimArray = ",";
		TArray<FString>::ElementType inSourceString = srcAsArray.GetData()[0];

		inSourceString.ParseIntoArray(dataarray, *delimArray, false);
		{
			if (dataarray.Num() != 3)return false;

			for (auto num : dataarray)
			{
				if (num.IsNumeric())continue;
				return false;
			}

			loc.X = FCString::Atof(*dataarray[0]);
			loc.Y = FCString::Atof(*dataarray[1]);
			loc.Z = FCString::Atof(*dataarray[2]);
		}
		inSourceString = srcAsArray.GetData()[1];
		inSourceString.ParseIntoArray(dataarray, *delimArray, false);
		{
			if (dataarray.Num() != 3)return false;

			for (auto num : dataarray)
			{
				if (num.IsNumeric())continue;
				return false;
			}

			rot.Roll = FCString::Atof(*dataarray[2]);
			rot.Pitch = FCString::Atof(*dataarray[0]);
			rot.Yaw = FCString::Atof(*dataarray[1]);
		}
		inSourceString = srcAsArray.GetData()[2];
		inSourceString.ParseIntoArray(dataarray, *delimArray, false);
		{
			if (dataarray.Num() != 3)return false;

			for (auto num : dataarray)
			{
				if (num.IsNumeric())continue;
				return false;
			}

			sc.X = FCString::Atof(*dataarray[0]);
			sc.Y = FCString::Atof(*dataarray[1]);
			sc.Z = FCString::Atof(*dataarray[2]);
		}

		Return.SetLocation(loc);
		Return.SetRotation(rot.Quaternion());
		Return.SetScale3D(sc);

		return true;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|Materials")
		static TArray<FVectorParameterValue> GetAllVectorParametersFromMateralInstances(TArray<UMaterialInstanceDynamic*> MaterialInstances)
	{
		TArray<FVectorParameterValue> vectors;
		TArray<FName> namesAdded;

		for (UMaterialInstanceDynamic* mat : MaterialInstances) {

			TArray<FVectorParameterValue> matScalars = mat->VectorParameterValues;

			for (FVectorParameterValue val : matScalars) {

				if (!namesAdded.Contains(val.ParameterInfo.Name))
					vectors.AddUnique(val);
				namesAdded.AddUnique(val.ParameterInfo.Name);

			}

		}




		return vectors;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers/Materials")
		static TArray<FScalarParameterValue> GetAllScalarParametersFromMateralInstances(TArray<UMaterialInstanceDynamic*> MaterialInstances)
	{
		TArray<FScalarParameterValue> scalars;
		TArray<FName> namesAdded;

		for (UMaterialInstanceDynamic* mat : MaterialInstances) {

			TArray<FScalarParameterValue> matScalars = mat->ScalarParameterValues;

			for (FScalarParameterValue val : matScalars) {

				if (!namesAdded.Contains(val.ParameterInfo.Name))
					scalars.AddUnique(val);
				namesAdded.AddUnique(val.ParameterInfo.Name);

			}

		}


		return scalars;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|Materials")
		static TArray<FTextureParameterValue> GetAllTextureParametersFromMateralInstances(TArray<UMaterialInstanceDynamic*> MaterialInstances)
	{
		TArray<FTextureParameterValue> textures;
		TArray<FName> namesAdded;

		for (UMaterialInstanceDynamic* mat : MaterialInstances) {

			TArray<FTextureParameterValue> matScalars = mat->TextureParameterValues;

			for (FTextureParameterValue val : matScalars) {

				if (!namesAdded.Contains(val.ParameterInfo.Name))
					textures.AddUnique(val);
				namesAdded.AddUnique(val.ParameterInfo.Name);

			}

		}


		return textures;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers/Array")
		static TArray<int32> SelectArray(TArray<int32> A, TArray<int32> B, bool bSelectA)
	{
		return bSelectA ? A : B;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|Morphs")
		static void GetActiveMorphTargetNames(const USkeletalMeshComponent* Mesh, TArray<FName>& Morphs) {

		if (!Mesh)return;

		auto morphs = Mesh->ActiveMorphTargets;

		for (auto morph : morphs)
		{
			if (!(morph.MorphTarget->GetFName().ToString().Contains("pjcm")))
				Morphs.Add(morph.MorphTarget->GetFName());
		}

	}

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers|Skeleton")
		static void ResetRefPoseOnSkeletalMesh(USkeletalMeshComponent* Mesh, const FBoneRefPoseTransforms& Transforms);

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers|Skeleton")
		static void ResetRefPoseOnSkeletalMeshFromOtherMes(USkeletalMeshComponent* Mesh, USkeletalMeshComponent* OtherMesh);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|Skeleton")
		static void GetRefPoseOnSkeletalMesh(USkeletalMeshComponent* Mesh, TArray<FTransform>& Transforms);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|Skeleton")
		static void GetBoneTransformsMap(USkeletalMeshComponent* Mesh, FName BoneName, TMap<FName, FTransform>& Transforms,ERelativeTransformSpace TransformSpace);

	UFUNCTION(BlueprintCallable, Category = "FormatCHelpers|Packaging", Meta = (ExpandEnumAsExecs = "Branches"))
		static void GetPackagingStatus(EPackagingStatus& Branches) 
	{

#if WITH_EDITOR
		Branches= EPackagingStatus::Editor;
		return;
#endif
#if UE_BUILD_DEVELOPMENT
		Branches= EPackagingStatus::DevBuild;
		return;
#endif
#if UE_BUILD_TEST
		Branches= EPackagingStatus::TestBuild;
		return;
#endif
#if UE_BUILD_SHIPPING
		Branches= EPackagingStatus::ShippingBuild;
		return;
#else
		Branches= EPackagingStatus::Other;
#endif
		
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|Packaging")
	static bool IsInEditor()
	{
		return GIsEditor;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FormatCHelpers|Packaging")
	bool IsShippingBuild()
	{
#if (UE_BUILD_SHIPPING && !UE_BUILD_TEST)
		return true;
#else
		return false;
#endif
	}


	static void PrintToScreen(FString text, float time, FColor color = FColor::Blue)
	{
		GEngine->AddOnScreenDebugMessage(-1, time, color, text);
	}

private:
};
