// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "Object.h"
#include "CSCustomAnimNodes.h"
#include "CSUtil.generated.h"

/**
 * 
 */
UCLASS()
class COOLSUMMER_API UCSUtil : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure, Meta = (Keywords = "Safe / Float")) static float SafeDivideFloat(const float A, const float B, const float Failed = 0);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure, Meta = (Keywords = "Safe / Vector")) static FVector SafeDivideVector(const FVector& A, const float B, const FVector Failed = FVector::ZeroVector);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure, Meta = (Keywords = "Lerp Interp Float")) static float FLerp(const float Current, const float Target, const float DeltaTime, const float Speed);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure, Meta = (Keywords = "Check Lerp Interp Float", Tooltip = "If True, Need Apply OutResult. Otherwise can ignore.")) static bool CheckFLerp(const float Current, const float Target, const float DeltaTime, const float Speed, float& OutResult);
	
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) static float ToMeterFloat(float Unreal);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) static float ToUnrealFloat(float Meter);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) static FVector ToMeterVector(FVector Unreal);
	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) static FVector ToUnrealVector(FVector Meter);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure, Meta = (ToolTip = "If true, Location is Front.")) static bool LocationPitchYaw(const FRotator& FrontRotation, const FVector& RelativeLocation, float& OutPitchDegree, float& OutYawDegree);

	UFUNCTION(Category = "CS", BlueprintCallable, BlueprintPure) static FString CombineLF(const FString& A, const FString& B);

	static const float MeterInUnreal;
};
