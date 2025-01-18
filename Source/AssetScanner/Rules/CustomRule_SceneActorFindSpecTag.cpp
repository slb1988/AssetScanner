#include "CustomRule_SceneActorFindSpecTag.h"

#include "AssetScannerSettings.h"
#include "EngineUtils.h"

void UCustomRule_SceneActorFindSpecTag::InitDataTable_Implementation(URuleDataTable* InDataTable)
{
	InDataTable->AddColumn(FColumnIds::Name, 0.25f, FAssetRegistryTag::ETagType::TT_Alphabetical);
	InDataTable->AddColumn(FColumnIds::Path, 0.5f, FAssetRegistryTag::ETagType::TT_Alphabetical);
	InDataTable->AddColumn(FColumnIds::SceneName, 0.4f, FAssetRegistryTag::ETagType::TT_Alphabetical);
}

bool UCustomRule_SceneActorFindSpecTag::IsSceneMatch(const FAssetData& AssetData, const UWorld* AssetObj)
{
	return AssetData.GetClass()->IsChildOf(UWorld::StaticClass());
}

bool UCustomRule_SceneActorFindSpecTag::CheckScene_Implement(const FAssetData& AssetData, const UWorld* WorldObj)
{
	// 获取设置中需要检查的类
	UAssetScannerSettings* Settings = GetMutableDefault<UAssetScannerSettings>();
	if (!Settings || Settings->AttentionClassPaths.IsEmpty())
	{
		return true;
	}

	// 遍历场景中的所有Actor
	for (TActorIterator<AActor> It(WorldObj); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsA<ALevelScriptActor>())
		{
			continue;
		}

		// 检查Actor的类是否在关注列表中
		TSet<AActor*> WarningActors;
		
		FString ActorClassPath = Actor->GetClass()->GetPathName();
		for (const FString& Tag : Settings->AttentionTags)
		{
			if (Actor->Tags.Contains(Tag))
			{
				WarningActors.Add(Actor);
			}
		}


		for (const AActor* const WActor : WarningActors)
		{
			// 找到匹配的类，添加到结果表中
			TSharedPtr<TArray<FString>> Row = MakeShareable(new TArray<FString>);
			Row.Get()->Add(WActor->GetName());
			Row.Get()->Add(AssetData.GetObjectPathString());
			Row.Get()->Add(WorldObj->GetName());
				
			DataTable->AddRow(Row);
		}
	}

	return true;
}
