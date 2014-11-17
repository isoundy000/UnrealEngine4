// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Android_DXTTargetPlatform : ModuleRules
{
	public Android_DXTTargetPlatform( TargetInfo Target )
	{
		BinariesSubFolder = "Android";

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Sockets",
				"TargetPlatform",
				"Messaging",
				"TargetDeviceServices",
				"AndroidDeviceDetection",
			}
		);

		PublicIncludePaths.AddRange(
			new string[]
			{
				"Runtime/Core/Public/Android"
			}
		);

		if (UEBuildConfiguration.bCompileAgainstEngine)
		{
			PrivateDependencyModuleNames.Add("Engine");
			PrivateIncludePathModuleNames.Add("TextureCompressor");		//@todo android: AndroidTargetPlatform.Build
		}

		PrivateIncludePaths.AddRange(
			new string[]
			{
				"Developer/Android/AndroidTargetPlatform/Private",				
			}
		);
	}
}