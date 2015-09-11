// Copyright (c) Jong-il Hong 2015. All rights reserved.

#include "CoolSummerEditor.h"
#include "AnimGraphNode_CSAnimPlayer.h"

#include "CompilerResultsLog.h"
#include "GraphEditorActions.h"
#include "AssetRegistryModule.h"
#include "AnimationGraphSchema.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintActionFilter.h"
#include "EditorCategoryUtils.h"
#include "BlueprintNodeSpawner.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

/////////////////////////////////////////////////////
// FNewSequencePlayerAction

// Action to add a sequence player node to the graph
struct FNewSequencePlayerAction : public FEdGraphSchemaAction_K2NewNode
{
protected:
	FAssetData AssetInfo;
public:
	FNewSequencePlayerAction(const FAssetData& InAssetInfo, FText Title)
	{
		AssetInfo = InAssetInfo;

		UAnimGraphNode_CSAnimPlayer* Template = NewObject<UAnimGraphNode_CSAnimPlayer>();
		NodeTemplate = Template;

		MenuDescription = Title;
		TooltipDescription = TEXT("Evaluates an animation sequence to produce a pose");
		Category = TEXT("Animations");

		// Grab extra keywords
		Keywords = InAssetInfo.ObjectPath.ToString();
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override
	{
		UAnimGraphNode_CSAnimPlayer* SpawnedNode = CastChecked<UAnimGraphNode_CSAnimPlayer>(FEdGraphSchemaAction_K2NewNode::PerformAction(ParentGraph, FromPin, Location, bSelectNewNode));
		SpawnedNode->Node.Sequence = Cast<UAnimSequence>(AssetInfo.GetAsset());

		return SpawnedNode;
	}
};

/////////////////////////////////////////////////////
// UAnimGraphNode_CSAnimPlayer

UAnimGraphNode_CSAnimPlayer::UAnimGraphNode_CSAnimPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAnimGraphNode_CSAnimPlayer::PreloadRequiredAssets()
{
	PreloadObject(Node.Sequence);

	Super::PreloadRequiredAssets();
}

FLinearColor UAnimGraphNode_CSAnimPlayer::GetNodeTitleColor() const
{
	if ((Node.Sequence != NULL) && Node.Sequence->IsValidAdditive())
	{
		return FLinearColor(0.10f, 0.60f, 0.12f);
	}
	else
	{
		return FColor(200, 100, 100);
	}
}

FText UAnimGraphNode_CSAnimPlayer::GetTooltipText() const
{
	const bool bAdditive = ((Node.Sequence != NULL) && Node.Sequence->IsValidAdditive());
	return GetTitleGivenAssetInfo(FText::FromString(Node.Sequence->GetPathName()), bAdditive);
}

FText UAnimGraphNode_CSAnimPlayer::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Node.Sequence == nullptr)
	{
		return LOCTEXT("SequenceNullTitle", "CS Play (None)");
	}
	// @TODO: the bone can be altered in the property editor, so we have to 
	//        choose to mark this dirty when that happens for this to properly work
	else //if (!CachedNodeTitles.IsTitleCached(TitleType))
	{
		if (SyncGroup.GroupName == NAME_None)
		{
			TitleType = ENodeTitleType::ListView;

			if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
			{
				const bool bAdditive = Node.Sequence->IsValidAdditive();
				// FText::Format() is slow, so we cache this to save on performance
				CachedNodeTitles.SetCachedTitle(TitleType, GetTitleGivenAssetInfo(FText::FromName(Node.Sequence->GetFName()), bAdditive));
			}
		}
		else if (TitleType == ENodeTitleType::FullTitle)
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("Title"), CachedNodeTitles[ENodeTitleType::ListView]);
			Args.Add(TEXT("SyncGroup"), FText::FromName(SyncGroup.GroupName));

			// FText::Format() is slow, so we cache this to save on performance
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("SequenceNodeGroupWithSubtitle", "{Title}\nSync group {SyncGroup}"), Args));
		}
	}

	if (Node.NodeName != NAME_None)
	{
		return FText::FromString(FString("[") + Node.NodeName.ToString() + "] " + CachedNodeTitles[TitleType].ToString());
	}
	else
	{
		return CachedNodeTitles[TitleType];
	}
}

FText UAnimGraphNode_CSAnimPlayer::GetTitleGivenAssetInfo(const FText& AssetName, bool bKnownToBeAdditive)
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("AssetName"), AssetName);

	if (bKnownToBeAdditive)
	{
		return FText::Format(LOCTEXT("SequenceNodeTitleAdditive", "CS Play {AssetName} (additive)"), Args);
	}
	else
	{
		return FText::Format(LOCTEXT("SequenceNodeTitle", "CS Play {AssetName}"), Args);
	}
}

void UAnimGraphNode_CSAnimPlayer::GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	if ((ContextMenuBuilder.FromPin == NULL) || (UAnimationGraphSchema::IsPosePin(ContextMenuBuilder.FromPin->PinType) && (ContextMenuBuilder.FromPin->Direction == EGPD_Input)))
	{
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(ContextMenuBuilder.CurrentGraph);

		if (UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(Blueprint))
		{
			// Load the asset registry module
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

			FARFilter Filter;
			Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
			Filter.bRecursiveClasses = true;

			// Filter by skeleton
			FAssetData SkeletonData(AnimBlueprint->TargetSkeleton);
			Filter.TagsAndValues.Add(TEXT("Skeleton"), SkeletonData.GetExportTextName());

			// Find matching assets and add an entry for each one
			TArray<FAssetData> SequenceList;
			AssetRegistryModule.Get().GetAssets(Filter, /*out*/ SequenceList);

			for (auto AssetIt = SequenceList.CreateConstIterator(); AssetIt; ++AssetIt)
			{
				const FAssetData& Asset = *AssetIt;

				// Try to determine if the asset is additive (can't do it right now if the asset is unloaded)
				bool bAdditive = false;
				if (Asset.IsAssetLoaded())
				{
					if (UAnimSequence* Sequence = Cast<UAnimSequence>(Asset.GetAsset()))
					{
						bAdditive = Sequence->IsValidAdditive();
					}
				}

				// Create the menu item
				const FText Title = UAnimGraphNode_CSAnimPlayer::GetTitleGivenAssetInfo(FText::FromName(Asset.AssetName), bAdditive);
				TSharedPtr<FNewSequencePlayerAction> NewAction(new FNewSequencePlayerAction(Asset, Title));
				ContextMenuBuilder.AddAction(NewAction);
			}
		}
	}
}

FText UAnimGraphNode_CSAnimPlayer::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Animation);
}

void UAnimGraphNode_CSAnimPlayer::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	auto LoadedAssetSetup = [](UEdGraphNode* NewNode, bool /*bIsTemplateNode*/, TWeakObjectPtr<UAnimSequence> SequencePtr)
	{
		UAnimGraphNode_CSAnimPlayer* SequencePlayerNode = CastChecked<UAnimGraphNode_CSAnimPlayer>(NewNode);
		SequencePlayerNode->Node.Sequence = SequencePtr.Get();
	};

	auto UnloadedAssetSetup = [](UEdGraphNode* NewNode, bool bIsTemplateNode, const FAssetData AssetData)
	{
		UAnimGraphNode_CSAnimPlayer* SequencePlayerNode = CastChecked<UAnimGraphNode_CSAnimPlayer>(NewNode);
		if (bIsTemplateNode)
		{
			if (const FString* SkeletonTag = AssetData.TagsAndValues.Find(TEXT("Skeleton")))
			{
				SequencePlayerNode->UnloadedSkeletonName = *SkeletonTag;
			}
		}
		else
		{
			UAnimSequence* Sequence = Cast<UAnimSequence>(AssetData.GetAsset());
			check(Sequence != nullptr);
			SequencePlayerNode->Node.Sequence = Sequence;
		}
	};

	const UObject* QueryObject = ActionRegistrar.GetActionKeyFilter();
	if (QueryObject == nullptr)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		// define a filter to help in pulling UAnimSequence asset data from the registry
		FARFilter Filter;
		Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		Filter.bRecursiveClasses = true;
		// Find matching assets and add an entry for each one
		TArray<FAssetData> SequenceList;
		AssetRegistryModule.Get().GetAssets(Filter, /*out*/SequenceList);

		for (auto AssetIt = SequenceList.CreateConstIterator(); AssetIt; ++AssetIt)
		{
			const FAssetData& Asset = *AssetIt;

			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			if (Asset.IsAssetLoaded())
			{
				TWeakObjectPtr<UAnimSequence> AnimSequence = Cast<UAnimSequence>(Asset.GetAsset());
				NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(LoadedAssetSetup, AnimSequence);
				NodeSpawner->DefaultMenuSignature.MenuName = GetTitleGivenAssetInfo(FText::FromName(AnimSequence->GetFName()), AnimSequence->IsValidAdditive());
			}
			else
			{
				NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(UnloadedAssetSetup, Asset);
				NodeSpawner->DefaultMenuSignature.MenuName = GetTitleGivenAssetInfo(FText::FromName(Asset.AssetName), /*bKnownToBeAdditive =*/false);
			}
			ActionRegistrar.AddBlueprintAction(Asset, NodeSpawner);
		}
	}
	else if (const UAnimSequence* AnimSequence = Cast<UAnimSequence>(QueryObject))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());

		TWeakObjectPtr<UAnimSequence> SequencePtr = AnimSequence;
		NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(LoadedAssetSetup, SequencePtr);
		NodeSpawner->DefaultMenuSignature.MenuName = GetTitleGivenAssetInfo(FText::FromName(AnimSequence->GetFName()), AnimSequence->IsValidAdditive());

		ActionRegistrar.AddBlueprintAction(QueryObject, NodeSpawner);
	}
	else if (QueryObject == GetClass())
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		// define a filter to help in pulling UAnimSequence asset data from the registry
		FARFilter Filter;
		Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		Filter.bRecursiveClasses = true;
		// Find matching assets and add an entry for each one
		TArray<FAssetData> SequenceList;
		AssetRegistryModule.Get().GetAssets(Filter, /*out*/SequenceList);

		for (auto AssetIt = SequenceList.CreateConstIterator(); AssetIt; ++AssetIt)
		{
			const FAssetData& Asset = *AssetIt;
			if (Asset.IsAssetLoaded())
			{
				continue;
			}

			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(UnloadedAssetSetup, Asset);
			NodeSpawner->DefaultMenuSignature.MenuName = GetTitleGivenAssetInfo(FText::FromName(Asset.AssetName), /*bKnownToBeAdditive =*/false);
			ActionRegistrar.AddBlueprintAction(Asset, NodeSpawner);
		}
	}
}

bool UAnimGraphNode_CSAnimPlayer::IsActionFilteredOut(class FBlueprintActionFilter const& Filter)
{
	bool bIsFilteredOut = false;
	FBlueprintActionContext const& FilterContext = Filter.Context;

	for (UBlueprint* Blueprint : FilterContext.Blueprints)
	{
		if (UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(Blueprint))
		{
			if (Node.Sequence)
			{
				if (Node.Sequence->GetSkeleton() != AnimBlueprint->TargetSkeleton)
				{
					// Sequence does not use the same skeleton as the Blueprint, cannot use
					bIsFilteredOut = true;
					break;
				}
			}
			else
			{
				FAssetData SkeletonData(AnimBlueprint->TargetSkeleton);
				if (UnloadedSkeletonName != SkeletonData.GetExportTextName())
				{
					bIsFilteredOut = true;
					break;
				}
			}
		}
		else
		{
			// Not an animation Blueprint, cannot use
			bIsFilteredOut = true;
			break;
		}
	}
	return bIsFilteredOut;
}

void UAnimGraphNode_CSAnimPlayer::ValidateAnimNodeDuringCompilation(class USkeleton* ForSkeleton, class FCompilerResultsLog& MessageLog)
{
	if (Node.Sequence == NULL)
	{
		MessageLog.Error(TEXT("@@ references an unknown sequence"), this);
	}
	else
	{
		USkeleton* SeqSkeleton = Node.Sequence->GetSkeleton();
		if (SeqSkeleton&& // if anim sequence doesn't have skeleton, it might be due to anim sequence not loaded yet, @todo: wait with anim blueprint compilation until all assets are loaded?
			!SeqSkeleton->IsCompatible(ForSkeleton))
		{
			MessageLog.Error(TEXT("@@ references sequence that uses different skeleton @@"), this, SeqSkeleton);
		}
	}
}

void UAnimGraphNode_CSAnimPlayer::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	if (!Context.bIsDebugging)
	{
		// add an option to convert to single frame
		Context.MenuBuilder->BeginSection("AnimGraphNodeSequencePlayer", NSLOCTEXT("A3Nodes", "SequencePlayerHeading", "CS Anim Player"));
		{
			Context.MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().OpenRelatedAsset);
			Context.MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ConvertToSeqEvaluator);
		}
		Context.MenuBuilder->EndSection();
	}
}

void UAnimGraphNode_CSAnimPlayer::BakeDataDuringCompilation(class FCompilerResultsLog& MessageLog)
{
	UAnimBlueprint* AnimBlueprint = GetAnimBlueprint();
	Node.GroupIndex = AnimBlueprint->FindOrAddGroup(SyncGroup.GroupName);
	Node.GroupRole = SyncGroup.GroupRole;
}

void UAnimGraphNode_CSAnimPlayer::GetAllAnimationSequencesReferred(TArray<UAnimationAsset*>& ComplexAnims, TArray<UAnimSequence*>& AnimationSequences) const
{
	if (Node.Sequence)
	{
		UE_LOG(CSELog, Error, TEXT("GetAllAnimationSequencesReferred:: Not Implemented!"));
		//HandleAnimReferenceCollection(Node.Sequence, ComplexAnims, AnimationSequences);
	}
}

void UAnimGraphNode_CSAnimPlayer::ReplaceReferredAnimations(const TMap<UAnimationAsset*, UAnimationAsset*>& ComplexAnimsMap, const TMap<UAnimSequence*, UAnimSequence*>& AnimSequenceMap)
{
	HandleAnimReferenceReplacement(Node.Sequence, ComplexAnimsMap, AnimSequenceMap);
}


bool UAnimGraphNode_CSAnimPlayer::DoesSupportTimeForTransitionGetter() const
{
	return true;
}

UAnimationAsset* UAnimGraphNode_CSAnimPlayer::GetAnimationAsset() const
{
	return Node.Sequence;
}

const TCHAR* UAnimGraphNode_CSAnimPlayer::GetTimePropertyName() const
{
	return TEXT("InternalTimeAccumulator");
}

UScriptStruct* UAnimGraphNode_CSAnimPlayer::GetTimePropertyStruct() const
{
	return FAnimNode_SequencePlayer::StaticStruct();
}

FString UAnimGraphNode_CSAnimPlayer::GetNodeCategory() const
{
	return TEXT("CS");
}

#undef LOCTEXT_NAMESPACE

//UAnimGraphNode_CSAnimPlayer::UAnimGraphNode_CSAnimPlayer(const FObjectInitializer& Initializer)
//	: Super(Initializer)
//{
//
//}
//
//FText UAnimGraphNode_CSAnimPlayer::GetNodeTitle(ENodeTitleType::Type TitleType) const
//{
//	return FText::FromString(TEXT("AnimPlayer"));
//}
//FLinearColor UAnimGraphNode_CSAnimPlayer::GetNodeTitleColor() const
//{
//	return FLinearColor(0.3f, 0.75f, 0.75f);
//}
//FString UAnimGraphNode_CSAnimPlayer::GetNodeCategory() const
//{
//	return TEXT("CS");
//}
