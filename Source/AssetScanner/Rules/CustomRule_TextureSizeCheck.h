// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomRuleIds.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetScanner/Core/CustomRuleBase.h"
#include "Engine/AssetManager.h"
#include "CustomRule_TextureSizeCheck.generated.h"

#define LOCTEXT_NAMESPACE "AssetScanner"
/**
 * 
 */
UCLASS(Config=AssetScanRule)
class ASSETSCANNER_API UCustomRule_TextureSizeCheck : public UCustomRuleBase
{
	GENERATED_BODY()

public:
	// TODO 可能要针对不同文件夹做不同的阈值
	UPROPERTY(Config, EditAnywhere)
	int32 ThresholdSize = 512;
	
	virtual ECustomRuleIds GetRuleID_Implementation() override
	{
		return ECustomRuleIds::TextureSizeCheck;
	}

	virtual FString GetName_Implementation() override
	{
		return TEXT("TextureSizeCheck");
	}
	
	virtual FString GetDescription_Implementation() override
	{
		// | 用来分割web 显示 检测规则名称 | 阈值
		return FString::Printf(TEXT("尺寸过大的纹理|长或宽>%d"), ThresholdSize);
	}

	virtual void InitDataTable_Implementation(URuleDataTable* InDataTable) override
	{
		InDataTable->AddColumn(FColumnIds::Name, 3, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Path, 6, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn("Width", 2, UObject::FAssetRegistryTag::ETagType::TT_Numerical);
		InDataTable->AddColumn("Height", 2, UObject::FAssetRegistryTag::ETagType::TT_Numerical);
	}
	
	virtual bool CheckImplement_Implementation() override
	{
		// FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		UAssetManager& Manager = UAssetManager::Get();
		IAssetRegistry& AssetRegistry = Manager.GetAssetRegistry();

		// 创建一个过滤器来查找所有 Texture 类的资产
		FARFilter Filter;
		Filter.ClassPaths.Add(UTexture::StaticClass()->GetClassPathName());
		Filter.bRecursiveClasses = true;	// 这样可以查找到所有的 Texture 的子类，如 Texture2D 等
		
		TArray<FAssetData> AssetDatas;
		AssetRegistry.GetAssets(Filter, AssetDatas);
	
		FScopedSlowTask SlowTask(AssetDatas.Num(), FText::FromString(TEXT("Processing assets...")));
		SlowTask.MakeDialog();

		for (int i = 0; i < AssetDatas.Num(); i++)
		{
			if (SlowTask.ShouldCancel())
			{
				break;
			}
			// 每次循环更新进度
			SlowTask.EnterProgressFrame(1, FText::FromString(FString::Printf(
				TEXT("Processing asset %d/%d"), i + 1, AssetDatas.Num())));
			const FAssetData& Asset = AssetDatas[i];
			UObject* AssetObj = UEditorAssetLibrary::LoadAsset(Asset.PackageName.ToString());
			UTexture* texture = Cast<UTexture>(AssetObj);
			if (texture == nullptr)
			{
				continue;
			}
			float width = texture->GetSurfaceWidth();
			float height = texture->GetSurfaceHeight();
			if (width <= ThresholdSize && height <= ThresholdSize)
			{
				continue;
			}
			
			TSharedPtr<TArray<FString>> Row = MakeShareable<TArray<FString>>(new TArray<FString>);
			Row.Get()->Add(Asset.AssetName.ToString());
			Row.Get()->Add(*Asset.GetSoftObjectPath().ToString());
			Row.Get()->Add(FString::Printf(TEXT("%.0f"), width));
			Row.Get()->Add(FString::Printf(TEXT("%.0f"), height));
			DataTable->AddRow(Row);
		}
		return true;
	}
};
#undef LOCTEXT_NAMESPACE