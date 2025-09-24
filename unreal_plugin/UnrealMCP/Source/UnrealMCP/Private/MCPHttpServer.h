// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestHandler.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"

class IHttpRouter;
struct FHttpServerRequest;

/**
 * HTTP Server for handling MCP (Model Context Protocol) requests
 * Listens on a specified port and processes Blueprint node creation requests
 */
class FMCPHttpServer
{
public:
	FMCPHttpServer();
	~FMCPHttpServer();

	/**
	 * Start the HTTP server on the specified port
	 * @param Port The port to listen on
	 * @return True if server started successfully
	 */
	bool StartServer(uint32 Port);

	/**
	 * Stop the HTTP server
	 */
	void StopServer();

	/**
	 * Check if the server is currently running
	 * @return True if server is running
	 */
	bool IsServerRunning() const;

	/**
	 * Get the port the server is listening on
	 * @return Port number, or 0 if not running
	 */
	uint32 GetServerPort() const;


private:
	/**
	 * Bind HTTP routes to handlers
	 * @return True if routes bound successfully
	 */
	bool BindRoutes();

	/**
	 * Unbind HTTP routes
	 */
	void UnbindRoutes();

	/**
	 * Handle POST /add_node requests
	 * @param Request The HTTP request
	 * @param OnComplete Callback to send response
	 * @return True if request was handled
	 */
	bool HandleAddNode(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	/**
	 * Handle GET /status requests
	 * @param Request The HTTP request
	 * @param OnComplete Callback to send response
	 * @return True if request was handled
	 */
	bool HandleStatus(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	/**
	 * Create a new Blueprint and add a PrintString node to it
	 * @param BlueprintPath The asset path for the new Blueprint (e.g., "/Game/MyBlueprint")
	 * @param NodeType The type of node to add (currently only "PrintString" supported)
	 * @return True if Blueprint was created and node added successfully
	 */
	bool CreateBlueprintWithPrintString(const FString& BlueprintPath, const FString& NodeType);

private:
	/** HTTP Router instance */
	TSharedPtr<IHttpRouter> HttpRouter;

	/** Route handles */
	FHttpRouteHandle AddNodeRouteHandle;
	FHttpRouteHandle StatusRouteHandle;

	/** Server state */
	uint32 ServerPort;
	bool bIsServerRunning;
};