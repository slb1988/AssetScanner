// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetScannerBrowser.h"

#include "SAssetViewTableRow.h"
#include "SlateOptMacros.h"
#include "AssetScanner/Core/RuleDataTable.h"
#include "AssetScanner/Rules/CustomRuleIds.h"
#include "AssetScanner.h"
#include "AssetScannerCommands.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/ReflectedTypeAccessors.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "AssetScanner"
#define COLUMN_MIN_WIDTH  150.0f

void SAssetScannerBrowser::Construct(const FArguments& InArgs)
{
	ColumnInfos = TArray<TSharedPtr<FColumnInfo>>();
	RowItems = TArray<TSharedPtr<FAssetViewItemData>>();

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			CreateToolBar()
		]
		+ SVerticalBox::Slot()
		.Padding(5)
		[
			CreateContent()
		]
		+ SVerticalBox::Slot()
		.FillHeight(0.1f)
		[
			CreateFooter()
		]
	];

	InitCommandList();
}

TSharedRef<SWidget> SAssetScannerBrowser::CreateToolBar()
{
	UEnum* EnumPtr = StaticEnum<ECustomRuleIds>();
	for (ECustomRuleIds RuleId : TEnumRange<ECustomRuleIds>())
	{
		if (RuleId == ECustomRuleIds::None)
			continue;
		FString RuleName = EnumPtr->GetNameStringByValue(static_cast<int>(RuleId));
		EnumOptions.Add(MakeShared<FString>(RuleName));
	}

	SceneOptions.Empty();
	SceneOptions.Add(MakeShared<FString>(TEXT("All")));
	
	if (RowItems.Num() > 0)
	{
		TSet<FString> UniqueScenes;
		for (const auto& Item : RowItems)
		{
			FString SceneName;
			if (Item->GetCustomColumnValue(FColumnIds::SceneName, SceneName, nullptr))
			{
				UniqueScenes.Add(SceneName);
			}
		}
		
		for (const auto& Scene : UniqueScenes)
		{
			SceneOptions.Add(MakeShared<FString>(Scene));
		}
	}

	// 创建基础工具栏
	TSharedRef<SHorizontalBox> ToolBar = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("RuleId:")))
		]
		+ SHorizontalBox::Slot()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&EnumOptions)
			.OnSelectionChanged(this, &SAssetScannerBrowser::OnSelectedRuleComboChanged)
			.OnGenerateWidget(this, &SAssetScannerBrowser::OnGenerateRuleComboContent)
			.Content()
			[
				SAssignNew(RuleComboDisplayTextBlock, STextBlock)
				.Text_Lambda([this]()
				{
					return FText::FromString(TEXT("List Rule Option"));
				})
			]
		];

	// 创建场景筛选部分
	SAssignNew(SceneFilterBox, SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Scene:")))
		]
		+ SHorizontalBox::Slot()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&SceneOptions)
			.OnSelectionChanged(this, &SAssetScannerBrowser::OnSelectedSceneComboChanged)
			.OnGenerateWidget(this, &SAssetScannerBrowser::OnGenerateSceneComboContent)
			.Content()
			[
				SAssignNew(SceneComboDisplayTextBlock, STextBlock)
				.Text(FText::FromString(TEXT("All")))
			]
		];

	// 根据是否有SceneName列来设置场景筛选部分的可见性
	SceneFilterBox->SetVisibility(HasSceneNameColumn() ? EVisibility::Visible : EVisibility::Collapsed);

	// 添加场景筛选部分和扫描按钮到工具栏
	ToolBar->AddSlot()
	[
		SceneFilterBox.ToSharedRef()
	];

	ToolBar->AddSlot()
	[
		SNew(SButton)
		.Text(FText::FromString(TEXT("Scan")))
		.OnClicked(this, &SAssetScannerBrowser::ScanButtonClicked)
	];

	return ToolBar;
}

void SAssetScannerBrowser::OnItemDoubleClick(ListViewItemPtr Item)
{
	if (!Item.IsValid())
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("OnItemDoubleClick"));
}


void SAssetScannerBrowser::InitCommandList()
{
	CommandList = MakeShared<FUICommandList>();

	CommandList->MapAction(FAssetScannerCommands::Get().Command_FindInContentBrowser,
		FExecuteAction::CreateSP(this, &SAssetScannerBrowser::ContextMenu_FindInContentBrowser),
		FCanExecuteAction::CreateSP(this, &SAssetScannerBrowser::ContextMenu_FindInContentBrowser_CanExecute));
}

void SAssetScannerBrowser::ContextMenu_ExecuteEditItems() const
{
	const TArray<TSharedPtr<FAssetViewItemData>>& SelectedItems = ListView->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		// Find the UObject asset from the owner name path
		TArray<FString> SelectedPaths;
		for (const TSharedPtr<FAssetViewItemData>& ItemData : SelectedItems)
		{
			FString ObjectPathString;
			ItemData->GetCustomColumnValue(FColumnIds::Path, ObjectPathString, nullptr);
			SelectedPaths.Add(ObjectPathString);
		}
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		AssetEditorSubsystem->OpenEditorsForAssets(SelectedPaths);
	}
}

TSharedPtr<SWidget> SAssetScannerBrowser::OnOpenContextMenu()
{
	FMenuBuilder MenuBuilder(true, CommandList.ToSharedRef());
	MenuBuilder.AddMenuEntry
	(
		FAssetScannerCommands::Get().Command_FindInContentBrowser,
		NAME_None,
		TAttribute<FText>(),
		TAttribute<FText>(),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SystemWideCommands.FindInContentBrowser.Small")
		);
	// Edit
	MenuBuilder.AddMenuEntry(
		LOCTEXT("EditAsset", "Edit..."),
		LOCTEXT("EditAssetTooltip", "Opens the selected item(s) for edit."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Edit"),
		FUIAction(
			FExecuteAction::CreateSP(this, &SAssetScannerBrowser::ContextMenu_ExecuteEditItems),
			FCanExecuteAction::CreateSP(this, &SAssetScannerBrowser::CanExecuteEditItems)
		)
	);

	return MenuBuilder.MakeWidget();
}

void SAssetScannerBrowser::RefreshListView()
{
	ListView->RequestListRefresh();
}

void SAssetScannerBrowser::OnSortModeChanged(
	EColumnSortPriority::Type SortPriority, const FName& ColName, EColumnSortMode::Type SortMode)
{
	SortedColumn = ColName;
	CurrentSortMode = SortMode;

	if (CurrentSortMode != EColumnSortMode::Type::None && !SortedColumn.IsNone())
	{
		RowItems.Sort([ColName, SortMode](const TSharedPtr<FAssetViewItemData>& A,
		                                  const TSharedPtr<FAssetViewItemData>& B)
		{
			FString AColData, BColData;
			UObject::FAssetRegistryTag::ETagType TagType;
			if (A->GetCustomColumnValue(ColName, AColData, &TagType)
				&&
				B->GetCustomColumnValue(ColName, BColData, &TagType))
			{
				if (TagType == UObject::FAssetRegistryTag::ETagType::TT_Numerical)
				{
					float ANumValue = FCString::Atof(*AColData);
					float BNumValue = FCString::Atof(*BColData);
					if (SortMode == EColumnSortMode::Ascending)
					{
						return ANumValue < BNumValue;
					}
					else
					{
						return ANumValue > BNumValue;
					}
				}
				//if (TagType == UObject::FAssetRegistryTag::ETagType::TT_Alphabetical)
				if (SortMode == EColumnSortMode::Ascending)
					return AColData < BColData;
				else
				{
					return AColData > BColData;
				}
			}
			return false;
		});
	}

	RefreshListView();
}

TSharedPtr<SHeaderRow> SAssetScannerBrowser::BuildHeaderRow()
{
	SAssignNew(HeaderRow, SHeaderRow)
	.CanSelectGeneratedColumn(true)
	.ResizeMode(ESplitterResizeMode::FixedPosition);

	return HeaderRow;
}

TSharedRef<SWidget> SAssetScannerBrowser::CreateContent()
{
	return SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SAssignNew(ListView, SListView<TSharedPtr<FAssetViewItemData>>)
			.ItemHeight(24)
			.ListItemsSource(&RowItems)
			.OnGenerateRow(this, &SAssetScannerBrowser::OnGenerateRowForListView)
			.HeaderRow
			(
				this->BuildHeaderRow().ToSharedRef()
			)
			.AllowOverscroll(EAllowOverscroll::No)
			// .OnMouseButtonDoubleClick(this, &SAssetScannerBrowser::OnItemDoubleClick)
			.ConsumeMouseWheel(EConsumeMouseWheel::Always)
			.OnContextMenuOpening(FOnContextMenuOpening::CreateSP(this, &SAssetScannerBrowser::OnOpenContextMenu))
		];
}

TSharedRef<SWidget> SAssetScannerBrowser::CreateFooter()
{
	return SNew(SHorizontalBox)
		.Visibility(EVisibility::Visible)
		+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Status:")))
		]
		+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("当前状态详细信息")))
		];
}

void SAssetScannerBrowser::ContextMenu_FindInContentBrowser()
{
	FAssetData SelectedAsset;
	const TArray<TSharedPtr<FAssetViewItemData>>& SelectedItems = ListView->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		FString ObjectPathString;
		SelectedItems[0]->GetCustomColumnValue(FColumnIds::Path, ObjectPathString, nullptr);
		// 加载 AssetRegistry 模块
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		// 查找资源的 FAssetData
		FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPathString);

		// 检查资源是否有效
		if (AssetData.IsValid())
		{
			SelectedAsset = AssetData;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Asset not found for path: %s"), *ObjectPathString);
		}
	}

	if (SelectedAsset != nullptr)
	{
		// Highlight the asset in content browser
		const TArray<FAssetData>& Assets = { SelectedAsset };
		const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		ContentBrowserModule.Get().SyncBrowserToAssets(Assets);
	}
}

FReply SAssetScannerBrowser::ScanButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Scan RuleId : %d"), CurrentSelectedRuleId);
	if (CurrentSelectedRuleId < 0)
		return FReply::Handled();
	// 调用Delegate
	const URuleDataTable* tableData = FAssetScannerActionCallbacks::FindAndCheckCustomRules(CurrentSelectedRuleId);
	if (tableData == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("TableData is null"));
		return FReply::Handled();
	}
	ColumnInfos.Empty();
	ColumnInfos = *tableData->GetColumnInfos();
	
	// 更新 AllRowItems 和 RowItems
	AllRowItems.Empty();
	RowItems.Empty();
	for (TSharedPtr<TArray<FString>>& Item : *tableData->GetRows())
	{
		auto NewItem = MakeShared<FAssetViewItemData>(ColumnInfos, Item);
		AllRowItems.Add(NewItem);
		RowItems.Add(NewItem);
	}
	
	// 更新场景选项和UI可见性
	SceneOptions.Empty();
	bool bHasSceneColumn = HasSceneNameColumn();
	
	if (bHasSceneColumn)
	{
		SceneOptions.Add(MakeShared<FString>(TEXT("All")));
		
		TSet<FString> UniqueScenes;
		for (const auto& Item : AllRowItems)
		{
			FString SceneName;
			if (Item->GetCustomColumnValue(FColumnIds::SceneName, SceneName, nullptr))
			{
				UniqueScenes.Add(SceneName);
			}
		}
		
		for (const auto& Scene : UniqueScenes)
		{
			SceneOptions.Add(MakeShared<FString>(Scene));
		}
	}

	// 更新场景筛选控件的可见性
	if (SceneFilterBox.IsValid())
	{
		SceneFilterBox->SetVisibility(bHasSceneColumn ? EVisibility::Visible : EVisibility::Collapsed);
	}

	if (HeaderRow->IsParentValid())
	{
		HeaderRow.Get()->ClearColumns();
		for (const TSharedPtr<FColumnInfo>& ColumnInfo : ColumnInfos)
		{
			HeaderRow->AddColumn(
				SHeaderRow::Column(ColumnInfo.Get()->ColumnName)
				.DefaultLabel(FText::FromName(ColumnInfo.Get()->ColumnName))
				.HAlignCell(HAlign_Left)
				.HAlignHeader(HAlign_Center)
				.FillWidth(ColumnInfo.Get()->FillWidth)
				.VAlignCell(VAlign_Fill)
				.OnSort(FOnSortModeChanged::CreateRaw(this, &SAssetScannerBrowser::OnSortModeChanged))
				.SortMode(this, &SAssetScannerBrowser::GetSortMode, ColumnInfo.Get()->ColumnName)
			);
		}
	}
	if (ListView->IsParentValid())
	{
		ListView->RebuildList();
	}

	return FReply::Handled();
}

TSharedRef<SWidget> SAssetScannerBrowser::OnGenerateRuleComboContent(TSharedPtr<FString> InItem)
{
	TSharedRef<SWidget> ConstructedComboContent =
		SNew(STextBlock)
		.Text(FText::FromString(*InItem.Get()));
	return ConstructedComboContent;
}

void SAssetScannerBrowser::OnSelectedRuleComboChanged(TSharedPtr<FString> SelectedOptionString,
                                                      ESelectInfo::Type ArgType)
{
	FString RuleIdStr = *SelectedOptionString;
	UE_LOG(LogTemp, Display, TEXT("RuleId is : %s"), *RuleIdStr);
	UEnum* EnumPtr = StaticEnum<ECustomRuleIds>();
	int64 EnumValue = EnumPtr->GetValueByNameString(*SelectedOptionString.Get());
	if (EnumValue != INDEX_NONE)
	{
		CurrentSelectedRuleId = static_cast<int32>(EnumValue);
		RuleComboDisplayTextBlock->SetText(FText::FromString(*SelectedOptionString.Get()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RuleId not found : %s"), *(*SelectedOptionString.Get()));
	}
}

TSharedRef<ITableRow> SAssetScannerBrowser::OnGenerateRowForListView(TSharedPtr<FAssetViewItemData> RowData,
                                                                     const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SAssetViewTableRow, OwnerTable, RowData);
}

void SAssetScannerBrowser::OnSelectedSceneComboChanged(TSharedPtr<FString> SelectedOptionString, ESelectInfo::Type ArgType)
{
	if (SelectedOptionString.IsValid())
	{
		CurrentSelectedScene = *SelectedOptionString.Get();
		SceneComboDisplayTextBlock->SetText(FText::FromString(CurrentSelectedScene));
		FilterRowsByScene();
	}
}

TSharedRef<SWidget> SAssetScannerBrowser::OnGenerateSceneComboContent(TSharedPtr<FString> InItem)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*InItem.Get()));
}

void SAssetScannerBrowser::FilterRowsByScene()
{
	if (CurrentSelectedScene == TEXT("All"))
	{
		RowItems = AllRowItems;
	}
	else
	{
		RowItems.Empty();
		for (const auto& Item : AllRowItems)
		{
			FString SceneName;
			if (Item->GetCustomColumnValue(FColumnIds::SceneName, SceneName, nullptr) && 
				SceneName == CurrentSelectedScene)
			{
				RowItems.Add(Item);
			}
		}
	}
	
	RefreshListView();
}

bool SAssetScannerBrowser::HasSceneNameColumn() const
{
	if (ColumnInfos.Num() == 0)
		return false;

	for (const auto& Column : ColumnInfos)
	{
		if (Column->ColumnName == FColumnIds::SceneName)
		{
			return true;
		}
	}
	return false;
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
