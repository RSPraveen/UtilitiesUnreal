// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Unzip.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FUnzipEntriesData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "UnzipEntryData")
		FString UnzipEntryName;
	UPROPERTY(BlueprintReadOnly, Category = "UnzipEntryData")
		double UnzipEntrySize;
	UPROPERTY(BlueprintReadOnly, Category = "UnzipEntryData")
		TArray<uint8> UnzipEntryRawData;

	FUnzipEntriesData()
	{
		UnzipEntryName = "";
		UnzipEntrySize = 0.0;
		UnzipEntryRawData = {};
	}
};

USTRUCT(BlueprintType)
struct FUnzipResult
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "UnzipResult")
		bool bUnzipResult;

	UPROPERTY(BlueprintReadOnly, Category = "UnzipResult")
		FString OutputString;

	UPROPERTY(BlueprintReadOnly, Category = "UnzipResult")
		FString UnzipFolderPath;

	UPROPERTY(BlueprintReadOnly, Category = "UnzipResult")
		double UnzipFolderSizeBytes;

	UPROPERTY(BlueprintReadOnly, Category = "UnzipResult")
		double DiskFreeSpaceBytesAfterExtraction;

	UPROPERTY(BlueprintReadOnly, Category = "UnzipResult")
		TArray<uint8> UnzipFolderRawData;

	UPROPERTY(BlueprintReadOnly, Category = "UnzipResult")
		TArray<FUnzipEntriesData> UnzipEntriesData;

	FUnzipResult()
	{
		bUnzipResult = false;
		OutputString = "";
		UnzipFolderPath = "";
		UnzipFolderSizeBytes = 0.0;
		DiskFreeSpaceBytesAfterExtraction = 0.0;
		UnzipFolderRawData = {};
		UnzipEntriesData = {};
	}
};
/** Delegate broadcasting the result of asynchronous archive operations */
DECLARE_DYNAMIC_DELEGATE_OneParam(FUnzipAsyncOperationResult, FUnzipResult, UnzipResult);

UCLASS()
class UTILITYFNSLIB_API UUnzip : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utility_functionslibrary|Unzip", meta = (DisplayName = "UnarchiveDirectory", Keywords = "async"))
	static void UnarchiveDirectory(FUnzipAsyncOperationResult Out, FString ArchivePath, FString DirectoryPath);

private:

	UPROPERTY()
	int32 NumOfEntries = 0;

	UPROPERTY()
	TArray<bool> IsFileSavedArray = {};

	

	UPROPERTY()
	double EntryInMemoryTotalSize = 0.0;

	UPROPERTY()
	double DiskFreeSpace = 0.0;

	UPROPERTY()
		FUnzipResult UnzipResultObj;

	UPROPERTY()
		FUnzipEntriesData UnzipEntriesDataObj;

	
};
