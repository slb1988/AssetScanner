#pragma once
#include "CustomRule_SceneActorBase.h"
#include "CustomRule_SceneActorFindSpecTag.generated.h"

UCLASS(Blueprintable)
class UCustomRule_SceneActorFindSpecTag : public UCustomRule_SceneActorBase
{
	GENERATED_BODY()
	
public:
	virtual ECustomRuleIds GetRuleID_Implementation() override
	{
		return ECustomRuleIds::SceneActorFindSpecTag;
	}

	virtual FString GetName_Implementation() override
	{
		return TEXT("SceneActorFindSpecTag");
	}

	virtual FString GetDescription_Implementation() override
	{
		return TEXT("Find Scene Actor is Special Tag");
	}

	virtual void InitDataTable_Implementation(URuleDataTable* InDataTable) override;

	virtual bool IsSceneMatch(const FAssetData& AssetData, const UWorld* WorldObj) override;

	virtual bool CheckScene_Implement(const FAssetData& AssetData, const UWorld* WorldObj) override;
	
};
