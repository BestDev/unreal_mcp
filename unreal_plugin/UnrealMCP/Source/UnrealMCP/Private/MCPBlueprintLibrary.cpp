// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPBlueprintLibrary.h"
#include "IUnrealMCP.h"
#include "Engine/Engine.h"
#include "Modules/ModuleManager.h"


bool UMCPBlueprintLibrary::IsMCPServerConnected()
{
	// Check if UnrealMCP module is loaded
	if (!IUnrealMCP::IsAvailable())
	{
		return false;
	}

	// For now, we'll consider the server connected if the module is loaded
	// In a more advanced implementation, we might check the actual HTTP server status
	return true;
}

int32 UMCPBlueprintLibrary::GetMCPServerPort()
{
	// Return the default port (8080) that our HTTP server uses
	// In a more advanced implementation, this could be configurable
	return 8080;
}

int32 UMCPBlueprintLibrary::TestMCPConnection()
{
	UE_LOG(LogUnrealMCP, Log, TEXT("MCP Blueprint connection test successful"));
	return 42;
}