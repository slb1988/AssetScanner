// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FAssetViewItemData;
struct FColumnInfo;
/**
 * 参考 Engine/Plugins/Editor/AssetManagerEditor/Source/AssetManagerEditor/Private/SAssetAuditBrowser.cpp
 */
class ASSETSCANNER_API SAssetScannerBrowser : public SCompoundWidget
{
	typedef TSharedPtr<FAssetViewItemData> ListViewItemPtr;
public:
	SLATE_BEGIN_ARGS(SAssetScannerBrowser)
		{
		}
		// SLATE_ARGUMENT(TSharedPtr<TArray<FColumnInfo>>, ColumnInfo)
		// SLATE_ARGUMENT(TSharedPtr<TArray<TSharedPtr<TArray<FString>>>>, RowItems)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual TSharedRef<SWidget> CreateToolBar();
	void OnItemDoubleClick(ListViewItemPtr Item);
	void InitCommandList();
	bool CanExecuteEditItems() { return true; }
	void ContextMenu_ExecuteEditItems() const;
	TSharedPtr<SWidget> OnOpenContextMenu();
	TSharedRef<SWidget> CreateContent();
	TSharedRef<SWidget> CreateFooter();

	TSharedRef<ITableRow> OnGenerateRowForListView(TSharedPtr<class FAssetViewItemData> RowData, const TSharedRef<STableViewBase>& OwnerTable);
	bool ContextMenu_FindInContentBrowser_CanExecute() const
	{
		return ListView->GetSelectedItems().Num() > 0;
	}

	void ContextMenu_FindInContentBrowser();

private:

	FReply ScanButtonClicked();	
	// void OnSelectedRuleIdChanged(const FText& Text);
	TSharedRef<SWidget> OnGenerateRuleComboContent(TSharedPtr<FString> InItem);
	void OnSelectedRuleComboChanged(TSharedPtr<FString> SelectedOptionString, ESelectInfo::Type ArgType);
	
private:
	TSharedPtr<SListView<TSharedPtr<FAssetViewItemData>>> ListView;
	TSharedPtr<SHeaderRow> HeaderRow;

	/**
	* Sort list data functions
	*/    
	EColumnSortMode::Type CurrentSortMode = EColumnSortMode::Type::None;
	FName SortedColumn = NAME_None;
	TSharedPtr<FUICommandList> CommandList;

	EColumnSortMode::Type GetSortMode(FName ColName) const
	{
		if (ColName != SortedColumn)
		{
			return EColumnSortMode::Type::None;
		}
		return CurrentSortMode;
	}

	void RefreshListView();
	virtual void OnSortModeChanged(EColumnSortPriority::Type SortPriority, const FName& ColName, EColumnSortMode::Type SortMode);

	TSharedPtr<SHeaderRow> BuildHeaderRow();

	TArray<TSharedPtr<FString>> EnumOptions;
	TSharedPtr<STextBlock> RuleComboDisplayTextBlock;
	int32 CurrentSelectedRuleId = -1;
	TArray<TSharedPtr<FColumnInfo>> ColumnInfos;
	// 用于过滤后显示的行信息
	TArray<TSharedPtr<FAssetViewItemData>> RowItems;

	// 场景选项相关
	TArray<TSharedPtr<FString>> SceneOptions;
	TSharedPtr<STextBlock> SceneComboDisplayTextBlock;
	FString CurrentSelectedScene = TEXT("All");

	// 场景选择相关函数
	void OnSelectedSceneComboChanged(TSharedPtr<FString> SelectedOptionString, ESelectInfo::Type ArgType);
	TSharedRef<SWidget> OnGenerateSceneComboContent(TSharedPtr<FString> InItem);
	
	// 用于过滤显示的行项
	void FilterRowsByScene();
	TArray<TSharedPtr<FAssetViewItemData>> AllRowItems; // 存储所有原始数据

	// 检查是否包含场景名称列
	bool HasSceneNameColumn() const;
	
	// Scene UI widgets
	TSharedPtr<SHorizontalBox> SceneFilterBox;
};
