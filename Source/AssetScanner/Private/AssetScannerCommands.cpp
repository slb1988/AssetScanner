#include "AssetScannerCommands.h"

#define LOCTEXT_NAMESPACE "AssetScanner"

void FAssetScannerCommands::RegisterCommands()
{
	const FText CommandletShortName = LOCTEXT("Commandlet", "Asset Scanner Commandlet");
	const FText CommandletDescription = LOCTEXT("Commandlet_Tips", "Execute Asset Scanner Commandlet");

	FUICommandInfo::MakeCommandInfo(
		this->AsShared(),
		Command_RunCommandlet,
		"Asset Scanner Commandlet",
		CommandletShortName,
		CommandletDescription,
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "MainFrame.SaveAll"),
		EUserInterfaceActionType::Button,
		FInputChord());
	
	const FText BrowserShortName = LOCTEXT("Browser", "Asset Scanner Browser");
	const FText BrowserDescription = LOCTEXT("Browser_Tips", "Open Asset Scanner Browser");

	FUICommandInfo::MakeCommandInfo(
		this->AsShared(),
		Command_OpenContentBrowser,
		*BrowserShortName.ToString(),
		BrowserShortName,
		BrowserDescription,
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports"),
		EUserInterfaceActionType::Button,
		FInputChord());
	// UI_COMMAND(CommandletAction, "Asset Scanner Commandlet", "Execute Asset Scanner Commandlet", EUserInterfaceActionType::Button, FInputChord());
	// UI_COMMAND(ContentBrowserAction, "Asset Scanner Browser", "Bring up Asset Scanner window", EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::T ));
	
	UI_COMMAND(Command_FindInContentBrowser,
		"Browse to Asset", 
		"Browses to the associated asset and selects it in the most recently used Content Browser (summoning one if necessary)", 
		EUserInterfaceActionType::Button, 
		FInputChord(EModifierKey::Control, EKeys::B));
}

#undef LOCTEXT_NAMESPACE