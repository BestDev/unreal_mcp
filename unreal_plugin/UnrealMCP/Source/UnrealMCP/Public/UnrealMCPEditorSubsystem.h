// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Engine/DeveloperSettings.h"
#include "MCPHttpServer.h"
#include "UnrealMCPEditorSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnServerStatusChanged, bool /*bIsRunning*/);

USTRUCT(BlueprintType)
struct FMCPRequestLogEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Timestamp;

	UPROPERTY(BlueprintReadOnly)
	FString Method;

	UPROPERTY(BlueprintReadOnly)
	FString Path;

	UPROPERTY(BlueprintReadOnly)
	int32 StatusCode = 0;

	UPROPERTY(BlueprintReadOnly)
	FString ClientIP;

	FMCPRequestLogEntry()
	{
		Timestamp = FDateTime::Now().ToString();
	}

	FMCPRequestLogEntry(const FString& InMethod, const FString& InPath, int32 InStatusCode, const FString& InClientIP)
		: Method(InMethod)
		, Path(InPath)
		, StatusCode(InStatusCode)
		, ClientIP(InClientIP)
	{
		Timestamp = FDateTime::Now().ToString();
	}
};

/**
 * Editor Subsystem for managing UnrealMCP HTTP Server
 * Provides centralized control and monitoring of the HTTP server
 */
UCLASS()
class UNREALMCP_API UUnrealMCPEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	// UEditorSubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	/** Delegate called when server status changes */
	FOnServerStatusChanged OnServerStatusChanged;

	// HTTP Server Control Functions

	/** Start the HTTP server on the specified port */
	UFUNCTION(BlueprintCallable, Category = "UnrealMCP")
	bool StartServer(int32 Port = 8080);

	/** Stop the HTTP server */
	UFUNCTION(BlueprintCallable, Category = "UnrealMCP")
	void StopServer();

	/** Restart the HTTP server */
	UFUNCTION(BlueprintCallable, Category = "UnrealMCP")
	void RestartServer();

	/** Check if the HTTP server is currently running */
	UFUNCTION(BlueprintPure, Category = "UnrealMCP")
	bool IsServerRunning() const;

	/** Get the current server port */
	UFUNCTION(BlueprintPure, Category = "UnrealMCP")
	int32 GetServerPort() const;

	/** Get human-readable server status */
	UFUNCTION(BlueprintPure, Category = "UnrealMCP")
	FString GetServerStatus() const;


	/** Test connection to the server */
	UFUNCTION(BlueprintCallable, Category = "UnrealMCP")
	bool TestConnection();


	// Log Functions

	/** Add a request log entry */
	void AddRequestLog(const FString& Method, const FString& Path, int32 StatusCode, const FString& ClientIP = TEXT(""));

	/** Get request log entries */
	UFUNCTION(BlueprintPure, Category = "UnrealMCP")
	const TArray<FMCPRequestLogEntry>& GetRequestLogs() const { return RequestLogs; }

	/** Clear request logs */
	UFUNCTION(BlueprintCallable, Category = "UnrealMCP")
	void ClearRequestLogs();

	/** Get request statistics */
	UFUNCTION(BlueprintPure, Category = "UnrealMCP")
	void GetRequestStats(int32& TotalRequests, int32& SuccessfulRequests, int32& FailedRequests) const;

private:
	/** HTTP Server instance */
	TUniquePtr<FMCPHttpServer> HttpServer;

	/** Current server port */
	int32 CurrentPort = 8080;

	/** Server start time */
	FDateTime ServerStartTime;

	/** Request log entries */
	UPROPERTY()
	TArray<FMCPRequestLogEntry> RequestLogs;


	/** Initialize server based on settings */
	void InitializeServer();


	/** Show notification to user */
	void ShowNotification(const FText& Message, bool bIsError = false);
};