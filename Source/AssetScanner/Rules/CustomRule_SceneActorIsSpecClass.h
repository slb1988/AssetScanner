#pragma once
#include "CustomRuleIds.h"
#include "CustomRule_SceneActorBase.h"
#include "CustomRule_SceneActorIsSpecClass.generated.h"

UCLASS(Blueprintable)
class UCustomRule_SceneActorIsSpecClass : public UCustomRule_SceneActorBase
{
	GENERATED_BODY()
	
public:
	virtual ECustomRuleIds GetRuleID_Implementation() override
	{
		return ECustomRuleIds::SceneActorIsSpecClass;
	}

	virtual FString GetName_Implementation() override
	{
		return TEXT("SceneActorIsSpecClass");
	}

	virtual FString GetDescription_Implementation() override
	{
		return TEXT("Find Scene Actor is Special Class");
	}

	virtual void InitDataTable_Implementation(URuleDataTable* InDataTable) override;

	virtual bool IsSceneMatch(const FAssetData& AssetData, const UWorld* WorldObj) override;

	virtual bool CheckScene_Implement(const FAssetData& AssetData, const UWorld* WorldObj) override;
	
};
