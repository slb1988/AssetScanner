#pragma once

UENUM()
enum class ECustomRuleIds : int32
{
	None = 0,
	StaticMeshVertCheck = 1,
	TextureSizeCheck = 2,
	AnimInstanceFunctionForbid = 3,
	BlueprintCheck = 4,
	ShaderInstruction = 5,
	SkeletonMissingBones = 6,
	// 检测文件夹是否被外部引用
	FolderReferenceCheck = 7,
	// make sure to update MaxRuleId
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(ECustomRuleIds, ECustomRuleIds::Count);

/**
 * Label for the path column
 */
class FColumnIds
{
public:
	static const FName Path;
	static const FName Name;
	static const FName Vertices;
	static const FName Triangles;
	static const FName Message;
	static const FName Instructions;
};
