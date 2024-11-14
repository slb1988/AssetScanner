#pragma once
#include "AssetScanner/Core/RuleDataTable.h"

class FAssetViewItemData
{
private:
	FAssetViewItemData() = default;
public:
	FAssetViewItemData(TArray<TSharedPtr<FColumnInfo>> ColumnInfos,
		TSharedPtr<TArray<FString>> RowData)
	{
		for (int32 i = 0; i < ColumnInfos.Num(); i++)
		{
			const FName& ColName = ColumnInfos[i]->ColumnName;
			const FString& Content = RowData.Get()->GetData()[i];
			TTuple<FString, UObject::FAssetRegistryTag::ETagType> v = MakeTuple(
				Content, ColumnInfos[i].Get()->ColumnType);
			CachedCustomColumnData.Add(ColName, v);
			CachedCustomColumnDisplayText.Add(ColName, FText::FromName(ColName));
		}
		
	}

	bool GetCustomColumnDisplayValue(const FName& ColumnName, FText& OutText) const
	{
		if (const FText* DisplayValue = CachedCustomColumnDisplayText.Find(ColumnName))
		{
			OutText = *DisplayValue;
			return true;
		}

		return false;
	}

	bool GetCustomColumnValue(const FName ColumnName, FString& OutString, UObject::FAssetRegistryTag::ETagType* OutType) const
	{
		if (const auto* ColumnDataPair = CachedCustomColumnData.Find(ColumnName))
		{
			OutString = ColumnDataPair->Key;
			if (OutType)
			{
				*OutType = ColumnDataPair->Value;
			}
			return true;
		}
		return false;
	}

private:

	void ClearCachedCustomColumnData()
	{
		CachedCustomColumnData.Empty();
		CachedCustomColumnDisplayText.Empty();
	}

	TMap<FName, TTuple<FString, UObject::FAssetRegistryTag::ETagType>> CachedCustomColumnData;

	TMap<FName, FText> CachedCustomColumnDisplayText;
};
