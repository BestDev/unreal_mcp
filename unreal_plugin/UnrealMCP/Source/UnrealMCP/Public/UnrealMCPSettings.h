// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/DeveloperSettings.h"
#include "UnrealMCPSettings.generated.h"


/**
 * Settings for UnrealMCP Plugin
 * Stores configuration for HTTP server behavior and UI preferences
 */
UCLASS(Config=UnrealMCP, DefaultConfig, meta=(DisplayName="UnrealMCP Settings"))
class UNREALMCP_API UUnrealMCPSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UUnrealMCPSettings();

	// UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	virtual FText GetSectionText() const override;

public:
	/** Whether to automatically start the HTTP server when the editor starts */
	UPROPERTY(Config, EditAnywhere, Category = "Server", meta = (DisplayName = "Auto Start Server"))
	bool bAutoStartServer = false;

	/** Default port for the HTTP server */
	UPROPERTY(Config, EditAnywhere, Category = "Server", meta = (DisplayName = "Default Port", ClampMin = 1024, ClampMax = 65535))
	int32 DefaultPort = 8080;

	/** Log level for UnrealMCP operations (0=NoLogging, 1=Fatal, 2=Error, 3=Warning, 4=Display, 5=Log, 6=Verbose, 7=VeryVerbose) */
	UPROPERTY(Config, EditAnywhere, Category = "Logging", meta = (DisplayName = "Log Level", ClampMin = 0, ClampMax = 7))
	uint8 LogLevel = 3;

	/** Whether to show detailed request logs in the UI */
	UPROPERTY(Config, EditAnywhere, Category = "UI", meta = (DisplayName = "Show Request Logs"))
	bool bShowRequestLogs = true;

	/** Maximum number of request log entries to keep */
	UPROPERTY(Config, EditAnywhere, Category = "UI", meta = (DisplayName = "Max Log Entries", ClampMin = 10, ClampMax = 1000))
	int32 MaxLogEntries = 100;

public:
	/** Get the singleton instance of settings */
	static UUnrealMCPSettings* Get();

	/** Save settings to config file */
	void SaveSettings();

	/** Load settings from config file */
	void LoadSettings();

	/** Get engine log verbosity type */
	ELogVerbosity::Type GetLogVerbosity() const;
};