// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FAssetScannerStyle
{
public:
	static void Initialize();
	static void Shutdown();

	static FName GetStyleSetName();
private:
};
