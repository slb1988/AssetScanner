// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomRuleIds.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetScanner/Core/CustomRuleBase.h"
#include "Engine/AssetManager.h"
#include "Misc/ScopedSlowTask.h"
#include "CustomRule_StaticMeshVertCheck.generated.h"

#define LOCTEXT_NAMESPACE "AssetScannerRule"
/**
 * 
 */
UCLASS(Config=AssetScanRule)
class ASSETSCANNER_API UCustomRule_StaticMeshVertCheck : public UCustomRuleBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	int32 ThresholdTriangleCount = 2048;
	UPROPERTY(Config, EditAnywhere)
	int32 ThresholdVertexCount = 2048;
	
	virtual ECustomRuleIds GetRuleID_Implementation() override
	{
		return ECustomRuleIds::StaticMeshVertCheck;
	}
	virtual FString GetName_Implementation() override
	{
		return TEXT("StaticMeshVertCheck");
	}
	
	virtual FString GetDescription_Implementation() override
	{
		return TEXT("Static Mesh Vert Checker");
	}
	
	virtual void InitDataTable_Implementation(URuleDataTable* InDataTable) override
	{
		InDataTable->AddColumn(FColumnIds::Name, 0.25f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Path, 0.45f, UObject::FAssetRegistryTag::ETagType::TT_Alphabetical);
		InDataTable->AddColumn(FColumnIds::Vertices, 0.15f, UObject::FAssetRegistryTag::ETagType::TT_Numerical);
		InDataTable->AddColumn(FColumnIds::Triangles, 0.15f, UObject::FAssetRegistryTag::ETagType::TT_Numerical);
	}

	virtual bool CheckImplement_Implementation() override
	{
		UAssetManager& Manager = UAssetManager::Get();
		IAssetRegistry& AssetRegistry = Manager.GetAssetRegistry();
	
		TArray<FAssetData> AssetDatas;
		AssetRegistry.GetAssetsByClass(FTopLevelAssetPath("/Script/Engine", "StaticMesh"), AssetDatas);
	
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
			auto mesh = Cast<UStaticMesh>(AssetObj);
			if (mesh == nullptr)
			{
				continue;
			}
			// UE_LOG(LogTemp, Display, TEXT("Load Asset : %s"), *Asset.PackageName.ToString());
			if (mesh->IsA(UStaticMesh::StaticClass()))
			{
				int32 max_vert_num = 0;
				int32 max_tri_num = 0;
				for (int LODIndex = 0; LODIndex < mesh->GetNumLODs(); LODIndex++)
				{
					max_vert_num = FMath::Max(max_vert_num, mesh->GetNumVertices(LODIndex));
					max_tri_num = FMath::Max(max_tri_num, mesh->GetNumTriangles(LODIndex));
				}
				// UE_LOG(LogTemp, Warning, TEXT("Max Vertices : %d"), max_vert_num);
				// UE_LOG(LogTemp, Warning, TEXT("Max Triangles : %d"), max_tri_num);
				if (max_vert_num <= ThresholdVertexCount || max_tri_num <= ThresholdTriangleCount)
				{
					continue;
				}

				TSharedPtr<TArray<FString>> Row = MakeShareable(new TArray<FString>);
				Row.Get()->Add(Asset.AssetName.ToString());
				Row.Get()->Add(Asset.GetSoftObjectPath().ToString());
				Row.Get()->Add(FString::Printf(TEXT("%d"), max_vert_num));
				Row.Get()->Add(FString::Printf(TEXT("%d"), max_tri_num));
				DataTable->AddRow(Row);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("%s is not a Static Mesh"), *Asset.PackageName.ToString());
			}
		}
		return true;
	}
};
#undef LOCTEXT_NAMESPACE