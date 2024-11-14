// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetScannerSettings.h"
#include "CustomRuleIds.h"
#include "AssetScanner/Core/CustomRuleBase.h"
#include "Misc/ScopedSlowTask.h"
#include "CustomRule_AnimInstanceFunctionForbid.generated.h"

#define LOCTEXT_NAMESPACE "AssetScannerRule"
class UAssetScannerSettings;
/**
 * 
 */
UCLASS(Config=AssetScanRule)
class ASSETSCANNER_API UCustomRule_AnimInstanceFunctionForbid : public UCustomRuleBase
{
	GENERATED_BODY()

public:

	virtual ECustomRuleIds GetRuleID() override
	{
		return ECustomRuleIds::AnimInstanceFunctionForbid;
	}
	virtual FString GetName() override
	{
		return TEXT("AnimInstanceFunctionForbid");
	}
	
	virtual FString GetDescription() override
	{
		return TEXT("AnimInstance Function Forbidden");
	}
	
	virtual void InitDataTable(TObjectPtr<URuleDataTable> InDataTable) override
	{
		InDataTable->AddColumn(FColumnIds::Name, 0.25f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Path, 0.45f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Message, 0.3f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
	}

	virtual bool IsMatchByData(const FAssetData& AssetData) override
	{
		if (AssetData == nullptr)
			return false;

		if (AssetData.AssetClassPath.CompareFast(UAnimBlueprint::StaticClass()->GetClassPathName()) == 0)
			return true;

		return false;
	}

	virtual bool CheckImplementByData(const FAssetData& AssetData, const UObject* AssetObj) override
	{
		const UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AssetObj);
		if (AnimBP == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("%s is not a AnimBlueprint"), *AssetData.PackageName.ToString());
			return false;
		}
		UClass* Class = AnimBP->GeneratedClass;
		if (!IsValid(Class))
			return false;

		TArray<FName> generatedFunctionList;
		Class->GenerateFunctionList(generatedFunctionList);

		UAssetScannerSettings* Settings = GetMutableDefault<UAssetScannerSettings>();
		for (const FName& FunctionName : generatedFunctionList)
		{
			if (Settings->bDebugMode || Settings->IsForbiddenFunction(FunctionName))
			{
				TSharedPtr<TArray<FString>> Row = MakeShareable(new TArray<FString>);
				Row.Get()->Add(AssetData.AssetName.ToString());
				Row.Get()->Add(AssetData.GetSoftObjectPath().ToString());
				Row.Get()->Add(FunctionName.ToString());
				DataTable->AddRow(Row);					
			}
		}
		return true;
	}
};
#undef LOCTEXT_NAMESPACE