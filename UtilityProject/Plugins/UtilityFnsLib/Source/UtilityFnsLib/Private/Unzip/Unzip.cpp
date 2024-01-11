// Fill out your copyright notice in the Description page of Project Settings.


#include "Unzip/Unzip.h"
#include "miniz.h"
#include "miniz.c"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include <Async/Async.h>
#include <UObject/GCObjectScopeGuard.h>
#include "HAL/FileManager.h"
#include <Math/UnitConversion.h>

void UUnzip::UnarchiveDirectory(FUnzipAsyncOperationResult Out, FString ArchivePath, FString DirectoryPath)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [Out, ArchivePath, DirectoryPath]()
		{
			UUnzip* Unzip = NewObject<UUnzip>();
			void* MinizArchiver = static_cast<mz_zip_archive*>(FMemory::Memzero(FMemory::Malloc(sizeof(mz_zip_archive)), sizeof(mz_zip_archive)));
			mz_zip_reader_init_file(static_cast<mz_zip_archive*>(MinizArchiver), TCHAR_TO_UTF8(*ArchivePath), 0);
			Unzip->NumOfEntries = mz_zip_reader_get_num_files(static_cast<mz_zip_archive*>(MinizArchiver));
			//bool bResult = false;
			int32 DriveNamePosition = DirectoryPath.Find("\\");
			FString DriveName = DirectoryPath.Left(DriveNamePosition);
			//UE_LOG(LogTemp, Display, TEXT("Drive: %d"), x);
			//UE_LOG(LogTemp, Display, TEXT("Drive name: %s"), *Drivename);
			uint64 TotalDiskSpace = 0;
			uint64 TotalDiskFreeSpace = 0;
			if (FPlatformMisc::GetDiskTotalAndFreeSpace(DriveName, TotalDiskSpace, TotalDiskFreeSpace))
			{
				Unzip->DiskFreeSpace = (double)TotalDiskFreeSpace;
				//UE_LOG(LogTemp, Display, TEXT("Size of Disk: %lf"), Unzip->DiskFreeSpace);
			}
			for (int32 EntryIndex = 0; EntryIndex < Unzip->NumOfEntries; ++EntryIndex)
			{
				size_t EntryInMemorySize;
				void* EntryInMemoryPtr{ mz_zip_reader_extract_to_heap(static_cast<mz_zip_archive*>(MinizArchiver), static_cast<mz_uint>(EntryIndex), &EntryInMemorySize, 0) };
				mz_zip_archive_file_stat ArchiveFileStat;
				const bool bResult{ static_cast<bool>(mz_zip_reader_file_stat(static_cast<mz_zip_archive*>(MinizArchiver), static_cast<mz_uint>(EntryIndex), &ArchiveFileStat)) };
				FString Name = UTF8_TO_TCHAR(ArchiveFileStat.m_filename);

				Unzip->EntryInMemoryTotalSize = Unzip->EntryInMemoryTotalSize + (double)EntryInMemorySize;
				//UE_LOG(LogTemp, Display, TEXT("Size of file: %lf"), EntryInMemoryTotalSize);
			}
			Unzip->UnzipResultObj.UnzipFolderSizeBytes = Unzip->EntryInMemoryTotalSize;
			Unzip->UnzipResultObj.DiskFreeSpaceBytesAfterExtraction = Unzip->DiskFreeSpace - Unzip->EntryInMemoryTotalSize;
			if (Unzip->DiskFreeSpace > Unzip->EntryInMemoryTotalSize)
			{
				for (int32 EntryIndex = 0; EntryIndex < Unzip->NumOfEntries; ++EntryIndex)
				{
					size_t EntryInMemorySize;

					void* EntryInMemoryPtr{ mz_zip_reader_extract_to_heap(static_cast<mz_zip_archive*>(MinizArchiver), static_cast<mz_uint>(EntryIndex), &EntryInMemorySize, 0) };
					mz_zip_archive_file_stat ArchiveFileStat;
					const bool bResult{ static_cast<bool>(mz_zip_reader_file_stat(static_cast<mz_zip_archive*>(MinizArchiver), static_cast<mz_uint>(EntryIndex), &ArchiveFileStat)) };

					FString Name = UTF8_TO_TCHAR(ArchiveFileStat.m_filename);
					//Unzip->UnzipResultObj.UnzipEntriesNames.Add(Name);
					Unzip->UnzipEntriesDataObj.UnzipEntrySize = (double)EntryInMemorySize;
					Unzip->UnzipEntriesDataObj.UnzipEntryName = Name;
					
					FString ZipFolderName;



					const FString Filepath = FPaths::Combine(*DirectoryPath, *Name);

					if (EntryIndex == 0)
					{
						if (FPaths::DirectoryExists(Filepath))
						{
							UE_LOG(LogTemp, Display, TEXT("Name of the folder is: %s"), *Filepath);
							UE_LOG(LogTemp, Display, TEXT("THERE IS A FOLDER ALREADY EXISTING IN THE DESTINATION PATH WITH THE SAME NAME OF THE ZIP FOLDER NAME MENTIONED IN THE 'SourcePath'. DELETE THAT FOLDER AND TRY AGAIN"));
							Unzip->UnzipResultObj.OutputString.Append("THERE IS A FOLDER ALREADY EXISTING IN THE DESTINATION PATH WITH THE SAME NAME OF THE ZIP FOLDER NAME MENTIONED IN THE 'SourcePath'. DELETE THAT FOLDER AND TRY AGAIN");
							break;
							return;
						}
						else
						{
							ZipFolderName = Filepath;
							Unzip->UnzipResultObj.UnzipFolderPath.Append(FPaths::ConvertRelativePathToFull(ZipFolderName));
							//UE_LOG(LogTemp, Display, TEXT("Name of the folder is: %s"), *ZipFolderName);
							Unzip->UnzipResultObj.UnzipFolderRawData.Append(TArray64<uint8>(static_cast<uint8*>(EntryInMemoryPtr), EntryInMemorySize));
							Unzip->UnzipEntriesDataObj.UnzipEntryRawData = TArray64<uint8>(static_cast<uint8*>(EntryInMemoryPtr), EntryInMemorySize);
							Unzip->UnzipResultObj.UnzipEntriesData.Add(Unzip->UnzipEntriesDataObj);
							const bool IsFileSaved = FFileHelper::SaveArrayToFile(TArray64<uint8>(static_cast<uint8*>(EntryInMemoryPtr), EntryInMemorySize), *Filepath);
							Unzip->IsFileSavedArray.Add(IsFileSaved);
							//bResult = true;
						}
					}
					else
					{
						Unzip->UnzipResultObj.UnzipFolderRawData.Append(TArray64<uint8>(static_cast<uint8*>(EntryInMemoryPtr), EntryInMemorySize));
						Unzip->UnzipEntriesDataObj.UnzipEntryRawData = TArray64<uint8>(static_cast<uint8*>(EntryInMemoryPtr), EntryInMemorySize);
						Unzip->UnzipResultObj.UnzipEntriesData.Add(Unzip->UnzipEntriesDataObj);
						const bool IsFileSaved = FFileHelper::SaveArrayToFile(TArray64<uint8>(static_cast<uint8*>(EntryInMemoryPtr), EntryInMemorySize), *Filepath);
						Unzip->IsFileSavedArray.Add(IsFileSaved);
						//bResult = true;
					}
					

				}
			}
			
			else
			{
				UE_LOG(LogTemp, Display, TEXT("YOU DO NOT HAVE ENOUGH SPACE TO EXTRACT AND SAVE THE ZIP FILE IN YOUR STORAGE.\n THE UNCOMPRESSED FOLDER SIZE WOULD BE: %lf\n THE SPACE AVAILABLE IN YOUR DISK is: %lf"), Unzip->EntryInMemoryTotalSize, Unzip->DiskFreeSpace);
				Unzip->UnzipResultObj.OutputString.Append("YOU DO NOT HAVE ENOUGH SPACE TO EXTRACT AND SAVE THE ZIP FILE IN YOUR STORAGE.\n Check Output log for more details");
			}
			

			static_cast<bool>(mz_zip_reader_end(static_cast<mz_zip_archive*>(MinizArchiver)));
			static_cast<bool>(mz_zip_writer_finalize_archive(static_cast<mz_zip_archive*>(MinizArchiver)));
			static_cast<bool>(mz_zip_writer_end(static_cast<mz_zip_archive*>(MinizArchiver)));

			if (MinizArchiver)
			{
				FMemory::Free(MinizArchiver);
				MinizArchiver = nullptr;
			}
			//IFileManager::FileSize(TEXT("h"));
			//IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			//int32 fs = PlatformFile.FileSize(TEXT("C:\\Users\\sai.praveen.rampalli\\Documents\\Unreal Plugins"));
			//IFileManager& _FileManager = IFileManager::Get();
			//UE_LOG(LogTemp, Display, TEXT("Size of the folder is: %d"), fs);
			if (Unzip->IsFileSavedArray.Find(false))
			{
				//bool bResult{ false };
				Unzip->UnzipResultObj.bUnzipResult = false;				
				Unzip->UnzipResultObj.OutputString.Append("FAILED to Extract File");
			}
			else
			{
				
				Unzip->UnzipResultObj.bUnzipResult = true;
				Unzip->UnzipResultObj.OutputString.Append("File Extraction is SUCCESSFUL");
			}
			
			
			//bool bResult{ true };
			AsyncTask(ENamedThreads::GameThread, [Out, Unzip]()
				{
					Out.ExecuteIfBound(Unzip->UnzipResultObj);
				});

		});

}
