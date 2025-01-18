#pragma once
#include "CustomRule_SceneActorBase.h"
#include "CustomRule_SceneActorFindComponent.generated.h"

UCLASS(Blueprintable)
class UCustomRule_SceneActorFindComponent : public UCustomRule_SceneActorBase
{
	GENERATED_BODY()
	
public:
	virtual ECustomRuleIds GetRuleID_Implementation() override
	{
		return ECustomRuleIds::SceneActorFindComponent;
	}

	virtual FString GetName_Implementation() override
	{
		return TEXT("SceneActorFindComponent");
	}

	virtual FString GetDescription_Implementation() override
	{
		return TEXT("Find Scene Actor is Special Component");
	}

	virtual void InitDataTable_Implementation(URuleDataTable* InDataTable) override;

	virtual bool IsSceneMatch(const FAssetData& AssetData, const UWorld* WorldObj) override;

	virtual bool CheckScene_Implement(const FAssetData& AssetData, const UWorld* WorldObj) override;

private:
	bool DoesActorHaveAttentionComponents(AActor* Actor, const TArray<FSoftClassPath>& AttentionComponentClasses);
};
