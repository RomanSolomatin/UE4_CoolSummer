// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummer.h"
#include "CSAnimNodeAcceptable.h"
#include "CSCustomAnimNodes.h"

void FAnimNode_CSAnimPlayer::Initialize(const FAnimationInitializeContext& Context)
{
	FAnimNode_SequencePlayer::Initialize(Context);
	if (NodeName != NAME_None && nullptr != Context.AnimInstance)
	{
		ICSAnimNodeAcceptable* I = Cast<ICSAnimNodeAcceptable>(Context.AnimInstance->GetOwningComponent());
		if (nullptr != I)
		{
			I->AcceptAnimPlayer(this);
		}
		else
		{
			I = Cast<ICSAnimNodeAcceptable>(Context.AnimInstance->GetOwningActor());
			if (nullptr != I)
			{
				I->AcceptAnimPlayer(this);
			}
		}
	}
}

void FAnimNode_CSAnimPlayer::SetCurrentTime(const float Seconds)
{
	InternalTimeAccumulator = Seconds;
}
