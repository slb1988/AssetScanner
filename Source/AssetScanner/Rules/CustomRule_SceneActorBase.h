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
UCLASS(Config=AssetScanRule)
class ASSETSCANNER_API UCustomRule_SceneActorBase : public UCustomRuleBase
{
	GENERATED_BODY()

public:

	virtual ECustomRuleIds GetRuleID() override
	{
		return ECustomRuleIds::None;
	}
	virtual FString GetName() override
	{
		return TEXT("SceneActorBase");
	}
	
	virtual FString GetDescription() override
	{
		return TEXT("SceneActorBase");
	}
	
	virtual void InitDataTable(TObjectPtr<URuleDataTable> InDataTable) override PURE_VIRTUAL(UCustomRuleBase::InitDataTable, );

	virtual bool IsSceneMatch(const FAssetData& AssetData, const UScene* AssetObj) PURE_VIRTUAL(IsSceneMatch, return false;);
	virtual bool CheckScene_Implement(const FAssetData& AssetData, const UScene* AssetObj) PURE_VIRTUAL(ScanScene, return false;);

};
#undef LOCTEXT_NAMESPACE