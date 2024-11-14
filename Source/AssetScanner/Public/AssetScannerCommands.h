// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AssetScannerStyle.h"

class FAssetScannerCommands : public TCommands<FAssetScannerCommands>
{
public:

	FAssetScannerCommands()
		: TCommands<FAssetScannerCommands>(TEXT("AssetScannerCommands"),
			NSLOCTEXT("Contexts", "AssetScanner", "AssetScanner Plugin"),
			NAME_None, FAssetScannerStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > Command_RunCommandlet;
	TSharedPtr< FUICommandInfo > Command_OpenContentBrowser;
	TSharedPtr< FUICommandInfo > Command_FindInContentBrowser;
};
