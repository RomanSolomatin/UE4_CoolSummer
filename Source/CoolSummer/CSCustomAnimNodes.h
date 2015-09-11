// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "Animation/AnimNode_SequencePlayer.h"
#include "CSCustomAnimNodes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct COOLSUMMER_API FAnimNode_CSAnimPlayer : public FAnimNode_SequencePlayer
{
	GENERATED_USTRUCT_BODY()

public:
	virtual void Initialize(const FAnimationInitializeContext& Context) override;

	virtual void SetCurrentTime(const float Seconds);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault)) mutable FName NodeName;
};