// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UnrealMCPEditorSubsystem.h"

class SButton;
class SCheckBox;
class SEditableTextBox;
class STextBlock;
class SVerticalBox;
class SHorizontalBox;
class UUnrealMCPEditorSubsystem;

/**
 * Slate widget for controlling and monitoring the MCP HTTP Server
 * Provides UI for starting/stopping server, configuring settings, and viewing request logs
 */
class UNREALMCP_API SMCPServerControlWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMCPServerControlWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual ~SMCPServerControlWidget();

private:
	// Widget references
	TSharedPtr<STextBlock> StatusTextBlock;
	TSharedPtr<STextBlock> PortTextBlock;
	TSharedPtr<STextBlock> UptimeTextBlock;
	TSharedPtr<STextBlock> RequestStatsTextBlock;

	TSharedPtr<SButton> StartButton;
	TSharedPtr<SButton> StopButton;
	TSharedPtr<SButton> RestartButton;
	TSharedPtr<SButton> TestButton;
	TSharedPtr<SButton> ClearLogsButton;

	TSharedPtr<SEditableTextBox> PortEditBox;
	TSharedPtr<SCheckBox> AutoStartCheckBox;

	TSharedPtr<SListView<TSharedPtr<FMCPRequestLogEntry>>> RequestLogListView;
	TArray<TSharedPtr<FMCPRequestLogEntry>> RequestLogEntries;

	// Subsystem reference
	TWeakObjectPtr<UUnrealMCPEditorSubsystem> MCPSubsystem;

	// Timer for updating UI
	FTimerHandle UpdateTimerHandle;

	// Widget construction methods
	TSharedRef<SWidget> CreateStatusSection();
	TSharedRef<SWidget> CreateControlSection();
	TSharedRef<SWidget> CreateSettingsSection();
	TSharedRef<SWidget> CreateRequestLogSection();

	// Event handlers
	FReply OnStartServerClicked();
	FReply OnStopServerClicked();
	FReply OnRestartServerClicked();
	FReply OnTestConnectionClicked();
	FReply OnClearLogsClicked();

	void OnPortTextChanged(const FText& Text);
	void OnPortTextCommitted(const FText& Text, ETextCommit::Type CommitType);
	void OnAutoStartCheckChanged(ECheckBoxState NewState);

	// Server status callbacks
	void OnServerStatusChanged(bool bIsRunning);

	// UI update methods
	void UpdateUI();
	void UpdateRequestLogList();
	void RefreshRequestStats();

	// List view methods
	TSharedRef<ITableRow> OnGenerateRequestLogRow(TSharedPtr<FMCPRequestLogEntry> Item, const TSharedRef<STableViewBase>& OwnerTable);

	// Utility methods
	FText GetStatusText() const;
	FSlateColor GetStatusColor() const;
	FText GetPortText() const;
	FText GetRequestStatsText() const;
	bool IsServerRunning() const;


	// Validation methods
	bool IsPortValid(const FString& PortString) const;
	int32 GetPortFromText() const;

	// Notification helpers
	void ShowNotification(const FText& Message, bool bIsError = false);
};