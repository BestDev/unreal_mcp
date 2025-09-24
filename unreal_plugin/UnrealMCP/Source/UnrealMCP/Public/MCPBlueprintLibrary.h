// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MCPBlueprintLibrary.generated.h"

/**
 * Blueprint function library for MCP (Model Context Protocol) operations
 * Provides simple functions that can be used in Blueprint graphs
 */
UCLASS()
class UNREALMCP_API UMCPBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Gets information about the current MCP connection status
	 *
	 * @return True if MCP server is running and accessible
	 */
	UFUNCTION(BlueprintPure, Category = "MCP")
	static bool IsMCPServerConnected();

	/**
	 * Gets the current MCP server port
	 *
	 * @return The port number the MCP server is listening on
	 */
	UFUNCTION(BlueprintPure, Category = "MCP")
	static int32 GetMCPServerPort();

	/**
	 * Simple test function to verify Blueprint-C++ connection
	 * Logs a message and returns a test value
	 *
	 * @return Always returns 42 for testing
	 */
	UFUNCTION(BlueprintCallable, Category = "MCP", meta = (Keywords = "test debug"))
	static int32 TestMCPConnection();
};