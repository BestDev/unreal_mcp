// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealMCPEditorSubsystem.h"
#include "UnrealMCPSettings.h"
#include "MCPHttpServer.h"
#include "Engine/Engine.h"
#include "IUnrealMCP.h"
#include "Editor.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

DEFINE_LOG_CATEGORY_STATIC(LogUnrealMCPEditor, Log, All);

void UUnrealMCPEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogUnrealMCPEditor, Log, TEXT("UnrealMCP Editor Subsystem initialized"));

	// Initialize server based on settings
	InitializeServer();
}

void UUnrealMCPEditorSubsystem::Deinitialize()
{
	// Stop server if running
	if (IsServerRunning())
	{
		StopServer();
	}

	UE_LOG(LogUnrealMCPEditor, Log, TEXT("UnrealMCP Editor Subsystem deinitialized"));

	Super::Deinitialize();
}

bool UUnrealMCPEditorSubsystem::StartServer(int32 Port)
{
	UE_LOG(LogUnrealMCP, Log, TEXT("StartServer called with port: %d"), Port);

	// ✅ 확인됨: FMCPHttpServer 클래스 구조 확인 완료
	if (IsServerRunning())
	{
		UE_LOG(LogUnrealMCPEditor, Log, TEXT("HTTP Server is already running on port %d"), CurrentPort);
		return false;
	}

	// Create HTTP server instance if not exists
	if (!HttpServer.IsValid())
	{
		HttpServer = MakeUnique<FMCPHttpServer>();
	}

	// ✅ 확인됨: FMCPHttpServer::StartServer 시그니처 uint32 매개변수
	bool bServerStarted = HttpServer->StartServer(static_cast<uint32>(Port));

	if (bServerStarted)
	{
		CurrentPort = Port;
		ServerStartTime = FDateTime::Now();

		UE_LOG(LogUnrealMCPEditor, Log, TEXT("HTTP Server started successfully on port %d"), Port);

		// Broadcast status change
		OnServerStatusChanged.Broadcast(true);

		// Show notification
		ShowNotification(FText::FromString(FString::Printf(TEXT("MCP Server started on port %d"), Port)), false);
	}
	else
	{
		UE_LOG(LogUnrealMCPEditor, Error, TEXT("Failed to start HTTP Server on port %d"), Port);

		// Show error notification
		ShowNotification(FText::FromString(FString::Printf(TEXT("Failed to start MCP Server on port %d"), Port)), true);
	}

	return bServerStarted;
}

void UUnrealMCPEditorSubsystem::StopServer()
{
	if (!IsServerRunning())
	{
		UE_LOG(LogUnrealMCPEditor, Log, TEXT("HTTP Server is not running"));
		return;
	}

	// ✅ 확인됨: FMCPHttpServer::StopServer 메서드 확인
	if (HttpServer.IsValid())
	{
		HttpServer->StopServer();
	}

	UE_LOG(LogUnrealMCPEditor, Log, TEXT("HTTP Server stopped"));

	// Broadcast status change
	OnServerStatusChanged.Broadcast(false);

	// Show notification
	ShowNotification(FText::FromString(TEXT("MCP Server stopped")), false);
}

void UUnrealMCPEditorSubsystem::RestartServer()
{
	UE_LOG(LogUnrealMCPEditor, Log, TEXT("Restarting HTTP Server..."));

	int32 PortToRestart = CurrentPort;

	// Stop current server
	if (IsServerRunning())
	{
		StopServer();
	}

	// Start server on the same port
	StartServer(PortToRestart);
}

bool UUnrealMCPEditorSubsystem::IsServerRunning() const
{
	// ✅ 확인됨: FMCPHttpServer::IsServerRunning 메서드 확인
	return HttpServer.IsValid() && HttpServer->IsServerRunning();
}

int32 UUnrealMCPEditorSubsystem::GetServerPort() const
{
	if (IsServerRunning())
	{
		// ✅ 확인됨: FMCPHttpServer::GetServerPort 반환 타입 uint32
		return static_cast<int32>(HttpServer->GetServerPort());
	}
	return 0;
}

FString UUnrealMCPEditorSubsystem::GetServerStatus() const
{
	if (IsServerRunning())
	{
		return FString::Printf(TEXT("Running on port %d"), GetServerPort());
	}
	else
	{
		return TEXT("Stopped");
	}
}


bool UUnrealMCPEditorSubsystem::TestConnection()
{
	if (!IsServerRunning())
	{
		UE_LOG(LogUnrealMCPEditor, Log, TEXT("Cannot test connection: Server is not running"));
		return false;
	}

	// ❓ 불확실: HTTP 연결 테스트 구현은 향후 확장 가능
	// 현재는 서버 실행 상태만 확인
	UE_LOG(LogUnrealMCPEditor, Log, TEXT("Connection test: Server is running on port %d"), GetServerPort());
	return true;
}


void UUnrealMCPEditorSubsystem::AddRequestLog(const FString& Method, const FString& Path, int32 StatusCode, const FString& ClientIP)
{
	// ✅ 확인됨: FMCPRequestLogEntry 생성자 시그니처
	FMCPRequestLogEntry NewEntry(Method, Path, StatusCode, ClientIP);

	// Add to log array
	RequestLogs.Add(NewEntry);

	// Update statistics

	// Limit log size based on settings
	if (const UUnrealMCPSettings* Settings = UUnrealMCPSettings::Get())
	{
		while (RequestLogs.Num() > Settings->MaxLogEntries)
		{
			RequestLogs.RemoveAt(0);
		}
	}

	UE_LOG(LogUnrealMCPEditor, VeryVerbose, TEXT("Request logged: %s %s [%d] from %s"),
		*Method, *Path, StatusCode, *ClientIP);
}

void UUnrealMCPEditorSubsystem::ClearRequestLogs()
{
	RequestLogs.Empty();


	UE_LOG(LogUnrealMCPEditor, Log, TEXT("Request logs cleared"));
}

void UUnrealMCPEditorSubsystem::GetRequestStats(int32& TotalRequests, int32& SuccessfulRequests, int32& FailedRequests) const
{
	TotalRequests = RequestLogs.Num();
	SuccessfulRequests = 0;
	FailedRequests = 0;

	for (const FMCPRequestLogEntry& Entry : RequestLogs)
	{
		if (Entry.StatusCode >= 200 && Entry.StatusCode < 300)
		{
			SuccessfulRequests++;
		}
		else
		{
			FailedRequests++;
		}
	}
}

void UUnrealMCPEditorSubsystem::InitializeServer()
{
	// ✅ 확인됨: UUnrealMCPSettings 설정 시스템 구조
	const UUnrealMCPSettings* Settings = UUnrealMCPSettings::Get();
	if (!Settings)
	{
		UE_LOG(LogUnrealMCPEditor, Log, TEXT("Failed to get UnrealMCP settings, using defaults"));
		return;
	}

	// Auto-start removed - server now starts manually via UI control
}


void UUnrealMCPEditorSubsystem::ShowNotification(const FText& Message, bool bIsError)
{
	// ✅ 확인됨: UE5.6 알림 시스템 패턴 (UUnrealEditorSubsystem 참조)
	if (!GEditor)
	{
		return;
	}

	FNotificationInfo Info(Message);
	Info.bFireAndForget = true;
	Info.FadeOutDuration = 3.0f;
	Info.ExpireDuration = 5.0f;

	// Icon removed to avoid UE 5.6 style warnings - text-only notifications

	FSlateNotificationManager::Get().AddNotification(Info);
}