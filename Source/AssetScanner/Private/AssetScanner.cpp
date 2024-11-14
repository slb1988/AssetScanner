// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetScanner.h"

#include "AssetScannerCommands.h"
#include "AssetScannerSettings.h"
#include "EditorAssetLibrary.h"
#include "LevelEditor.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetScanner/Core/CustomRuleBase.h"
#include "AssetScanner/Slate/AssetScannerBrowser.h"
#include "Engine/AssetManager.h"
#include "Toolkits/AssetEditorToolkit.h"

UE_DISABLE_OPTIMIZATION

#define LOCTEXT_NAMESPACE "AssetScanner"

void FAssetScanner::BuildMenuEntry(FMenuBuilder& Builder)
{
	Builder.BeginSection(FName("AssetScanner"));
	Builder.AddMenuEntry(FAssetScannerCommands::Get().Command_RunCommandlet);
	Builder.AddMenuEntry(FAssetScannerCommands::Get().Command_OpenContentBrowser);
	Builder.EndSection();
}

void FAssetScanner::RegisterMenus()
{
	TSharedPtr<FExtender> Extender = MakeShared<FExtender>();
	
	TSharedPtr<FUICommandList> ActionList = MakeShareable(new FUICommandList);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	ActionList->MapAction( FAssetScannerCommands::Get().Command_OpenContentBrowser,
		FExecuteAction::CreateStatic( &FAssetScannerActionCallbacks::AssetScannerBrowser ));
	
	ActionList->MapAction(FAssetScannerCommands::Get().Command_RunCommandlet,
		FExecuteAction::CreateStatic(&FAssetScannerActionCallbacks::ScanAssets));

	{
		TSharedPtr<FExtender> NewMenuExtender = MakeShareable(new FExtender);
		NewMenuExtender->AddMenuExtension("Tools",
			EExtensionHook::After,
			ActionList,
			FMenuExtensionDelegate::CreateStatic(&FAssetScanner::BuildMenuEntry));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(NewMenuExtender);
	}
}

FName GAssetScannerBrowserTabName("AssetScannerBrowserTab");

TSharedPtr<FAssetScanner> FAssetScanner::Instance = nullptr;

void FAssetScanner::Initialize()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShared<FAssetScanner>();
		Instance->Register();
	}
}

void FAssetScanner::Uninitialize()
{
	if (Instance.IsValid())
	{
		Instance->Unregister();
		Instance = nullptr;
	}
}

void FAssetScanner::Register()
{
	FAssetScannerCommands::Register();
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(GAssetScannerBrowserTabName,
		FOnSpawnTab::CreateRaw(this, &FAssetScanner::OnSpawnAssetScannerBrowser))
			.SetDisplayName(LOCTEXT("BrowserTitle", "AssetScannerBrowser"))
			.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FAssetScanner::Unregister()
{
	FGlobalTabmanager::Get()->UnregisterTabSpawner(GAssetScannerBrowserTabName);

	FAssetScannerCommands::Unregister();
}

TSharedRef<SDockTab> FAssetScanner::OnSpawnAssetScannerBrowser(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.Label(LOCTEXT("BrowserTitle", "AssetScannerBrowser"))
		[
			SNew(SAssetScannerBrowser)
		];
}

void FAssetScannerActionCallbacks::ScanAssets()
{
	FindAndCheckCustomRules(-1);	
}

void FAssetScannerActionCallbacks::AssetScannerBrowser()
{
	extern FName GAssetScannerBrowserTabName;
	FGlobalTabmanager::Get()->TryInvokeTab(GAssetScannerBrowserTabName);
}

const URuleDataTable* FAssetScannerActionCallbacks::FindAndCheckCustomRules(int RuleId)
{
	FString BaseCsvDir = FPaths::Combine(FPaths::ProjectSavedDir(), "AssetScanner");

	TArray<TObjectPtr<UCustomRuleBase>> Rules;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (Class->IsChildOf(UCustomRuleBase::StaticClass()) && !Class->HasAnyClassFlags(CLASS_Abstract))
		{
			UCustomRuleBase* RuleInstance = NewObject<UCustomRuleBase>(GetTransientPackage(), Class);
			Rules.Add(RuleInstance);
			if (RuleId >= 0 && RuleId != static_cast<int32>(RuleInstance->GetRuleID()))
			{
				continue;
			}
			UE_LOG(LogTemp, Display, TEXT("Found Custom Rule: %s - %s"), *RuleInstance->GetName(), *RuleInstance->GetDescription());

			if (RuleInstance)
			{
				RuleInstance->Check();
			}
		}
	}
	UAssetScannerSettings* Settings = GetMutableDefault<UAssetScannerSettings>();

	// 统一加载资源，然后遍历所有规则，是一种优化方案
	FARFilter Filter;
	Filter.ClassPaths.Add(UObject::StaticClass()->GetClassPathName());
	for (FDirectoryPath& DirPath : Settings->BaseScanPaths)
	{
		Filter.PackagePaths.Add(FName(DirPath.Path));	
	}
	Filter.bRecursiveClasses = true;	// 这样可以查找到所有的 Texture 的子类，如 Texture2D 等
	Filter.bRecursivePaths = true;
	
	UAssetManager& Manager = UAssetManager::Get();
	IAssetRegistry& AssetRegistry = Manager.GetAssetRegistry();
	TArray<FAssetData> AssetDatas;
	AssetRegistry.GetAssets(Filter, AssetDatas);

	FScopedSlowTask SlowTask(AssetDatas.Num(), FText::FromString(TEXT("Processing assets...")));
	SlowTask.MakeDialog();

	for (int i = 0; i < AssetDatas.Num(); i++)
	{
		const FAssetData& AssetData = AssetDatas[i];
		if (SlowTask.ShouldCancel())
		{
			break;
		}
		// 每次循环更新进度
		SlowTask.EnterProgressFrame(1, FText::FromString(FString::Printf(
			TEXT("Processing asset %d/%d"), i + 1, AssetDatas.Num())));
		bool bIsExcluded = false;
		for (auto& DirPath : Settings->ExcludedScanPaths)
		{
			if (AssetData.PackageName.ToString().StartsWith(DirPath.Path))
			{
				bIsExcluded = true;
				break;
			}
		}
		if (bIsExcluded)
			continue;
		
		for (TObjectPtr<UCustomRuleBase> Rule : Rules)
		{
			if (RuleId >= 0 && RuleId != static_cast<int32>(Rule->GetRuleID()))
			{
				continue;
			}
			if (Rule->IsMatchByData(AssetData))
			{
				UObject* AssetObj = UEditorAssetLibrary::LoadAsset(AssetData.PackageName.ToString());
				UE_LOG(LogTemp, Display, TEXT("Found Match Rule: %s - %s"), *Rule.Get()->GetName(), *Rule.Get()->GetDescription());

				Rule->CheckImplementByData(AssetData, AssetObj);
			}
		}
	}

	for (TObjectPtr<UCustomRuleBase> Rule : Rules)
	{
		if (!Rule->Validate())
			continue;
		Rule->SaveToCSV(BaseCsvDir);
	}

	for (TObjectPtr<UCustomRuleBase> Rule : Rules)
	{
		if (static_cast<int32>(Rule.Get()->GetRuleID()) == RuleId)
		{
			UCustomRuleBase* a = Rule.Get();
			TObjectPtr<URuleDataTable> dt = a->GetDataTable();
			URuleDataTable* dt_ptr = dt;
			return dt_ptr;
		}
	}
	
	Rules.Empty();
	
	return nullptr;
}

#undef LOCTEXT_NAMESPACE

UE_ENABLE_OPTIMIZATION
