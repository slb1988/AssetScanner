#include "AssetScannerStyle.h"

void FAssetScannerStyle::Initialize()
{
}

void FAssetScannerStyle::Shutdown()
{
	
}

FName FAssetScannerStyle::GetStyleSetName()
{
	static FName StyleSetName = "AssetScannerStyle";
	return StyleSetName;
}
