// Copyright (c) Jong-il Hong 2015. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "CSCustomAnimNodes.h"
#include "CSAnimNodeAcceptable.generated.h"

/**
 * 
 */
UINTERFACE()
class COOLSUMMER_API UCSAnimNodeAcceptable : public UInterface
{
	GENERATED_UINTERFACE_BODY()	
};

class COOLSUMMER_API ICSAnimNodeAcceptable
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual void AcceptAnimPlayer(FAnimNode_CSAnimPlayer* Node) = 0;
};
