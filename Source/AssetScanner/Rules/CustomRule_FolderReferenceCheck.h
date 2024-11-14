// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetScannerSettings.h"
#include "CustomRuleIds.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetScanner/Core/CustomRuleBase.h"
#include "Misc/ScopedSlowTask.h"
#include "CustomRule_FolderReferenceCheck.generated.h"

#define LOCTEXT_NAMESPACE "AssetScannerRule"
class UAssetScannerSettings;
/**
 * 
 */
UCLASS(Config=AssetScanRule)
class ASSETSCANNER_API UCustomRule_FolderReferenceCheck : public UCustomRuleBase
{
	GENERATED_BODY()

public:

	virtual ECustomRuleIds GetRuleID() override
	{
		return ECustomRuleIds::FolderReferenceCheck;
	}
	virtual FString GetName() override
	{
		return TEXT("FolderReferenceCheck");
	}
	
	virtual FString GetDescription() override
	{
		return TEXT("Folder Reference Check");
	}
	
	virtual void InitDataTable(TObjectPtr<URuleDataTable> InDataTable) override
	{
		InDataTable->AddColumn(FColumnIds::Name, 0.25f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Path, 0.45f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Message, 0.3f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
	}

	void FindAssetReferences(const FString& AssetPath) const
	{
		TArray<FAssetData> AssetDataList;
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		// 搜索指定路径的资源
		AssetRegistryModule.Get().GetAssetsByPath(*AssetPath, AssetDataList, true);

		// 遍历资源并输出引用信息
		for (const FAssetData& AssetData : AssetDataList)
		{
			TArray<FName> Referencers;
			AssetRegistryModule.Get().GetReferencers(AssetData.PackageName, Referencers);
			UE_LOG(LogTemp, Log, TEXT("Asset: %s"), *AssetData.AssetName.ToString());
			for (const FName& Ref : Referencers)
			{
				FString RefStr = Ref.ToString();
				if (RefStr.StartsWith(AssetPath) == false)
				{
					// UE_LOG(LogTemp, Log, TEXT(" - Referenced by: %s"), *Ref.ToString());
					TSharedPtr<TArray<FString>> Row = MakeShareable<TArray<FString>>(new TArray<FString>);
					Row.Get()->Add(AssetData.AssetName.ToString());
					Row.Get()->Add(*AssetData.GetSoftObjectPath().ToString());
					Row.Get()->Add(FString::Printf(TEXT("Referenced by: %s"), *RefStr));
					this->DataTable->AddRow(Row);
				}
			}
		}
	}

	virtual bool CheckImplement() override
	{
		UAssetScannerSettings* Settings = GetMutableDefault<UAssetScannerSettings>();
		for (const FDirectoryPath& DirPath : Settings->IsolatedReferencePaths)
		{
			FindAssetReferences(DirPath.Path);
		}
		
		return false;
	}
};
#undef LOCTEXT_NAMESPACE