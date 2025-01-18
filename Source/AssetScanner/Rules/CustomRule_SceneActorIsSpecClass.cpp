#include "CustomRule_SceneActorIsSpecClass.h"
#include "AssetScannerSettings.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/Level.h"
#include "Engine/LevelScriptActor.h"
#include "EngineUtils.h"
#include "Engine/Scene.h"

void UCustomRule_SceneActorIsSpecClass::InitDataTable_Implementation(URuleDataTable* InDataTable)
{
	InDataTable->AddColumn(FColumnIds::Name, 0.25f, FAssetRegistryTag::ETagType::TT_Alphabetical);
	InDataTable->AddColumn(FColumnIds::Path, 0.5f, FAssetRegistryTag::ETagType::TT_Alphabetical);
	InDataTable->AddColumn(FColumnIds::SceneName, 0.4f, FAssetRegistryTag::ETagType::TT_Alphabetical);
}

bool UCustomRule_SceneActorIsSpecClass::IsSceneMatch(const FAssetData& AssetData, const UWorld* AssetObj)
{
	// 只检查Level资产
	return AssetData.GetClass()->IsChildOf(UWorld::StaticClass());
}

bool UCustomRule_SceneActorIsSpecClass::CheckScene_Implement(const FAssetData& AssetData, const UWorld* WorldObj)
{
	if (!WorldObj)
	{
		return false;
	}

	const UWorld* World = WorldObj;
	if (!World)
	{
		return false;
	}

	// 获取设置中需要检查的类
	UAssetScannerSettings* Settings = GetMutableDefault<UAssetScannerSettings>();
	if (!Settings || Settings->AttentionClassPaths.IsEmpty())
	{
		return true;
	}

	// 遍历场景中的所有Actor
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsA<ALevelScriptActor>())
		{
			continue;
		}

		// 检查Actor的类是否在关注列表中
		FString ActorClassPath = Actor->GetClass()->GetPathName();
		for (const FSoftClassPath& ClassPath : Settings->AttentionClassPaths)
		{
			if (ActorClassPath.Contains(ClassPath.ToString()))
			{
				// 找到匹配的类，添加到结果表中
				TSharedPtr<TArray<FString>> Row = MakeShareable(new TArray<FString>);
				Row.Get()->Add(Actor->GetName());
				Row.Get()->Add(AssetData.GetObjectPathString());
				Row.Get()->Add(World->GetName());
				
				DataTable->AddRow(Row);
			}
		}
	}

	return true;
}
