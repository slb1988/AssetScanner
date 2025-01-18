# AssetCleanner
Unreal Engine 5 plugin use for Asset Checking with CustomRules
虚幻引擎5的插件，用来资源监测，让问题左移在暴露问题前提前发现

# UI调用入口
- Tools/AssetCleannerBrowser

![](/Pic/AssetCleannerBrowser.jpg)

- Tools/AssetCleannerCommandlet

![](/Pic/AssetScannerCommandlet.png)


# 使用指南
添加新规则，一般只需要修改 CustomRuleIds.h 和 添加 CustomRule_ 继承UCustomRuleBase即可
具体使用方法见 插件中的CustomRule_xxx.cpp

重载
  virtual bool CheckImplement() { return false; }
或
	virtual bool IsMatchByData(const FAssetData& AssetData) { return false; }
	virtual bool CheckImplementByData(const FAssetData& AssetData, const UObject* AssetObj) { return false; }

方法二针对资源加载优化


针对场景Actor扫描，可以动态添加Combo筛选过滤SceneName
可以继承 UCustomRule_SceneActorBase，其他类似


如果能帮上忙，请点个Star，有问题提Pull Request

