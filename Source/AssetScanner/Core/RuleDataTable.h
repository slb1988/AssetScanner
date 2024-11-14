#pragma once

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "RuleDataTable.generated.h"

// 定义列信息结构体
USTRUCT()
struct FColumnInfo
{
    GENERATED_BODY()
    
public:
    FColumnInfo() = default;
    
    FName ColumnName;
    // 用于定义列的数据类型（例如：int, float, string）,用于排序
    UObject::FAssetRegistryTag::ETagType ColumnType;
    float FillWidth = 1.f;

    FColumnInfo(const FName& InName, const UObject::FAssetRegistryTag::ETagType& InType, float InFillWidth)
        : ColumnName(InName), ColumnType(InType), FillWidth(InFillWidth)
    {
        
    }
};

// DataTable 类
UCLASS()
class URuleDataTable : public UObject
{
    GENERATED_BODY()
    
private:
    TSharedPtr<TArray<TSharedPtr<FColumnInfo>>> ColumnInfos;  // 列信息集合
    TSharedPtr<TArray<TSharedPtr<TArray<FString>>>> Rows;     // 表格数据，每行是一个字符串数组

public:
    URuleDataTable()
    {
        Rows = MakeShareable(new TArray<TSharedPtr<TArray<FString>>>());
        ColumnInfos = MakeShareable(new TArray<TSharedPtr<FColumnInfo>>());
    }
    
    TSharedPtr<TArray<TSharedPtr<FColumnInfo>>> GetColumnInfos() const { return ColumnInfos; }
    TSharedPtr<TArray<TSharedPtr<TArray<FString>>>> GetRows() const { return Rows; }
    
    // 添加列
    // TODO 添加多语言支持，DisplayText，可以放到 FAssetScanViewItem 中实现
    TSharedPtr<FColumnInfo> AddColumn(const FName& ColumnName,
        float FillWidth = 1.f,
        const FAssetRegistryTag::ETagType& ColumnType = FAssetRegistryTag::ETagType::TT_Alphabetical)
    {
        TSharedPtr<FColumnInfo> ColumnInfo = MakeShareable(
            new FColumnInfo(ColumnName, ColumnType, FillWidth));
        ColumnInfos.Get()->Add(ColumnInfo);
        return ColumnInfo;
    }

    // 添加行数据
    bool AddRow(const TSharedPtr<TArray<FString>>& RowData)
    {
        // 校验列数
        if (RowData.Get()->Num() != ColumnInfos.Get()->Num())
        {
            UE_LOG(LogTemp, Warning, TEXT("Row data does not match the number of columns."));
            return false;
        }

        Rows.Get()->Add(RowData);
        return true;
    }

    // 保存为 CSV 文件
    bool SaveToCSV(const FString& FilePath)
    {
        // 检查是否有数据
        if (Rows.Get()->Num() == 0 || ColumnInfos.Get()->Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("No data to save."));
            return false;
        }
        if (ColumnInfos.Get()->Num() < 2)
        {
            UE_LOG(LogTemp, Warning, TEXT("Column must greater than 2"));
            return false;
        }

        // 拼接CSV内容
        FString CSVContent;

        // 添加Header
        for (int32 i = 0; i < ColumnInfos.Get()->Num(); i++)
        {
            CSVContent += ColumnInfos.Get()->GetData()[i].Get()->ColumnName.ToString();
            if (i < ColumnInfos.Get()->Num() - 1)
            {
                CSVContent += TEXT(",");
            }
        }
        CSVContent += TEXT("\n");
        
        for (int32 i = 0; i < ColumnInfos.Get()->Num(); i++)
        {
            CSVContent += FString::FromInt(ColumnInfos.Get()->GetData()[i].Get()->ColumnType);
            if (i < ColumnInfos.Get()->Num() - 1)
            {
                CSVContent += TEXT(",");
            }
        }
        CSVContent += TEXT("\n");

        // 添加数据行
        for (const TSharedPtr<TArray<FString>>& Row : *(Rows.Get()))
        {
            for (int32 i = 0; i < Row.Get()->Num(); i++)
            {
                CSVContent += Row.Get()->GetData()[i];
                if (i < Row.Get()->Num() - 1)
                {
                    CSVContent += TEXT(",");
                }
            }
            CSVContent += TEXT("\n");
        }

        // 写入文件
        return FFileHelper::SaveStringToFile(CSVContent, *FilePath);
    }

    // 列和行数校验
    bool ValidateTable() const
    {
        for (const TSharedPtr<TArray<FString>>& Row : *(Rows.Get()))
        {
            if (Row.Get()->Num() != ColumnInfos.Get()->Num())
            {
                return false;
            }
        }
        return true;
    }
};
