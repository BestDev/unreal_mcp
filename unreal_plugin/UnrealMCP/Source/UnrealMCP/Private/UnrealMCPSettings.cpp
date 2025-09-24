// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealMCPSettings.h"
#include "IUnrealMCP.h"
#include "Logging/LogVerbosity.h"

#define LOCTEXT_NAMESPACE "UnrealMCPSettings"

UUnrealMCPSettings::UUnrealMCPSettings()
{
	// Set default category name for settings
	CategoryName = TEXT("Plugins");
	SectionName = TEXT("UnrealMCP");
}

FName UUnrealMCPSettings::GetCategoryName() const
{
	return FName(TEXT("Plugins"));
}

FText UUnrealMCPSettings::GetSectionText() const
{
	return LOCTEXT("UnrealMCPSectionText", "UnrealMCP");
}

UUnrealMCPSettings* UUnrealMCPSettings::Get()
{
	return GetMutableDefault<UUnrealMCPSettings>();
}

void UUnrealMCPSettings::SaveSettings()
{
	SaveConfig();
	UE_LOG(LogUnrealMCP, Log, TEXT("UnrealMCP settings saved"));
}

void UUnrealMCPSettings::LoadSettings()
{
	LoadConfig();
	UE_LOG(LogUnrealMCP, Log, TEXT("UnrealMCP settings loaded"));
}

ELogVerbosity::Type UUnrealMCPSettings::GetLogVerbosity() const
{
	return static_cast<ELogVerbosity::Type>(FMath::Clamp(LogLevel, 0, 7));
}

#undef LOCTEXT_NAMESPACE