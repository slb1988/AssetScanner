// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuleDataTable.h"
#include "AssetScanner/Rules/CustomRuleIds.h"
#include "UObject/Object.h"
#include "CustomRuleBase.generated.h"

UENUM()
enum class RulePriority
{
	Medium,
	High
};

/**
 * 
 */
UCLASS(Abstract)
class ASSETSCANNER_API UCustomRuleBase : public UObject
{
	GENERATED_BODY()
public:

	bool Check()
	{
		if (!DataTable->IsValidLowLevel())
		{
			DataTable = NewObject<URuleDataTable>(this);
			InitDataTable(DataTable);
		}
		return CheckImplement();
	}
	virtual ECustomRuleIds GetRuleID() PURE_VIRTUAL(UCustomRuleBase::GetRuleID, return ECustomRuleIds::None; );
	virtual FString GetName() PURE_VIRTUAL(UCustomRuleBase::GetName, return TEXT(""); );
	virtual FString GetDescription() PURE_VIRTUAL(UCustomRuleBase::GetDescription, return TEXT(""); );
	virtual void InitDataTable(TObjectPtr<URuleDataTable> InDataTable) PURE_VIRTUAL(UCustomRuleBase::InitDataTable, )
	virtual bool CheckImplement() { return false; }
	
	virtual RulePriority GetPriority() { return RulePriority::Medium; }
	virtual bool HasTable() { return Validate(); }

	// 根据单个FAssetData进行检测
	virtual bool IsMatchByData(const FAssetData& AssetData) { return false; }
	virtual bool CheckImplementByData(const FAssetData& AssetData, const UObject* AssetObj) { return false; }
	
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

	const TObjectPtr<URuleDataTable> GetDataTable() {
		if (HasTable())
		{
			return DataTable;
		}
		else
		{
			return nullptr;
		}
	}

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
	UPROPERTY()
	TObjectPtr<URuleDataTable> DataTable;
};
