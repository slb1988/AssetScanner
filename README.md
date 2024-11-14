# AssetCleanner
Unreal Engine 5 plugin use for Asset Checking with CustomRules
虚幻引擎5的插件，用来资源监测，让问题左移在暴露问题前提前发现

# 程序入口
Tools/AssetCleannerBrowser
Tools/AssetCleannerCommandlet

# 使用指南
添加新规则，一般只需要修改 CustomRuleIds.h 和 添加 CustomRule_ 继承UCustomRuleBase即可
具体使用方法见 插件中的CustomRule_xxx.cpp

如果能帮上忙，请点个star，有问题提pull request， 谢谢

