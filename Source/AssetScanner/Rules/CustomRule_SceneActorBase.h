// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomRuleIds.h"
#include "AssetScanner/Core/CustomRuleBase.h"
#include "Misc/ScopedSlowTask.h"
#include "CustomRule_SceneActorBase.generated.h"

#define LOCTEXT_NAMESPACE "AssetScannerRule"

/**
 * 
 */
UCLASS(Config=AssetScanRule, Abstract)
class ASSETSCANNER_API UCustomRule_SceneActorBase : public UCustomRuleBase
{
	GENERATED_BODY()

public:

	virtual ECustomRuleIds GetRuleID_Implementation() override
	{
		return ECustomRuleIds::None;
	}
	virtual FString GetName_Implementation() override
	{
		return TEXT("SceneActorBase");
	}
	
	virtual FString GetDescription_Implementation() override
	{
		return TEXT("SceneActorBase");
	}
	
	virtual void InitDataTable_Implementation(URuleDataTable* InDataTable) override PURE_VIRTUAL(UCustomRuleBase::InitDataTable, );

	virtual bool IsSceneMatch(const FAssetData& AssetData, const UWorld* WorldObj)
	{
		return false;
	}
	
	virtual bool CheckScene_Implement(const FAssetData& AssetData, const UWorld* WorldObj) PURE_VIRTUAL(CheckScene, return false;);

};
#undef LOCTEXT_NAMESPACE