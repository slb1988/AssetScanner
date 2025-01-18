// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuleDataTable.h"
#include "AssetScanner/Rules/CustomRuleIds.h"
#include "UObject/Object.h"
#include "CustomRuleBase.generated.h"

UENUM(BlueprintType)
enum class RulePriority : uint8
{
	Medium UMETA(DisplayName = "Medium"),
	High UMETA(DisplayName = "High")
};

/**
 * Base class for custom asset scanning rules
 * Can be extended in Blueprint
 */
UCLASS(Blueprintable, Abstract)
class ASSETSCANNER_API UCustomRuleBase : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Asset Scanner")
	bool Check()
	{
		if (!DataTable->IsValidLowLevel())
		{
			DataTable = NewObject<URuleDataTable>(this);
			InitDataTable(DataTable);
		}
		return CheckImplement();
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Asset Scanner")
	ECustomRuleIds GetRuleID();
	virtual ECustomRuleIds GetRuleID_Implementation() { return ECustomRuleIds::None; }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Asset Scanner")
	FString GetName();
	virtual FString GetName_Implementation() { return TEXT(""); }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Asset Scanner")
	FString GetDescription();
	virtual FString GetDescription_Implementation() { return TEXT(""); }

	UFUNCTION(BlueprintNativeEvent, Category = "Asset Scanner")
	void InitDataTable(URuleDataTable* InDataTable);
	
	virtual void InitDataTable_Implementation(URuleDataTable* InDataTable) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Asset Scanner")
	bool CheckImplement();
	virtual bool CheckImplement_Implementation() { return false; }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Asset Scanner")
	RulePriority GetPriority();
	virtual RulePriority GetPriority_Implementation() { return RulePriority::Medium; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Asset Scanner")
	virtual bool HasTable() { return Validate(); }

	// 根据单个FAssetData进行检测
	UFUNCTION(BlueprintNativeEvent, Category = "Asset Scanner")
	bool IsMatchByData(const FAssetData& AssetData);
	virtual bool IsMatchByData_Implementation(const FAssetData& AssetData) { return false; }

	UFUNCTION(BlueprintNativeEvent, Category = "Asset Scanner")
	bool CheckImplementByData(const FAssetData& AssetData, const UObject* AssetObj);
	virtual bool CheckImplementByData_Implementation(const FAssetData& AssetData, const UObject* AssetObj) { return false; }
	
	UFUNCTION(BlueprintCallable, Category = "Asset Scanner")
	void SaveToCSV(const FString& BaseDir)
	{
		if (!Validate())
		{
			UE_LOG(LogTemp, Error, TEXT("DataTable is null or error"));
			return;
		}
		FString CsvPath = FPaths::Combine(*BaseDir, *FString::Printf(TEXT("%s.csv"), *GetName()));
		DataTable->SaveToCSV(CsvPath);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Asset Scanner")
	const URuleDataTable* GetDataTable() {
		if (HasTable())
		{
			return DataTable;
		}
		else
		{
			return nullptr;
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Asset Scanner")
	bool Validate() const
	{
		if (DataTable == nullptr)
		{
			return false;
		}

		if (!DataTable->ValidateTable())
		{
			return false;
		}
		return true;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<URuleDataTable> DataTable;
};
