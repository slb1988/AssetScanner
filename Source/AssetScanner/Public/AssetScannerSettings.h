// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AssetScannerSettings.generated.h"

// USTRUCT(BlueprintType)
// struct FShaderCheckMetrics
// {
// 	GENERATED_BODY()
// 	
// 	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
// 	// EShaderPlatformEx Platform = EShaderPlatformEx::SP_PCD3D_SM5;
//
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere)
// 	ERepresentativeShader ShaderType;
//
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere)
// 	int Num = 0;
// };
/**
 * 
 */

UCLASS(Config=EditorAssetScanner)
class ASSETSCANNER_API UAssetScannerSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Global")
	TArray<FDirectoryPath> BaseScanPaths = {{ "/Game/" } };
	
	UPROPERTY(Config, EditAnywhere, Category="Global")
	TArray<FDirectoryPath> ExcludedScanPaths;

	UPROPERTY(Config, EditAnywhere, Category="AnimInstance")
	bool bDebugMode;
	
	UPROPERTY(Config, EditAnywhere, Category="AnimInstance")
	TArray<FString> AnimInstFuncNameBlacklist;

	bool IsForbiddenFunction(FName FunctionName) const
	{
		if (AnimInstFuncNameBlacklist.Contains(FunctionName.ToString()))
		{
			return true;
		}
		return false;
	}

	UPROPERTY(Config, EditAnywhere, Category="Shader")
	int32 MaxShaderInstructionsNum = 1024;

	UPROPERTY(Config, EditAnywhere, Category="ReferenceCheck")
	TArray<FDirectoryPath> IsolatedReferencePaths;

private:
	void ConvertRelativePath(FString& InOutPath);
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
