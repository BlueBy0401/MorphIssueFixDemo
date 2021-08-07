// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "RagdollMorpherBPLibrary.h"
#include "PhysicsAssetGenerationSettings.h"
#include "PhysicsAssetUtils.h"
#include "RagdollMorpher.h"

URagdollMorpherBPLibrary::URagdollMorpherBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void URagdollMorpherBPLibrary::TransformCapsuleWithInterface(FKSphylElem* const shape,TScriptInterface<IWIScalableCapsule> capsuleInterface,const FTransform By)
{
	FTransform by = By;
	auto newloc = By.GetRotation().RotateVector(shape->Center) + By.GetLocation();
	auto newrot = By.GetRotation() * shape->Rotation.Quaternion();

	by.SetLocation(newloc);
	by.SetRotation(newrot);

	UObject* Object = capsuleInterface.GetObject();

	capsuleInterface->Execute_SetScaleLength(Object, shape->Length / 100);
	capsuleInterface->Execute_SetScaleRadius(Object, shape->Radius / 100);
	capsuleInterface->Execute_SetWorldPosition(Object, by.GetLocation());
	capsuleInterface->Execute_SetWorldRotation(Object, by.GetRotation());
}

void URagdollMorpherBPLibrary::SetBodiesOnSkeletalMeshPhatFromArray(USkeletalMeshComponent* From, const FTransform By,const TArray<FName> BoneNames)
{
	if ((!IsValid(From)) || !IsValid(From->PhysicsAssetOverride))return;

	UPhysicsAsset* phat = From->PhysicsAssetOverride;

	UWorld* world = GetWorld(From);

	TArray<USkeletalBodySetup*> setups = phat->SkeletalBodySetups;
	TArray<UPhysicsConstraintTemplate*> constraints = phat->ConstraintSetup;

	TArray<int32> usedConstraintIndices;

	int debugActorIndex = 0;
	for (int i = 0; i < setups.Num(); i++)
	{
		USkeletalBodySetup* setupinstance = setups[i];
		const FName bodyname = setupinstance->BoneName;

		if (!BoneNames.Contains(bodyname))continue;

		TArray<int32> ConstraintIndices;

		phat->BodyFindConstraints(i, ConstraintIndices);

		for (auto Item : ConstraintIndices)
		{
			if (usedConstraintIndices.Contains(Item))continue;

			MoveConstraintBy(constraints[Item], By);
			usedConstraintIndices.AddUnique(Item);
		}

		for (int j = 0; j < setupinstance->AggGeom.SphylElems.Num(); j++)
		{
			debugActorIndex++;

			auto shape = &setupinstance->AggGeom.SphylElems[j];

			TransformCapsuleBy(shape, By);
		}
	}

	FPhysScene* PhysScene_PhysX = world->GetPhysicsScene();

	//From-> PhysicsAssetOverride->RefreshPhysicsAssetChange();  //slower than TermArticulated + InitArticulated, but works
	From->TermArticulated();
	From->InitArticulated(PhysScene_PhysX); //magic
}

//does not work
void URagdollMorpherBPLibrary::SetBodiesOnSkeletalMeshPhatFromArrayTo(USkeletalMeshComponent* From,const TMap<FName, FTransform> Transforms)
{
	if ((!IsValid(From)) || !IsValid(From->PhysicsAssetOverride))return;

	UPhysicsAsset* phat = From->PhysicsAssetOverride;

	UWorld* world = GetWorld(From);

	TArray<USkeletalBodySetup*> setups = phat->SkeletalBodySetups;
	TArray<UPhysicsConstraintTemplate*> constraints = phat->ConstraintSetup;

	TArray<int32> usedConstraintIndices;

	int debugActorIndex = 0;
	for (int i = 0; i < setups.Num(); i++)
	{
		USkeletalBodySetup* setupinstance = setups[i];
		const FName bodyname = setupinstance->BoneName;

		if (!Transforms.Contains(bodyname))continue;

		TArray<int32> ConstraintIndices;

		phat->BodyFindConstraints(i, ConstraintIndices);
		auto tr = *Transforms.Find(bodyname);
		for (auto Item : ConstraintIndices)
		{
			if (usedConstraintIndices.Contains(Item))continue;


			MoveConstraintTo(constraints[Item], tr);
			usedConstraintIndices.AddUnique(Item);
		}

		for (int j = 0; j < setupinstance->AggGeom.SphylElems.Num(); j++)
		{
			debugActorIndex++;
			auto shape = &setupinstance->AggGeom.SphylElems[j];
			TransformCapsuleTo(shape, tr);
		}
	}

	FPhysScene* PhysScene_PhysX = world->GetPhysicsScene();

	//From-> PhysicsAssetOverride->RefreshPhysicsAssetChange();  //slower than TermArticulated + InitArticulated, but works
	From->TermArticulated();
	From->InitArticulated(PhysScene_PhysX); //magic
}

void URagdollMorpherBPLibrary::GetPHATBodyTransforms(USkeletalMeshComponent* From, UPhysicsAsset* FromPhat, TArray<FPhatBody>& BodyTransforms,TArray<UPhysicsConstraintTemplate*>&Constraints)
{
	UPhysicsAsset* phat;
	if ((!IsValid(From))) {
	
		if(!IsValid(FromPhat))return;
		phat = FromPhat;
	
	}
	else if(!IsValid(From->PhysicsAssetOverride)){

		if (!IsValid(From->GetPhysicsAsset())) {
		
			return;
		}
		phat = From->GetPhysicsAsset();

	}
	else
	{

		if (!IsValid(From->PhysicsAssetOverride)) {

			return;
		}
		phat = From->PhysicsAssetOverride;
	}

	TArray<FName> simbonenames;
	
	GetSimulatedBonesFromPHAT(phat, simbonenames);



	
	
	
	
	

	TArray<USkeletalBodySetup*> setups = phat->SkeletalBodySetups;

	for (auto name : simbonenames)
	{
		USkeletalBodySetup* setupforthisbone = nullptr;
		for (auto* setup : setups)
		{
			if (setup->BoneName == name)
				setupforthisbone = setup;
		}
		if (setupforthisbone == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("No setup found!"))
			continue;
		}

		TArray<FPhatCapsule> capsulestoadd;

		for (auto elem : setupforthisbone->AggGeom.SphylElems)
		{
			FPhatCapsule capsuletoadd = FPhatCapsule(elem.Center, elem.Rotation, elem.Radius, elem.Length);

			capsulestoadd.Add(capsuletoadd);
		}
		auto bodytoadd = FPhatBody(name, capsulestoadd);

		BodyTransforms.Add(bodytoadd);
		/*
		UWorld* world = GetWorld(From);
		FPhysScene* PhysScene_PhysX = world->GetPhysicsScene();

		//From-> PhysicsAssetOverride->RefreshPhysicsAssetChange();  //slower than TermArticulated + InitArticulated, but works
		From->TermArticulated();
		From->InitArticulated(PhysScene_PhysX);
		*/
	}
}

//works, but not doing what is should
void URagdollMorpherBPLibrary::SetPHATBodyTransforms(USkeletalMeshComponent* From, UPhysicsAsset* FromPhat, TArray<FPhatBody> BodyTransforms, TArray<UPhysicsConstraintTemplate*>Constraints)
{
	UPhysicsAsset* phat;
	if ((!IsValid(From))) {

		if (!IsValid(FromPhat))return;
		phat = FromPhat;

	}
	else if (!IsValid(From->PhysicsAssetOverride)) {

		if (!IsValid(From->GetPhysicsAsset())) {

			return;
		}
		phat = From->GetPhysicsAsset();

	}
	else
	{

		if (!IsValid(From->PhysicsAssetOverride)) {

			return;
		}
		phat = From->PhysicsAssetOverride;
	}



	TArray<USkeletalBodySetup*> setups = phat->SkeletalBodySetups;
	for (auto transform : BodyTransforms)
	{
		USkeletalBodySetup* setupforthisbone = nullptr;
		for (auto* setup : setups)
		{
			if (setup->BoneName == transform.BoneName)
				setupforthisbone = setup;
		}
		if (setupforthisbone == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("No setup found!"))
				continue;
		}

		if (setupforthisbone->AggGeom.SphylElems.Num() != transform.Capsules.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("Struct and setup array length don't match!"))
				continue;
		}

		for (int i = 0; i < transform.Capsules.Num(); i++)
		{
			//go thru sphylelems and set them from struct array in any order

			FKSphylElem* elem = &setupforthisbone->AggGeom.SphylElems[i];

			auto capsulefromstruct = transform.Capsules[i];

			elem->Center = capsulefromstruct.Center;
			elem->Rotation = capsulefromstruct.Rotation;
			elem->Length = capsulefromstruct.Length;
			elem->Radius = capsulefromstruct.Radius;
		}
	}
	for(auto * Constraint : phat->ConstraintSetup)
	{
		FConstraintInstance* DefInstance = &Constraint->DefaultInstance;
		FTransform RefFrame = DefInstance->GetRefFrame(EConstraintFrame::Frame1);
		RefFrame.AddToTranslation(FVector(.01, 0, 0));
		DefInstance->SetRefPosition(EConstraintFrame::Frame1, RefFrame.GetLocation());
	}
	if (IsValid(From)) { From->MarkPackageDirty(); }
	if (IsValid(FromPhat)) { FromPhat->MarkPackageDirty(); }
}

void URagdollMorpherBPLibrary::UpdateDebugActors(const USkeletalMeshComponent* From, const TArray<FName> BodiesToDraw,TArray<TScriptInterface<IWIScalableCapsule>> DebugCapsules,const bool Enabled, const bool DrawAllBodies, FColor DebugColor)
{
	if (!Enabled || (!IsValid(From)) || !IsValid(From->PhysicsAssetOverride))
		return;

	TArray<USkeletalBodySetup*> setups = From->PhysicsAssetOverride->SkeletalBodySetups;

	int debugActorIndex = 0;

	for (int i = 0; i < setups.Num(); i++)
	{
		USkeletalBodySetup* setupinstance = setups[i];
		const FName bodyname = setupinstance->BoneName;

		if (!DrawAllBodies && !BodiesToDraw.Contains(bodyname))continue;

		const FTransform bonetransform = From->GetBoneTransform(From->GetBoneIndex(bodyname));

		for (int j = 0; j < setupinstance->AggGeom.SphylElems.Num(); j++)
		{
			debugActorIndex++;

			const auto shape = &setupinstance->AggGeom.SphylElems[j];

			if (DebugCapsules.IsValidIndex(debugActorIndex) && IsValid(DebugCapsules[debugActorIndex].GetObject()))
			{
				TransformCapsuleWithInterface(shape, DebugCapsules[debugActorIndex], bonetransform);
			}
		}
	}
}

//DON'T THINK IT WORKS
void URagdollMorpherBPLibrary::ConnectConstraintToSkeletalMeshActorPhysicsAsset(UPhysicsConstraintComponent* Constraint, USkeletalMeshComponent* Mesh, FName BoneName)
{
	if (!IsValid(Mesh) || !IsValid(Constraint))return;

	UPhysicsAsset* Phat = Mesh->PhysicsAssetOverride;

	if (!IsValid(Phat))return;

	UPhysicsConstraintTemplate* constraintTemplate = NewObject<UPhysicsConstraintTemplate>();

	FConstraintInstance* ConstraintInstance = &Constraint->ConstraintInstance;

	ConstraintInstance->ConstraintBone1 = BoneName;
	ConstraintInstance->ConstraintBone2 = FName(Constraint->GetName());

	constraintTemplate->DefaultInstance = *ConstraintInstance;

	if (!IsValid(constraintTemplate))return;

	Phat->ConstraintSetup.Add(constraintTemplate);

	UWorld* world = GetWorld(Constraint);
	FPhysScene* PhysScene_PhysX = world->GetPhysicsScene();

	Mesh->TermArticulated();
	Mesh->InitArticulated(PhysScene_PhysX);
}

URagdollSetupData* URagdollMorpherBPLibrary::GetPhysicsBodySetupDataFromRagdoll(const USkeletalMeshComponent* Mesh)
{
	if (!IsValid(Mesh))return nullptr;
	auto Phat = Mesh->PhysicsAssetOverride;

	URagdollSetupData* setupData = NewObject<URagdollSetupData>();
	if (!IsValid(Phat))return setupData;

	auto bodysetups = Phat->SkeletalBodySetups;
	auto constraintsetups = Phat->ConstraintSetup;

	for (auto setup : bodysetups)
	{
		auto capTransforms = TArray<FTransform>();

		for (auto capsule : setup->AggGeom.SphylElems)
			capTransforms.Add(capsule.GetTransform());

		URagdollBodySetupData* ragdollBodySetupData = NewObject<URagdollBodySetupData>();
		ragdollBodySetupData->capsuleTransforms = capTransforms;

		setupData->bodyData.Add(setup->BoneName.ToString(), ragdollBodySetupData);
	}

	for (auto setup : constraintsetups)
	{
		setupData->constraintFrame1Data.Add(setup->DefaultInstance.GetRefFrame(EConstraintFrame::Frame1));
		setupData->constraintFrame2Data.Add(setup->DefaultInstance.GetRefFrame(EConstraintFrame::Frame2));
	}

	return setupData;
}

bool URagdollMorpherBPLibrary::SetPhysicsBodySetupDataFromRagdoll(USkeletalMeshComponent* Mesh,const URagdollSetupData* Data)
{
	if (!IsValid(Mesh))return false;

	auto Phat = Mesh->PhysicsAssetOverride;

	if (!IsValid(Phat) || !IsValid(Data))return false;

	auto bodysetups = Phat->SkeletalBodySetups;
	auto constraintsetups = Phat->ConstraintSetup;

	const TMap<FString, URagdollBodySetupData*> bodyData = Data->bodyData;
	const auto constraintFrame1Data = Data->constraintFrame1Data;
	const auto constraintFrame2Data = Data->constraintFrame2Data;

	for (auto setup : bodysetups)
	{
		const auto setupname = setup->BoneName.ToString();

		//setupname.RemoveSpacesInline();

		bool setupsnotempty = bodyData.Num() != 0;
		bool bonenotinlist = !bodyData.Contains(setupname);

		if (!setupsnotempty || bonenotinlist)continue;

		URagdollBodySetupData* setuptransformsfromdata = *bodyData.Find(setupname);

		if (setuptransformsfromdata == nullptr)continue;

		auto* elems = &setup->AggGeom.SphylElems;

		TArray<FTransform>* DataTransforms = &setuptransformsfromdata->capsuleTransforms;

		if (elems->Num() != DataTransforms->Num())continue;

		for (int i = 0; i < elems->Num(); ++i)
		{
			if (elems->IsValidIndex(i) && DataTransforms->IsValidIndex(i))
				elems->GetData()[i].SetTransform(DataTransforms->GetData()[i]);
		}
	}

	if (constraintsetups.Num() == constraintFrame1Data.Num() == constraintFrame2Data.Num())

		for (int i = 0; i < constraintsetups.Num(); i++)
		{
			if (!constraintFrame1Data.IsValidIndex(i) || !constraintFrame2Data.IsValidIndex(i) || !constraintsetups.
				IsValidIndex(i))continue;

			constraintsetups.GetData()[i]->DefaultInstance.SetRefPosition(
				EConstraintFrame::Frame1, constraintFrame1Data.GetData()[i].GetLocation());


			constraintsetups.GetData()[i]->DefaultInstance.SetRefPosition(
				EConstraintFrame::Frame2, constraintFrame2Data.GetData()[i].GetLocation());
		}

	UWorld* world = GetWorld(Mesh);
	FPhysScene* PhysScene_PhysX = world->GetPhysicsScene();

	Mesh->TermArticulated();
	Mesh->InitArticulated(PhysScene_PhysX);

	return true;
}

void URagdollMorpherBPLibrary::CopyPhysicsBodiesBetweenPhysicsAssets(const UPhysicsAsset* From, UPhysicsAsset* To)
{
	//copy bodies from
	//instantiate a new one
	//copy params
	//add to array
	/*
		To->SkeletalBodySetups=From->SkeletalBodySetups;
		To->ConstraintSetup=From->ConstraintSetup;
	*/
	if (!IsValid(From) || !IsValid(To))return;

	TArray<USkeletalBodySetup*> NewSkeletalBodySetups;

	TArray<USkeletalBodySetup*> FromSetUps = From->SkeletalBodySetups;

	for (const auto Setup : FromSetUps)
	{
		
		const FPhysAssetCreateParams& NewBodyData = GetDefault<UPhysicsAssetGenerationSettings>()->CreateParams;
		const auto NewIndex=FPhysicsAssetUtils::CreateNewBody(To, Setup->BoneName, NewBodyData);

		USkeletalBodySetup* NewSetupToAdd = To->SkeletalBodySetups.GetData()[NewIndex];

		NewSetupToAdd->CopyBodyPropertiesFrom(Setup);

		NewSkeletalBodySetups.Add(NewSetupToAdd);
		To->UpdateBodySetupIndexMap();
		To->UpdateBoundsBodiesArray();
		Setup->Modify();
		Setup->MarkPackageDirty();
	}
	{
	}

	To->SkeletalBodySetups = NewSkeletalBodySetups;
	
	TArray<UPhysicsConstraintTemplate*> NewConstraintSetups;

	TArray<UPhysicsConstraintTemplate*> FromConstraintSetups = From->ConstraintSetup;

	for (const auto Setup : FromConstraintSetups)
	{

		const auto NewIndex = FPhysicsAssetUtils::CreateNewConstraint(To, Setup->GetFName(),Setup);

		auto NewSetupToAdd = To->ConstraintSetup.GetData()[NewIndex];
		//NewSetupToAdd->DefaultInstance.CopyConstraintParamsFrom(&setup->DefaultInstance);
		NewConstraintSetups.Add(NewSetupToAdd);

	
		

		To->UpdateBodySetupIndexMap();
		To->UpdateBoundsBodiesArray();
		Setup->Modify();
		Setup->MarkPackageDirty();
		
	}

	To->ConstraintSetup = NewConstraintSetups;
	To->InvalidateAllPhysicsMeshes();
	
#if WITH_EDITOR
	To->MarkPackageDirty();
#endif


}
