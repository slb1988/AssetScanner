#include "CustomRule_SceneActorFindComponent.h"

#include "AssetScannerSettings.h"
#include "EngineUtils.h"

void UCustomRule_SceneActorFindComponent::InitDataTable_Implementation(URuleDataTable* InDataTable)
{
	InDataTable->AddColumn(FColumnIds::Name, 0.25f, FAssetRegistryTag::ETagType::TT_Alphabetical);
	InDataTable->AddColumn(FColumnIds::Path, 0.5f, FAssetRegistryTag::ETagType::TT_Alphabetical);
	InDataTable->AddColumn(FColumnIds::SceneName, 0.4f, FAssetRegistryTag::ETagType::TT_Alphabetical);
}

bool UCustomRule_SceneActorFindComponent::IsSceneMatch(const FAssetData& AssetData, const UWorld* AssetObj)
{
	return AssetData.GetClass()->IsChildOf(UWorld::StaticClass());
}

bool UCustomRule_SceneActorFindComponent::DoesActorHaveAttentionComponents(AActor* Actor, const TArray<FSoftClassPath>& AttentionComponentClasses)
{
	if (!Actor || AttentionComponentClasses.Num() == 0)
	{
		return false;
	}

	// Convert FSoftClassPath to UClass* for comparison
	static TArray<UClass*> ComponentClasses;
	if (ComponentClasses.Num() == 0)
	{
		for (const FSoftClassPath& ClassPath : AttentionComponentClasses)
		{
			if (UClass* ResolvedClass = ClassPath.TryLoadClass<UActorComponent>())
			{
				ComponentClasses.Add(ResolvedClass);
			}
		}
	}

	// Check if the Actor has any of the specified components
	for (UClass* ComponentClass : ComponentClasses)
	{
		if (Actor->FindComponentByClass(ComponentClass))
		{
			return true;
		}
	}

	return false;
}

bool UCustomRule_SceneActorFindComponent::CheckScene_Implement(const FAssetData& AssetData, const UWorld* WorldObj)
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
		if (DoesActorHaveAttentionComponents(Actor, Settings->AttentionComponentClasses))
		{
			// 找到匹配的类，添加到结果表中
			TSharedPtr<TArray<FString>> Row = MakeShareable(new TArray<FString>);
			Row.Get()->Add(Actor->GetName());
			Row.Get()->Add(AssetData.GetObjectPathString());
			Row.Get()->Add(WorldObj->GetName());
				
			DataTable->AddRow(Row);
		}
	}

	return true;
}
