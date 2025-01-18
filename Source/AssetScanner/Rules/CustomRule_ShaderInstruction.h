// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetScannerSettings.h"
#include "CustomRuleIds.h"
#include "MaterialStatsCommon.h"
#include "AssetScanner/Core/CustomRuleBase.h"
#include "Misc/ScopedSlowTask.h"
#include "CustomRule_ShaderInstruction.generated.h"

#define LOCTEXT_NAMESPACE "AssetScannerRule"
class UAssetScannerSettings;
/**
 * 
 */
UCLASS(Config=AssetScanRule)
class ASSETSCANNER_API UCustomRule_ShaderInstruction : public UCustomRuleBase
{
	GENERATED_BODY()

public:

	virtual ECustomRuleIds GetRuleID_Implementation() override
	{
		return ECustomRuleIds::ShaderInstruction;
	}
	virtual FString GetName_Implementation() override
	{
		return TEXT("ShaderInstruction");
	}
	
	virtual FString GetDescription_Implementation() override
	{
		return TEXT("ShaderInstruction");
	}
	
	virtual void InitDataTable_Implementation(URuleDataTable* InDataTable) override
	{
		InDataTable->AddColumn(FColumnIds::Name, 0.25f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Path, 0.45f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Instructions, 0.3f, UObject::FAssetRegistryTag::ETagType::TT_Numerical);
	}

	virtual bool IsMatchByData_Implementation(const FAssetData& AssetData) override
	{
		if (AssetData == nullptr)
			return false;

		if (AssetData.AssetClassPath.CompareFast(UMaterial::StaticClass()->GetClassPathName()) == 0)
			return true;

		return false;
	}

	virtual bool CheckImplementByData_Implementation(const FAssetData& AssetData, const UObject* AssetObj) override
	{
		const UMaterial* Mat = Cast<UMaterial>(AssetObj);
		if (Mat == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("%s is not a UMaterial"), *AssetData.PackageName.ToString());
			return false;
		}
		const FMaterialResource* MatRes = Mat->GetMaterialResource(ERHIFeatureLevel::SM5);
		if (MatRes == nullptr)
		{
			return false;
		}

		
		TMap<FName, TArray<FMaterialStatsUtils::FRepresentativeShaderInfo> > ShaderTypeNamesAndDescriptions;
		
		FMaterialStatsUtils::GetRepresentativeShaderTypesAndDescriptions(ShaderTypeNamesAndDescriptions, MatRes);

		UAssetScannerSettings* Settings = GetMutableDefault<UAssetScannerSettings>();
		
		const FMaterialShaderMap* MaterialShaderMap = MatRes->GetGameThreadShaderMap();
		if (!MaterialShaderMap)
		{
			return false;
		}
		
		for (auto& DescriptionPair : ShaderTypeNamesAndDescriptions)
		{
			const TArray<FMaterialStatsUtils::FRepresentativeShaderInfo>& DescriptionArray = DescriptionPair.Value;
			for (const FMaterialStatsUtils::FRepresentativeShaderInfo& ShaderInfo : DescriptionArray)
			{
				FShaderType* ShaderType = FindShaderTypeByName(ShaderInfo.ShaderName);
				const int32 NumInstructions = MaterialShaderMap->GetMaxNumInstructionsForShader(ShaderType);

				// 分平台、分ShaderType检测指令数量
				// if ((int)ShaderInfo.ShaderType == (int)Setting.ShaderType && NumInstructions > Setting->MaxShaderInstructionsNum)
				{
					TSharedPtr<TArray<FString>> Row = MakeShareable(new TArray<FString>);
					Row.Get()->Add(AssetData.AssetName.ToString());
					Row.Get()->Add(AssetData.GetSoftObjectPath().ToString());
					Row.Get()->Add(FString::FromInt(NumInstructions));
					DataTable->AddRow(Row);
				}
			}
		}
		return true;
	}
};
#undef LOCTEXT_NAMESPACE