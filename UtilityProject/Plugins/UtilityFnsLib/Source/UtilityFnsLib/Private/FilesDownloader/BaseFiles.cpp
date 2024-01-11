// Fill out your copyright notice in the Description page of Project Settings.


#include "FilesDownloader/BaseFiles.h"
#include "Containers/UnrealString.h"
#include "ImageUtils.h"
#include "FilesDownloader/RuntimeChunk.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UBaseFiles::UBaseFiles()
{
	FWorldDelegates::OnWorldCleanup.AddWeakLambda(this, [this](UWorld* World, bool bSessionEnded, bool bCleanupResources)
		{
			if (bSessionEnded)
			{
				CancelDownload();
			}
		});
}

bool UBaseFiles::CancelDownload()
{
	return true;
}

void UBaseFiles::GetContentSize(const FString& URL, float Timeout, const FOnGetDownloadContentLength& OnComplete)
{
	GetContentSize(URL, Timeout, FOnGetDownloadContentLengthNative::CreateLambda([OnComplete](int64 ContentSize)
		{
			OnComplete.ExecuteIfBound(ContentSize);
		}));
}

void UBaseFiles::GetContentSize(const FString& URL, float Timeout, const FOnGetDownloadContentLengthNative& OnComplete)
{
	UBaseFiles* File = NewObject<UBaseFiles>();
	File->AddToRoot();
	File->RuntimeChunkPtr = MakeShared<FRuntimeChunk>();
	File->RuntimeChunkPtr->GetContentSize(URL, Timeout).Next([File, OnComplete](int64 ContentSize)
		{
			if (File)
			{
				File->RemoveFromRoot();
			}
			OnComplete.ExecuteIfBound(ContentSize);
		});
}

FString UBaseFiles::BytesToString(const TArray<uint8>& Bytes)
{
	const uint8* BytesData = Bytes.GetData();
	FString Result;
	for (int32 Count = Bytes.Num(); Count > 0; --Count)
	{
		Result += static_cast<TCHAR>(*BytesData);

		++BytesData;
	}
	return Result;
}

UTexture2D* UBaseFiles::BytesToTexture(const TArray<uint8>& Bytes)
{
	return FImageUtils::ImportBufferAsTexture2D(Bytes);
}

bool UBaseFiles::LoadFileToArray(const FString& Filename, TArray<uint8>& Result)
{
	return FFileHelper::LoadFileToArray(Result, *Filename);
}

bool UBaseFiles::SaveArrayToFile(const TArray<uint8>& Bytes, const FString& Filename)
{
	return FFileHelper::SaveArrayToFile(Bytes, *Filename);
}

bool UBaseFiles::LoadFileToString(FString& Result, const FString& Filename)
{
	return FFileHelper::LoadFileToString(Result, *Filename);
}

bool UBaseFiles::SaveStringToFile(const FString& String, const FString& Filename)
{
	return FFileHelper::SaveStringToFile(String, *Filename);
}

bool UBaseFiles::IsFileExist(const FString& FilePath)
{
	return FPaths::FileExists(FilePath);
}

void UBaseFiles::BroadcastProgress(int64 BytesReceived, int64 ContentLength, float ProgressRatio) const
{
	if (OnDownloadProgress.IsBound())
	{
		OnDownloadProgress.Execute(BytesReceived, ContentLength, ProgressRatio);
	}
}
