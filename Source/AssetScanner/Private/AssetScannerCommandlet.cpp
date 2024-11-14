// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetScannerCommandlet.h"

#include "AssetScanner.h"

int32 UAssetScannerCommandlet::Main(const FString& Params)
{
	// TODO 解析Params
	FAssetScannerActionCallbacks::FindAndCheckCustomRules(-1);
	
	return Super::Main(Params);
}
