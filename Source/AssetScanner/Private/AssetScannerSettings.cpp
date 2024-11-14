// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetScannerSettings.h"

void UAssetScannerSettings::ConvertRelativePath(FString& InOutPath)
{
	if (InOutPath.Find(TEXT("/Content")) >= 0)
	{
		FString FullPath = InOutPath;
		if (FullPath.EndsWith(TEXT("/")) == false)
		{
			FullPath = FullPath + TEXT("/");
		}

		// 将 Content 路径转换为 /Game/ 路径
		FString GamePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
		FullPath.RemoveFromStart(GamePath);  // 去除绝对路径的 Content 部分
		InOutPath = FString(TEXT("/Game/")) + FullPath;
	}
}
#if WITH_EDITOR
void UAssetScannerSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (FDirectoryPath& DirPath : BaseScanPaths)
	{
		ConvertRelativePath(DirPath.Path);
	}
	for (FDirectoryPath& DirPath : ExcludedScanPaths)
	{
		ConvertRelativePath(DirPath.Path);
	}
}
#endif