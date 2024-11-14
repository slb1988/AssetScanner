#pragma once

#define LOCTEXT_NAMESPACE "SAssetViewTableRow"
#include "AssetViewItemData.h"

class SAssetViewTableRow : public SMultiColumnTableRow<TSharedPtr<FString>>
{
public:

	SLATE_BEGIN_ARGS(SAssetViewTableRow) { }
	SLATE_END_ARGS()
	
	/**
	 * Constructs the widget.
	 *
	 * @param InArgs The construction arguments.
	 */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedPtr<FAssetViewItemData> InRowData)
	{
		RowData = InRowData;
		SMultiColumnTableRow<TSharedPtr<FString> >::Construct(FSuperRowType::FArguments(), InOwnerTableView);
	}

	/**
	 * Generates the widget for the specified column.
	 *
	 * @param ColumnName The name of the column to generate the widget for.
	 * @return The widget.
	 */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		FString OutString;
		UObject::FAssetRegistryTag::ETagType OutType;
		if (RowData.Get()->GetCustomColumnValue(ColumnName, OutString, &OutType))
		{
			if (OutType == UObject::FAssetRegistryTag::TT_Alphabetical)
			{
				return SNew(SBox)
					.Padding(FMargin(4.0f, 0.0f))
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[
						SNew(STextBlock)
							.Text(FText::FromString(OutString))
							.Justification(ETextJustify::Left)
					];
			}
			if (OutType == UObject::FAssetRegistryTag::TT_Numerical)
			{
				return SNew(SBox)
					.Padding(FMargin(4.0f, 0.0f))
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(OutString))
						.Justification(ETextJustify::Center)
					];
			}
			return SNew(SBox)
				.Padding(FMargin(4.0f, 0.0f))
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(OutString))
				];
		}
		// else if (ColumnName == TEXT("Date"))
		// {
		// 	return SNew(SBox)
		// 		.Padding(FMargin(4.0f, 0.0f))
		// 		.VAlign(VAlign_Center)
		// 		[
		// 			SNew(STextBlock)
		// 				.Text(LOCTEXT("TempDateColumn", "@todo gmp: implement"))
		// 		];
		// }
		// else if (ColumnName == TEXT("Name"))
		// {
		// 	return SNew(SBox)
		// 		.Padding(FMargin(4.0f, 0.0f))
		// 		.VAlign(VAlign_Center)
		// 		[
		// 			SNew(STextBlock)
		// 				.Text(LOCTEXT("TempNameColumn", "@todo gmp: implement"))
		// 		];
		// }
		// else if (ColumnName == TEXT("Owner"))
		// {
		// 	return SNew(SBox)
		// 		.Padding(FMargin(4.0f, 0.0f))
		// 		.VAlign(VAlign_Center)
		// 		[
		// 			SNew(STextBlock)
		// 				.Text(LOCTEXT("TempOwnerColumn", "@todo gmp: implement"))
		// 		];
		// }

		return SNullWidget::NullWidget;
	}

private:
	TSharedPtr<FAssetViewItemData> RowData;
};

#undef LOCTEXT_NAMESPACE