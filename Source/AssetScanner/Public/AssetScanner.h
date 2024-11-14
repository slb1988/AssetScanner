// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"

/**
 * 
 */
class FAssetScanner
{
public:
	static void BuildMenuEntry(FMenuBuilder& Builder);
	static void RegisterMenus();
	
public:
	static void Initialize();
	static void Uninitialize();

	void Register();
	void Unregister();

private:
	TSharedRef<SDockTab> OnSpawnAssetScannerBrowser(const FSpawnTabArgs& SpawnTabArgs);
	// TSharedRef<SDockTab> OnSpawnSimpleSlateTab(const FSpawnTabArgs& SpawnTabArgs);

	static TSharedPtr<FAssetScanner> Instance;
};

class FAssetScannerActionCallbacks
{
public:
	static void ScanAssets();
	
	static void AssetScannerBrowser();
	
	static const class URuleDataTable* FindAndCheckCustomRules(int RuleId);
};