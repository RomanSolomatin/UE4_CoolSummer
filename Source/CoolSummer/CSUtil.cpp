// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSUtil.h"

const float UCSUtil::MeterInUnreal = 100;

float UCSUtil::SafeDivideFloat(const float A, const float B, const float Failed)
{
	return 0 == B ? Failed : A / B;
}
FVector UCSUtil::SafeDivideVector(const FVector& A, const float B, const FVector Failed)
{
	return 0 == B ? Failed : A / B;
}
float UCSUtil::FLerp(const float Current, const float Target, const float DeltaTime, const float Speed)
{
	register float Delta = Target - Current;
	if (!FMath::IsNearlyZero(Delta))
	{
		if (Delta < 0)
		{
			register float NewValue = Current - DeltaTime * FMath::Abs(Speed);
			return NewValue < Target ? Target : NewValue;
		}
		else
		{
			register float NewValue = Current + DeltaTime * FMath::Abs(Speed);
			return NewValue > Target ? Target : NewValue;
		}
	}
	return Current;
}
bool UCSUtil::CheckFLerp(const float Current, const float Target, const float DeltaTime, const float Speed, float& OutResult)
{
	register float Delta = Target - Current;
	if (!FMath::IsNearlyZero(Delta))
	{
		if (Delta < 0)
		{
			register float NewValue = Current - DeltaTime * FMath::Abs(Speed);
			OutResult = NewValue < Target ? Target : NewValue;
		}
		else
		{
			register float NewValue = Current + DeltaTime * FMath::Abs(Speed);
			OutResult = NewValue > Target ? Target : NewValue;
		}
		return true;
	}
	OutResult = Target;
	return false;
}

float UCSUtil::ToMeterFloat(float Unreal)
{
	return Unreal / MeterInUnreal;
}
float UCSUtil::ToUnrealFloat(float Meter)
{
	return Meter * MeterInUnreal;
}
FVector UCSUtil::ToMeterVector(FVector Unreal)
{
	return Unreal / MeterInUnreal;
}
FVector UCSUtil::ToUnrealVector(FVector Meter)
{
	return Meter * MeterInUnreal;
}

bool UCSUtil::LocationPitchYaw(const FRotator& FrontRotation, const FVector& RelativeLocation, float& OutPitchDegree, float& OutYawDegree)
{
	const FVector LocalLocation = FrontRotation.UnrotateVector(RelativeLocation);
	FVector XZPlane = FVector::VectorPlaneProject(LocalLocation, FVector(0, 1, 0));
	FVector XYPlane = FVector::VectorPlaneProject(LocalLocation, FVector(0, 0, 1));
	XZPlane.Normalize();
	XYPlane.Normalize();
	float Pitch = FMath::RadiansToDegrees(FMath::Acos(XZPlane.X));
	if (0 > LocalLocation.Z)
	{
		Pitch = -Pitch;
	}
	float Yaw = FMath::RadiansToDegrees(FMath::Acos(XYPlane.X));
	if (0 > LocalLocation.Y)
	{
		Yaw = -Yaw;
	}
	OutPitchDegree = Pitch;
	OutYawDegree = Yaw;
	return 0 < LocalLocation.X;
}

FString UCSUtil::CombineLF(const FString& A, const FString& B)
{
	return A + TEXT('\n') + B;
}
