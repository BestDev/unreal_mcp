// Copyright Epic Games, Inc. All Rights Reserved.

#include "IUnrealMCP.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/AppStyle.h"
#include "UnrealMCPEditorSubsystem.h"
#include "Widgets/SMCPServerControlWidget.h"

DEFINE_LOG_CATEGORY(LogUnrealMCP);

#define LOCTEXT_NAMESPACE "FUnrealMCPModule"

/**
 * Main module implementation for UnrealMCP plugin
 * Provides MCP server management through Level Editor toolbar integration
 */
class FUnrealMCPModule : public IUnrealMCP
{
public:
	/** Constructor */
	FUnrealMCPModule()
	{
		UE_LOG(LogUnrealMCP, Log, TEXT("UnrealMCP Module Constructor"));
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Initialize Level Editor toolbar extension */
	void InitializeToolbarExtension();

	/** Cleanup toolbar extension */
	void CleanupToolbarExtension();

	/** Callback for MCP Server button click */
	void OnMCPServerButtonClicked();

	/** Create and show MCP Server Control window */
	void ShowMCPServerControlWindow();

	/** Check if MCP Server Control window is already open */
	bool IsMCPServerControlWindowOpen() const;

	/** Reference to the MCP Server Control window */
	TWeakPtr<SWindow> MCPServerControlWindowPtr;
};

void FUnrealMCPModule::StartupModule()
{
	UE_LOG(LogUnrealMCP, Log, TEXT("UnrealMCP Module Starting Up"));

	// Initialize toolbar extension after ToolMenus system is ready
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUnrealMCPModule::InitializeToolbarExtension));

	UE_LOG(LogUnrealMCP, Log, TEXT("UnrealMCP Module Startup Completed"));
}

void FUnrealMCPModule::InitializeToolbarExtension()
{
	UE_LOG(LogUnrealMCP, Log, TEXT("Initializing UnrealMCP Toolbar Extension"));

	// Extend the Level Editor toolbar
	UToolMenu* LevelEditorToolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	if (LevelEditorToolbar)
	{
		FToolMenuSection& MCPSection = LevelEditorToolbar->FindOrAddSection("UnrealMCP");
		MCPSection.AddEntry(FToolMenuEntry::InitToolBarButton(
			"MCPServerControl",
			FUIAction(FExecuteAction::CreateRaw(this, &FUnrealMCPModule::OnMCPServerButtonClicked)),
			LOCTEXT("MCPServerButtonLabel", "MCP Server"),
			LOCTEXT("MCPServerButtonTooltip", "Open MCP Server Control Panel"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings")
		));

		UE_LOG(LogUnrealMCP, Log, TEXT("MCP Server button added to Level Editor toolbar"));
	}
	else
	{
		UE_LOG(LogUnrealMCP, Warning, TEXT("Failed to extend Level Editor toolbar"));
	}
}

void FUnrealMCPModule::CleanupToolbarExtension()
{
	// Cleanup is handled automatically by ToolMenus system when the module unloads
	UE_LOG(LogUnrealMCP, Log, TEXT("UnrealMCP Toolbar Extension Cleanup"));
}

void FUnrealMCPModule::OnMCPServerButtonClicked()
{
	UE_LOG(LogUnrealMCP, Log, TEXT("MCP Server button clicked"));

	// Check if window is already open
	if (IsMCPServerControlWindowOpen())
	{
		// Bring existing window to front
		if (TSharedPtr<SWindow> ExistingWindow = MCPServerControlWindowPtr.Pin())
		{
			ExistingWindow->BringToFront();
		}
		return;
	}

	// Create and show new window
	ShowMCPServerControlWindow();
}

void FUnrealMCPModule::ShowMCPServerControlWindow()
{
	// Create MCP Server Control Widget
	TSharedRef<SMCPServerControlWidget> MCPControlWidget = SNew(SMCPServerControlWidget);

	// Create window
	TSharedRef<SWindow> MCPServerWindow = SNew(SWindow)
		.Title(LOCTEXT("MCPServerWindowTitle", "MCP Server Control"))
		.ClientSize(FVector2D(600, 400))
		.SizingRule(ESizingRule::UserSized)
		.SupportsMaximize(true)
		.SupportsMinimize(true)
		.HasCloseButton(true)
		.FocusWhenFirstShown(true)
		.ActivationPolicy(EWindowActivationPolicy::Always)
		[
			SNew(SBox)
			.Padding(FMargin(10.0f))
			[
				MCPControlWidget
			]
		];

	// Store weak reference to window
	MCPServerControlWindowPtr = MCPServerWindow;

	// Add window to application
	FSlateApplication::Get().AddWindow(MCPServerWindow);

	UE_LOG(LogUnrealMCP, Log, TEXT("MCP Server Control window created and shown"));
}

bool FUnrealMCPModule::IsMCPServerControlWindowOpen() const
{
	return MCPServerControlWindowPtr.IsValid() && MCPServerControlWindowPtr.Pin().IsValid();
}

void FUnrealMCPModule::ShutdownModule()
{
	UE_LOG(LogUnrealMCP, Log, TEXT("UnrealMCP Module Shutting Down"));

	// Close MCP Server Control window if open
	if (TSharedPtr<SWindow> Window = MCPServerControlWindowPtr.Pin())
	{
		Window->RequestDestroyWindow();
		MCPServerControlWindowPtr.Reset();
	}

	// Cleanup toolbar extension
	CleanupToolbarExtension();

	// Unregister from ToolMenus callbacks
	UToolMenus::UnRegisterStartupCallback(this);

	UE_LOG(LogUnrealMCP, Log, TEXT("UnrealMCP Module Shutdown Completed"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealMCPModule, UnrealMCP)