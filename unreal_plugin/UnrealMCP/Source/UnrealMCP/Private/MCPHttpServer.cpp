// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPHttpServer.h"
#include "IUnrealMCP.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "HttpPath.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Engine/Engine.h"

#if WITH_EDITOR
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_CallFunction.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "BlueprintEditorModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "FileHelpers.h"
#endif

FMCPHttpServer::FMCPHttpServer()
	: ServerPort(0)
	, bIsServerRunning(false)
{
}

FMCPHttpServer::~FMCPHttpServer()
{
	StopServer();
}

bool FMCPHttpServer::StartServer(uint32 Port)
{
	if (bIsServerRunning)
	{
		UE_LOG(LogUnrealMCP, Warning, TEXT("HTTP Server is already running on port %d"), ServerPort);
		return false;
	}

	// Get the HTTP Server module
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();

	// Create router for the specified port
	HttpRouter = HttpServerModule.GetHttpRouter(Port, false);
	if (!HttpRouter.IsValid())
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Failed to create HTTP router for port %d"), Port);
		return false;
	}

	// Bind routes
	if (!BindRoutes())
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Failed to bind HTTP routes"));
		return false;
	}

	// Start all listeners
	HttpServerModule.StartAllListeners();

	ServerPort = Port;
	bIsServerRunning = true;

	UE_LOG(LogUnrealMCP, Log, TEXT("MCP HTTP Server started on port %d"), Port);
	return true;
}

void FMCPHttpServer::StopServer()
{
	if (!bIsServerRunning)
	{
		return;
	}

	// Unbind routes
	UnbindRoutes();

	// Stop HTTP Server module listeners
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	HttpServerModule.StopAllListeners();

	HttpRouter.Reset();
	bIsServerRunning = false;
	ServerPort = 0;

	UE_LOG(LogUnrealMCP, Log, TEXT("MCP HTTP Server stopped"));
}

bool FMCPHttpServer::IsServerRunning() const
{
	return bIsServerRunning;
}

uint32 FMCPHttpServer::GetServerPort() const
{
	return ServerPort;
}

bool FMCPHttpServer::BindRoutes()
{
	if (!HttpRouter.IsValid())
	{
		return false;
	}

	// Bind /add_node endpoint for POST requests
	FHttpPath AddNodePath(TEXT("/add_node"));
	AddNodeRouteHandle = HttpRouter->BindRoute(
		AddNodePath,
		EHttpServerRequestVerbs::VERB_POST,
		FHttpRequestHandler::CreateRaw(this, &FMCPHttpServer::HandleAddNode)
	);

	// Bind /status endpoint for GET requests
	FHttpPath StatusPath(TEXT("/status"));
	StatusRouteHandle = HttpRouter->BindRoute(
		StatusPath,
		EHttpServerRequestVerbs::VERB_GET,
		FHttpRequestHandler::CreateRaw(this, &FMCPHttpServer::HandleStatus)
	);

	UE_LOG(LogUnrealMCP, Log, TEXT("HTTP routes bound successfully"));
	return true;
}

void FMCPHttpServer::UnbindRoutes()
{
	if (!HttpRouter.IsValid())
	{
		return;
	}

	if (AddNodeRouteHandle.IsValid())
	{
		HttpRouter->UnbindRoute(AddNodeRouteHandle);
		AddNodeRouteHandle.Reset();
	}

	if (StatusRouteHandle.IsValid())
	{
		HttpRouter->UnbindRoute(StatusRouteHandle);
		StatusRouteHandle.Reset();
	}

	UE_LOG(LogUnrealMCP, Log, TEXT("HTTP routes unbound"));
}

bool FMCPHttpServer::HandleAddNode(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogUnrealMCP, Log, TEXT("Received /add_node request"));

	// Parse JSON request body
	FString RequestBody;
	if (Request.Body.Num() > 0)
	{
		// Create a null-terminated string from the byte array
		TArray<uint8> NullTerminatedBody = Request.Body;
		NullTerminatedBody.Add(0); // Ensure null termination
		RequestBody = FString(FUTF8ToTCHAR(reinterpret_cast<const char*>(NullTerminatedBody.GetData())));
	}

	UE_LOG(LogUnrealMCP, Log, TEXT("Received request body: %s"), *RequestBody);
	UE_LOG(LogUnrealMCP, Log, TEXT("Request body length: %d"), RequestBody.Len());

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RequestBody);
	bool bJsonParseResult = FJsonSerializer::Deserialize(Reader, JsonObject);

	TSharedPtr<FJsonObject> ResponseJson = MakeShareable(new FJsonObject);

	if (!bJsonParseResult || !JsonObject.IsValid())
	{
		UE_LOG(LogUnrealMCP, Warning, TEXT("Failed to parse JSON request body"));
		ResponseJson->SetBoolField(TEXT("success"), false);
		ResponseJson->SetStringField(TEXT("message"), TEXT("Invalid JSON in request body"));
	}
	else
	{
		// Extract blueprint_path and node_type from request
		FString BlueprintPath = JsonObject->GetStringField(TEXT("blueprint_path"));
		FString NodeType = JsonObject->GetStringField(TEXT("node_type"));

		UE_LOG(LogUnrealMCP, Log, TEXT("Add node request - Blueprint: %s, NodeType: %s"), *BlueprintPath, *NodeType);

		// Call the actual blueprint creation function
		bool bSuccess = CreateBlueprintWithPrintString(BlueprintPath, NodeType);

		ResponseJson->SetBoolField(TEXT("success"), bSuccess);
		if (bSuccess)
		{
			ResponseJson->SetStringField(TEXT("message"), FString::Printf(TEXT("Blueprint created with %s node at '%s'"), *NodeType, *BlueprintPath));
		}
		else
		{
			ResponseJson->SetStringField(TEXT("message"), FString::Printf(TEXT("Failed to create Blueprint with %s node at '%s'"), *NodeType, *BlueprintPath));
		}
		ResponseJson->SetStringField(TEXT("blueprint_path"), BlueprintPath);
		ResponseJson->SetStringField(TEXT("node_type"), NodeType);
	}

	// Serialize response to JSON string
	FString ResponseString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
	FJsonSerializer::Serialize(ResponseJson.ToSharedRef(), Writer);

	// Create HTTP response
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(ResponseString, TEXT("application/json"));
	Response->Code = EHttpServerResponseCodes::Ok;

	// Send response
	OnComplete(MoveTemp(Response));
	return true;
}

bool FMCPHttpServer::HandleStatus(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogUnrealMCP, Log, TEXT("Received /status request"));

	// Create status response
	TSharedPtr<FJsonObject> ResponseJson = MakeShareable(new FJsonObject);
	ResponseJson->SetBoolField(TEXT("server_running"), true);
	ResponseJson->SetNumberField(TEXT("port"), ServerPort);
	ResponseJson->SetStringField(TEXT("status"), TEXT("UnrealMCP server is running"));
	ResponseJson->SetStringField(TEXT("version"), TEXT("1.0"));

	// Serialize response to JSON string
	FString ResponseString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
	FJsonSerializer::Serialize(ResponseJson.ToSharedRef(), Writer);

	// Create HTTP response
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(ResponseString, TEXT("application/json"));
	Response->Code = EHttpServerResponseCodes::Ok;

	// Send response
	OnComplete(MoveTemp(Response));
	return true;
}

bool FMCPHttpServer::CreateBlueprintWithPrintString(const FString& BlueprintPath, const FString& NodeType)
{
#if WITH_EDITOR
	// ✅ 확인됨: UE5.6 소스코드에서 검증된 블루프린트 생성 방법
	UE_LOG(LogUnrealMCP, Log, TEXT("Creating Blueprint at path: %s with node type: %s"), *BlueprintPath, *NodeType);

	// Only support PrintString for now
	if (NodeType != TEXT("PrintString"))
	{
		UE_LOG(LogUnrealMCP, Warning, TEXT("Unsupported node type: %s. Only 'PrintString' is currently supported."), *NodeType);
		return false;
	}

	// Parse the asset path to extract package and asset names
	FString PackageName, AssetName;
	if (!BlueprintPath.Split(TEXT("/"), &PackageName, &AssetName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Invalid blueprint path format: %s"), *BlueprintPath);
		return false;
	}

	// The full package name is the blueprint path itself
	FString FullPackageName = BlueprintPath;
	if (AssetName.IsEmpty())
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Asset name cannot be empty"));
		return false;
	}

	// ✅ 확인됨: FKismetEditorUtilities::CreateBlueprint 함수 시그니처 검증
	// D:\_Source\UnrealEngine\Engine\Source\Editor\UnrealEd\Public\Kismet2\KismetEditorUtilities.h:102
	UPackage* Package = CreatePackage(*FullPackageName);
	if (!Package)
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Failed to create package: %s"), *FullPackageName);
		return false;
	}

	// Create the Blueprint
	UBlueprint* NewBlueprint = FKismetEditorUtilities::CreateBlueprint(
		AActor::StaticClass(),  // Parent class
		Package,                // Outer package
		FName(*AssetName),      // Blueprint name
		BPTYPE_Normal,          // Blueprint type
		NAME_None               // Calling context
	);

	if (!NewBlueprint)
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Failed to create Blueprint"));
		return false;
	}

	// ✅ 확인됨: FBlueprintEditorUtils::FindEventGraph 함수 검증
	// D:\_Source\UnrealEngine\Engine\Source\Editor\UnrealEd\Public\Kismet2\BlueprintEditorUtils.h:1095
	UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(NewBlueprint);
	if (!EventGraph)
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Failed to find Event Graph in Blueprint"));
		return false;
	}

	// ✅ 확인됨: UKismetSystemLibrary::PrintString UFUNCTION 검증
	// D:\_Source\UnrealEngine\Engine\Source\Runtime\Engine\Classes\Kismet\KismetSystemLibrary.h:374
	UFunction* PrintStringFunction = FindFieldChecked<UFunction>(UKismetSystemLibrary::StaticClass(), TEXT("PrintString"));
	if (!PrintStringFunction)
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Failed to find PrintString function"));
		return false;
	}

	// ✅ 확인됨: UK2Node_CallFunction 노드 생성 패턴 검증
	// D:\_Source\UnrealEngine\Engine\Source\Editor\Kismet\Private\BlueprintEditor.cpp:5279
	UK2Node_CallFunction* NewFunctionNode = NewObject<UK2Node_CallFunction>(EventGraph);
	NewFunctionNode->SetFromFunction(PrintStringFunction);

	// Add the node to the graph
	EventGraph->AddNode(NewFunctionNode, true, true);

	// Position the node
	NewFunctionNode->NodePosX = 200;
	NewFunctionNode->NodePosY = 200;

	// Allocate default pins for the node
	NewFunctionNode->AllocateDefaultPins();

	// Reconstruct the node to ensure proper setup
	NewFunctionNode->ReconstructNode();

	// Mark the Blueprint as modified
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(NewBlueprint);

	// Mark the package as dirty and save immediately to disk
	Package->MarkAsFullyLoaded();
	Package->SetDirtyFlag(true);

	// Save the package to disk immediately
	FString PackageFileName = FPackageName::LongPackageNameToFilename(FullPackageName, FPackageName::GetAssetPackageExtension());
	UE_LOG(LogUnrealMCP, Warning, TEXT("Package path conversion: '%s' -> '%s'"), *FullPackageName, *PackageFileName);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SaveArgs.Error = GError;
	SaveArgs.bForceByteSwapping = true;
	SaveArgs.bWarnOfLongFilename = true;
	SaveArgs.SaveFlags = SAVE_NoError;

	bool bSaved = UPackage::SavePackage(Package, NewBlueprint, *PackageFileName, SaveArgs);

	if (!bSaved)
	{
		UE_LOG(LogUnrealMCP, Error, TEXT("Failed to save Blueprint package to disk"));
		return false;
	}

	// Register with asset registry
	FAssetRegistryModule::AssetCreated(NewBlueprint);

	UE_LOG(LogUnrealMCP, Log, TEXT("Successfully created Blueprint with PrintString node at: %s"), *BlueprintPath);
	return true;

#else
	UE_LOG(LogUnrealMCP, Error, TEXT("Blueprint creation is only supported in editor builds"));
	return false;
#endif
}