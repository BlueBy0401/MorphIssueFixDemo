// All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WIScalableCapsule.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UWIScalableCapsule : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RAGDOLLMORPHER_API IWIScalableCapsule
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RagdollMorpher/Debug")
	void SetScaleLength(float length);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RagdollMorpher/Debug")
	void SetScaleRadius(float radius);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RagdollMorpher/Debug")
	void SetWorldPosition(FVector worldPosition);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RagdollMorpher/Debug")
	void SetWorldRotation(FQuat worldRotation);

};
