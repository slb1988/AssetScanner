// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetScannerModule.h"

#include "AssetScanner.h"
#include "AssetScannerSettings.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"

#define LOCTEXT_NAMESPACE "AssetScannerModule"

void FAssetScannerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FAssetScanner::Initialize();
	
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateStatic(&FAssetScanner::RegisterMenus));

	RegisterSettings();
}

void FAssetScannerModule::ShutdownModule()
{
	UnregisterSettings();
	
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FAssetScanner::Uninitialize();
}

void FAssetScannerModule::RegisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project",
			"Plugins", "AssetScanner",
			LOCTEXT("AssetScanner", "Asset Scanner"),
			LOCTEXT("AssetScannerDescription", "Configure the Rule AssetScanner plug-in."),
			GetMutableDefault<UAssetScannerSettings>()
		);
	}
}

/** Unregister the point cloud editor settings. */
void FAssetScannerModule::UnregisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "AssetScanner");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetScannerModule, AssetScannerEditor)