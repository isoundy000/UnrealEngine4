// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnrealEd.h"
#include "AutoReimport/AssetSourceFilenameCache.h"
#include "AutoReimportUtilities.h"
#include "AssetRegistryModule.h"

FAssetSourceFilenameCache::FAssetSourceFilenameCache()
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	AssetRegistry.OnAssetAdded().AddRaw(this, &FAssetSourceFilenameCache::HandleOnAssetAdded);
	AssetRegistry.OnAssetRemoved().AddRaw(this, &FAssetSourceFilenameCache::HandleOnAssetRemoved);
	AssetRegistry.OnAssetRenamed().AddRaw(this, &FAssetSourceFilenameCache::HandleOnAssetRenamed);

	UAssetImportData::OnImportDataChanged.AddRaw(this, &FAssetSourceFilenameCache::HandleOnAssetUpdated);

	TArray<FAssetData> Assets;
	if (AssetRegistry.GetAllAssets(Assets))
	{
		for (auto& Asset : Assets)
		{
			HandleOnAssetAdded(Asset);
		}
	}
}

FAssetSourceFilenameCache& FAssetSourceFilenameCache::Get()
{
	static FAssetSourceFilenameCache Cache;
	return Cache;
}

void FAssetSourceFilenameCache::Shutdown()
{
	auto* ModulePtr = FModuleManager::GetModulePtr<FAssetRegistryModule>("AssetRegistry");
	if (ModulePtr)
	{
		ModulePtr->Get().OnAssetAdded().RemoveAll(this);
		ModulePtr->Get().OnAssetRemoved().RemoveAll(this);
		ModulePtr->Get().OnAssetRenamed().RemoveAll(this);
	}
	
	AssetRenamedEvent.Clear();

	UAssetImportData::OnImportDataChanged.RemoveAll(this);
}

TOptional<FAssetImportInfo> FAssetSourceFilenameCache::ExtractAssetImportInfo(const TArray<UObject::FAssetRegistryTag>& InTags)
{
	static const FName LegacySourceFilePathName("SourceFile");

	TOptional<FAssetImportInfo> Info;

	for (const auto& Tag : InTags)
	{
		if (Tag.Name == UObject::SourceFileTagName())
		{
			Info = FAssetImportInfo::FromJson(*Tag.Value);
			// We're done
			break;
		}
		else if (Tag.Name == LegacySourceFilePathName)
		{
			FAssetImportInfo Legacy;
			Legacy.Insert(Tag.Value);
			Info = Legacy;
			// Keep looking for a newer json version
		}
	}

	return Info;
}

TOptional<FAssetImportInfo> FAssetSourceFilenameCache::ExtractAssetImportInfo(const TSharedMapView<FName, FString>& InTags)
{
	static const FName LegacySourceFilePathName("SourceFile");

	if (const FString* ImportDataString = InTags.Find(UObject::SourceFileTagName()))
	{
		return FAssetImportInfo::FromJson(*ImportDataString);
	}
	else if (const FString* LegacyFilename = InTags.Find(LegacySourceFilePathName))
	{
		FAssetImportInfo Legacy;
		Legacy.Insert(*LegacyFilename);
		return Legacy;
	}
	else
	{
		return TOptional<FAssetImportInfo>();
	}
}

void FAssetSourceFilenameCache::HandleOnAssetAdded(const FAssetData& AssetData)
{
	TOptional<FAssetImportInfo> ImportData = ExtractAssetImportInfo(AssetData.TagsAndValues);
	if (ImportData.IsSet())
	{
		for (const auto& SourceFile : ImportData->GetSourceFileData())
		{
			SourceFileToObjectPathCache.FindOrAdd(FPaths::GetCleanFilename(SourceFile.RelativeFilename)).Add(AssetData.ObjectPath);
		}
	}
}

void FAssetSourceFilenameCache::HandleOnAssetRemoved(const FAssetData& AssetData)
{
	TOptional<FAssetImportInfo> ImportData = ExtractAssetImportInfo(AssetData.TagsAndValues);
	if (ImportData.IsSet())
	{
		for (auto& SourceFile : ImportData->GetSourceFileData())
		{
			FString CleanFilename = FPaths::GetCleanFilename(SourceFile.RelativeFilename);
			if (auto* Objects = SourceFileToObjectPathCache.Find(CleanFilename))
			{
				Objects->Remove(AssetData.ObjectPath);
				if (Objects->Num() == 0)
				{
					SourceFileToObjectPathCache.Remove(CleanFilename);
				}
			}
		}
	}
}

void FAssetSourceFilenameCache::HandleOnAssetRenamed(const FAssetData& AssetData, const FString& OldPath)
{
	TOptional<FAssetImportInfo> ImportData = ExtractAssetImportInfo(AssetData.TagsAndValues);
	if (ImportData.IsSet())
	{
		FName OldPathName = *OldPath;

		for (auto& SourceFile : ImportData->GetSourceFileData())
		{
			FString CleanFilename = FPaths::GetCleanFilename(SourceFile.RelativeFilename);

			if (auto* Objects = SourceFileToObjectPathCache.Find(CleanFilename))
			{
				Objects->Remove(OldPathName);
				if (Objects->Num() == 0)
				{
					SourceFileToObjectPathCache.Remove(CleanFilename);
				}
			}

			SourceFileToObjectPathCache.FindOrAdd(CleanFilename).Add(AssetData.ObjectPath);
		}
	}

	AssetRenamedEvent.Broadcast(AssetData, OldPath);
}

void FAssetSourceFilenameCache::HandleOnAssetUpdated(const FAssetImportInfo& OldData, const UAssetImportData* ImportData)
{
	FName ObjectPath = FName(*ImportData->GetOuter()->GetPathName());

	for (const auto& SourceFile : OldData.GetSourceFileData())
	{
		FString CleanFilename = FPaths::GetCleanFilename(SourceFile.RelativeFilename);
		if (auto* Objects = SourceFileToObjectPathCache.Find(CleanFilename))
		{
			Objects->Remove(ObjectPath);
			if (Objects->Num() == 0)
			{
				SourceFileToObjectPathCache.Remove(CleanFilename);
			}
		}
	}

	for (const auto& SourceFile : ImportData->GetSourceFileData())
	{
		SourceFileToObjectPathCache.FindOrAdd(FPaths::GetCleanFilename(SourceFile.RelativeFilename)).Add(ObjectPath);
	}
}

TArray<FAssetData> FAssetSourceFilenameCache::GetAssetsPertainingToFile(const IAssetRegistry& Registry, const FString& AbsoluteFilename) const
{
	TArray<FAssetData> Assets;
	
	if (const auto* ObjectPaths = SourceFileToObjectPathCache.Find(FPaths::GetCleanFilename(AbsoluteFilename)))
	{
		for (const FName& Path : *ObjectPaths)
		{
			FAssetData Asset = Registry.GetAssetByObjectPath(Path);
			TOptional<FAssetImportInfo> ImportInfo = ExtractAssetImportInfo(Asset.TagsAndValues);
			if (ImportInfo.IsSet())
			{
				auto AssetPackagePath = FPackageName::LongPackageNameToFilename(Asset.PackagePath.ToString() / TEXT(""));

				// Attempt to find the matching source filename in the list of imported sorce files (generally there are only one of these)
				const bool bWasImportedFromFile = ImportInfo->GetSourceFileData().ContainsByPredicate([&](const FAssetImportInfo::FSourceFile& File){

					if (AbsoluteFilename == FPaths::ConvertRelativePathToFull(AssetPackagePath / File.RelativeFilename) ||
						AbsoluteFilename == FPaths::ConvertRelativePathToFull(File.RelativeFilename))
					{
						return true;
					}

					return false;
				});

				if (bWasImportedFromFile)
				{
					Assets.Emplace();
					Swap(Asset, Assets[Assets.Num() - 1]);
				}
			}
		}
	}

	return Assets;
}