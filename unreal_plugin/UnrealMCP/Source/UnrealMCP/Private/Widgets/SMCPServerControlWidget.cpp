// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/SMCPServerControlWidget.h"
#include "UnrealMCPEditorSubsystem.h"
#include "UnrealMCPSettings.h"
#include "IUnrealMCP.h"
#include "Editor.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Views/SHeaderRow.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SMCPServerControlWidget"

void SMCPServerControlWidget::Construct(const FArguments& InArgs)
{
	// Get the MCP subsystem
	MCPSubsystem = GEditor->GetEditorSubsystem<UUnrealMCPEditorSubsystem>();

	if (MCPSubsystem.IsValid())
	{
		// Bind to server status changes
		MCPSubsystem->OnServerStatusChanged.AddSP(this, &SMCPServerControlWidget::OnServerStatusChanged);
	}

	// Initialize request log entries
	UpdateRequestLogList();

	ChildSlot
	[
		SNew(SVerticalBox)

		// Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 8.0f, 8.0f, 4.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("Title", "MCP Server Control"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
		]

		// Status Section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 4.0f)
		[
			CreateStatusSection()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 2.0f)
		[
			SNew(SSeparator)
		]

		// Control Section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 4.0f)
		[
			CreateControlSection()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 2.0f)
		[
			SNew(SSeparator)
		]

		// Settings Section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 4.0f)
		[
			CreateSettingsSection()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 2.0f)
		[
			SNew(SSeparator)
		]

		// Request Log Section
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(8.0f, 4.0f)
		[
			CreateRequestLogSection()
		]
	];

	// Initial UI update
	UpdateUI();
}

SMCPServerControlWidget::~SMCPServerControlWidget()
{
	// Unbind delegates
	if (MCPSubsystem.IsValid())
	{
		MCPSubsystem->OnServerStatusChanged.RemoveAll(this);
	}
}

TSharedRef<SWidget> SMCPServerControlWidget::CreateStatusSection()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(8.0f)
		[
			SNew(SVerticalBox)

			// Section Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 4.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("StatusSectionTitle", "Server Status"))
				.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			]

			// Status Info
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				// Status
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(0.0f, 0.0f, 8.0f, 0.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("StatusLabel", "Status:"))
							.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
						]

						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SAssignNew(StatusTextBlock, STextBlock)
							.Text(this, &SMCPServerControlWidget::GetStatusText)
							.ColorAndOpacity(this, &SMCPServerControlWidget::GetStatusColor)
						]
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 2.0f, 0.0f, 0.0f)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(0.0f, 0.0f, 8.0f, 0.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("PortLabel", "Port:"))
							.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
						]

						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SAssignNew(PortTextBlock, STextBlock)
							.Text(this, &SMCPServerControlWidget::GetPortText)
						]
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 2.0f, 0.0f, 0.0f)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(0.0f, 0.0f, 8.0f, 0.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("RequestsLabel", "Requests:"))
							.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
						]

						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SAssignNew(RequestStatsTextBlock, STextBlock)
							.Text(this, &SMCPServerControlWidget::GetRequestStatsText)
						]
					]
				]
			]
		];
}

TSharedRef<SWidget> SMCPServerControlWidget::CreateControlSection()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(8.0f)
		[
			SNew(SVerticalBox)

			// Section Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 4.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ControlSectionTitle", "Server Control"))
				.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			]

			// Control Buttons
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 4.0f, 0.0f)
				[
					SAssignNew(StartButton, SButton)
					.Text(LOCTEXT("StartButton", "Start"))
					.ToolTipText(LOCTEXT("StartButtonTooltip", "Start the MCP HTTP server"))
					.OnClicked(this, &SMCPServerControlWidget::OnStartServerClicked)
					.IsEnabled_Lambda([this]() { return MCPSubsystem.IsValid() && !IsServerRunning(); })
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(4.0f, 0.0f)
				[
					SAssignNew(StopButton, SButton)
					.Text(LOCTEXT("StopButton", "Stop"))
					.ToolTipText(LOCTEXT("StopButtonTooltip", "Stop the MCP HTTP server"))
					.OnClicked(this, &SMCPServerControlWidget::OnStopServerClicked)
					.IsEnabled_Lambda([this]() { return MCPSubsystem.IsValid() && IsServerRunning(); })
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(4.0f, 0.0f)
				[
					SAssignNew(RestartButton, SButton)
					.Text(LOCTEXT("RestartButton", "Restart"))
					.ToolTipText(LOCTEXT("RestartButtonTooltip", "Restart the MCP HTTP server"))
					.OnClicked(this, &SMCPServerControlWidget::OnRestartServerClicked)
					.IsEnabled_Lambda([this]() { return MCPSubsystem.IsValid() && IsServerRunning(); })
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(4.0f, 0.0f, 0.0f, 0.0f)
				[
					SAssignNew(TestButton, SButton)
					.Text(LOCTEXT("TestButton", "Test"))
					.ToolTipText(LOCTEXT("TestButtonTooltip", "Test connection to the server"))
					.OnClicked(this, &SMCPServerControlWidget::OnTestConnectionClicked)
					.IsEnabled_Lambda([this]() { return MCPSubsystem.IsValid(); })
				]
			]
		];
}

TSharedRef<SWidget> SMCPServerControlWidget::CreateSettingsSection()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(8.0f)
		[
			SNew(SVerticalBox)

			// Section Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 4.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SettingsSectionTitle", "Settings"))
				.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			]

			// Port Setting
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 4.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 8.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("PortSettingLabel", "Port:"))
					.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.MaxWidth(100.0f)
				[
					SAssignNew(PortEditBox, SEditableTextBox)
					.Text(FText::FromString(FString::FromInt(UUnrealMCPSettings::Get() ? UUnrealMCPSettings::Get()->DefaultPort : 8080)))
					.OnTextChanged(this, &SMCPServerControlWidget::OnPortTextChanged)
					.OnTextCommitted(this, &SMCPServerControlWidget::OnPortTextCommitted)
					.ToolTipText(LOCTEXT("PortEditTooltip", "Set the server port (1024-65535)"))
				]
			]

			// Auto Start Setting
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SAssignNew(AutoStartCheckBox, SCheckBox)
					.IsChecked(UUnrealMCPSettings::Get() ? (UUnrealMCPSettings::Get()->bAutoStartServer ? ECheckBoxState::Checked : ECheckBoxState::Unchecked) : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SMCPServerControlWidget::OnAutoStartCheckChanged)
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				.Padding(8.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AutoStartLabel", "Auto-start server on editor startup"))
					.ToolTipText(LOCTEXT("AutoStartTooltip", "Automatically start the MCP server when the editor starts"))
				]
			]
		];
}

TSharedRef<SWidget> SMCPServerControlWidget::CreateRequestLogSection()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(8.0f)
		[
			SNew(SVerticalBox)

			// Section Title with Clear Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 4.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("RequestLogSectionTitle", "Request Log"))
					.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(ClearLogsButton, SButton)
					.Text(LOCTEXT("ClearLogsButton", "Clear"))
					.ToolTipText(LOCTEXT("ClearLogsTooltip", "Clear all request log entries"))
					.OnClicked(this, &SMCPServerControlWidget::OnClearLogsClicked)
					.ButtonStyle(FAppStyle::Get(), "FlatButton.Default")
				]
			]

			// Request Log List
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SAssignNew(RequestLogListView, SListView<TSharedPtr<FMCPRequestLogEntry>>)
				.ListItemsSource(&RequestLogEntries)
				.OnGenerateRow(this, &SMCPServerControlWidget::OnGenerateRequestLogRow)
				.SelectionMode(ESelectionMode::Single)
				.HeaderRow
				(
					SNew(SHeaderRow)

					+ SHeaderRow::Column("Timestamp")
					.DefaultLabel(LOCTEXT("TimestampColumn", "Time"))
					.FillWidth(0.25f)

					+ SHeaderRow::Column("Method")
					.DefaultLabel(LOCTEXT("MethodColumn", "Method"))
					.FillWidth(0.15f)

					+ SHeaderRow::Column("Path")
					.DefaultLabel(LOCTEXT("PathColumn", "Path"))
					.FillWidth(0.4f)

					+ SHeaderRow::Column("StatusCode")
					.DefaultLabel(LOCTEXT("StatusCodeColumn", "Status"))
					.FillWidth(0.1f)

					+ SHeaderRow::Column("ClientIP")
					.DefaultLabel(LOCTEXT("ClientIPColumn", "Client IP"))
					.FillWidth(0.1f)
				)
			]
		];
}

// Event Handlers

FReply SMCPServerControlWidget::OnStartServerClicked()
{
	if (MCPSubsystem.IsValid())
	{
		int32 Port = FCString::Atoi(*PortEditBox->GetText().ToString());
		if (MCPSubsystem->StartServer(Port))
		{
			ShowNotification(FText::Format(LOCTEXT("ServerStarted", "MCP Server started on port {0}"), FText::FromString(FString::FromInt(Port))));
		}
		else
		{
			ShowNotification(LOCTEXT("ServerStartFailed", "Failed to start MCP Server"), true);
		}
	}
	return FReply::Handled();
}

FReply SMCPServerControlWidget::OnStopServerClicked()
{
	if (MCPSubsystem.IsValid())
	{
		MCPSubsystem->StopServer();
		ShowNotification(LOCTEXT("ServerStopped", "MCP Server stopped"));
	}
	return FReply::Handled();
}

FReply SMCPServerControlWidget::OnRestartServerClicked()
{
	if (MCPSubsystem.IsValid())
	{
		MCPSubsystem->RestartServer();
		ShowNotification(LOCTEXT("ServerRestarted", "MCP Server restarted"));
	}
	return FReply::Handled();
}

FReply SMCPServerControlWidget::OnTestConnectionClicked()
{
	if (MCPSubsystem.IsValid())
	{
		bool bConnectionSuccessful = MCPSubsystem->TestConnection();
		if (bConnectionSuccessful)
		{
			ShowNotification(LOCTEXT("ConnectionTestSuccess", "Connection test successful"));
		}
		else
		{
			ShowNotification(LOCTEXT("ConnectionTestFailed", "Connection test failed"), true);
		}
	}
	return FReply::Handled();
}

FReply SMCPServerControlWidget::OnClearLogsClicked()
{
	if (MCPSubsystem.IsValid())
	{
		MCPSubsystem->ClearRequestLogs();
		UpdateRequestLogList();
		ShowNotification(LOCTEXT("LogsCleared", "Request logs cleared"));
	}
	return FReply::Handled();
}

void SMCPServerControlWidget::OnPortTextChanged(const FText& Text)
{
	// Real-time validation could be added here
}

void SMCPServerControlWidget::OnPortTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (MCPSubsystem.IsValid() && CommitType != ETextCommit::OnCleared)
	{
		int32 Port = FCString::Atoi(*Text.ToString());
		if (IsPortValid(Text.ToString()))
		{
			if (UUnrealMCPSettings* Settings = UUnrealMCPSettings::Get())
			{
				Settings->DefaultPort = Port;
				Settings->SaveSettings();
			}
		}
		else
		{
			// Reset to valid port
			PortEditBox->SetText(FText::FromString(FString::FromInt(UUnrealMCPSettings::Get() ? UUnrealMCPSettings::Get()->DefaultPort : 8080)));
			ShowNotification(LOCTEXT("InvalidPort", "Invalid port number. Port must be between 1024 and 65535."), true);
		}
	}
}

void SMCPServerControlWidget::OnAutoStartCheckChanged(ECheckBoxState NewState)
{
	if (UUnrealMCPSettings* Settings = UUnrealMCPSettings::Get())
	{
		bool bAutoStart = (NewState == ECheckBoxState::Checked);
		Settings->bAutoStartServer = bAutoStart;
		Settings->SaveSettings();

		FText Message = bAutoStart ?
			LOCTEXT("AutoStartEnabled", "Auto-start enabled") :
			LOCTEXT("AutoStartDisabled", "Auto-start disabled");
		ShowNotification(Message);
	}
}

void SMCPServerControlWidget::OnServerStatusChanged(bool bIsRunning)
{
	// UI will be updated by the timer, but we can add immediate feedback here if needed
	UpdateUI();
}

// UI Update Methods

void SMCPServerControlWidget::UpdateUI()
{
	UpdateRequestLogList();
	RefreshRequestStats();
}

void SMCPServerControlWidget::UpdateRequestLogList()
{
	if (MCPSubsystem.IsValid())
	{
		const TArray<FMCPRequestLogEntry>& NewLogs = MCPSubsystem->GetRequestLogs();

		// Clear and rebuild the shared pointer array
		RequestLogEntries.Empty();
		for (const FMCPRequestLogEntry& LogEntry : NewLogs)
		{
			RequestLogEntries.Add(MakeShared<FMCPRequestLogEntry>(LogEntry));
		}

		// Refresh the list view
		if (RequestLogListView.IsValid())
		{
			RequestLogListView->RequestListRefresh();
		}
	}
}

void SMCPServerControlWidget::RefreshRequestStats()
{
	// Stats are updated via binding, no need for explicit refresh
}

// List View Methods

TSharedRef<ITableRow> SMCPServerControlWidget::OnGenerateRequestLogRow(TSharedPtr<FMCPRequestLogEntry> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FMCPRequestLogEntry>>, OwnerTable)
		[
			SNew(SHorizontalBox)

			// Timestamp
			+ SHorizontalBox::Slot()
			.FillWidth(0.25f)
			.Padding(4.0f, 2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Item->Timestamp))
				.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
			]

			// Method
			+ SHorizontalBox::Slot()
			.FillWidth(0.15f)
			.Padding(4.0f, 2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Item->Method))
				.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
			]

			// Path
			+ SHorizontalBox::Slot()
			.FillWidth(0.4f)
			.Padding(4.0f, 2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Item->Path))
				.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
			]

			// Status Code
			+ SHorizontalBox::Slot()
			.FillWidth(0.1f)
			.Padding(4.0f, 2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(Item->StatusCode)))
				.ColorAndOpacity(Item->StatusCode >= 200 && Item->StatusCode < 300 ? FLinearColor::Green : FLinearColor::Red)
				.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
			]

			// Client IP
			+ SHorizontalBox::Slot()
			.FillWidth(0.1f)
			.Padding(4.0f, 2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Item->ClientIP))
				.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
			]
		];
}

// Utility Methods

FText SMCPServerControlWidget::GetStatusText() const
{
	if (MCPSubsystem.IsValid())
	{
		return FText::FromString(MCPSubsystem->GetServerStatus());
	}
	return LOCTEXT("SubsystemNotAvailable", "Subsystem not available");
}

FSlateColor SMCPServerControlWidget::GetStatusColor() const
{
	if (IsServerRunning())
	{
		return FLinearColor::Green;
	}
	return FLinearColor::Red;
}

FText SMCPServerControlWidget::GetPortText() const
{
	if (MCPSubsystem.IsValid())
	{
		return FText::FromString(FString::FromInt(MCPSubsystem->GetServerPort()));
	}
	return FText::FromString(TEXT("N/A"));
}


FText SMCPServerControlWidget::GetRequestStatsText() const
{
	if (MCPSubsystem.IsValid())
	{
		int32 Total, Success, Failed;
		MCPSubsystem->GetRequestStats(Total, Success, Failed);
		return FText::Format(LOCTEXT("RequestStatsFormat", "Total: {0}, Success: {1}, Failed: {2}"),
			FText::FromString(FString::FromInt(Total)), FText::FromString(FString::FromInt(Success)), FText::FromString(FString::FromInt(Failed)));
	}
	return FText::FromString(TEXT("No data"));
}

bool SMCPServerControlWidget::IsServerRunning() const
{
	return MCPSubsystem.IsValid() && MCPSubsystem->IsServerRunning();
}

// Button State Methods


// Validation Methods

bool SMCPServerControlWidget::IsPortValid(const FString& PortString) const
{
	int32 Port = FCString::Atoi(*PortString);
	return Port >= 1024 && Port <= 65535;
}


// Notification Helpers

void SMCPServerControlWidget::ShowNotification(const FText& Message, bool bIsError)
{
	FNotificationInfo Info(Message);
	Info.bFireAndForget = true;
	Info.FadeOutDuration = 3.0f;
	Info.ExpireDuration = 5.0f;

	FSlateNotificationManager::Get().AddNotification(Info);
}

#undef LOCTEXT_NAMESPACE