// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	MeshMaterialShader.h: Shader base classes
=============================================================================*/

#pragma once

#include "MaterialShader.h"
#include "MeshMaterialShaderType.h"

/** Base class of all shaders that need material and vertex factory parameters. */
class RENDERER_API FMeshMaterialShader : public FMaterialShader
{
public:
	FMeshMaterialShader() {}

	FMeshMaterialShader(const FMeshMaterialShaderType::CompiledShaderInitializerType& Initializer)
		:	FMaterialShader(Initializer)
		,	VertexFactoryParameters(Initializer.VertexFactoryType,Initializer.ParameterMap,(EShaderFrequency)Initializer.Target.Frequency)
	{
		NonInstancedDitherLODFactorParameter.Bind(Initializer.ParameterMap, TEXT("NonInstancedDitherLODFactor"));
	}

	template< typename ShaderRHIParamRef >
	void SetParameters(
		FRHICommandList& RHICmdList,
		const ShaderRHIParamRef ShaderRHI,
		const FMaterialRenderProxy* MaterialRenderProxy,
		const FMaterial& Material,
		const FSceneView& View,
		ESceneRenderTargetsMode::Type TextureMode )
	{
		FMaterialShader::SetParameters(RHICmdList, ShaderRHI,MaterialRenderProxy,Material,View,false,TextureMode);
	}

	void SetVFParametersOnly(FRHICommandList& RHICmdList, const FVertexFactory* VertexFactory,const FSceneView& View,const FMeshBatchElement& BatchElement)
	{
		VertexFactoryParameters.SetMesh(RHICmdList, this,VertexFactory,View,BatchElement, 0);
	}

	template< typename ShaderRHIParamRef >
	void SetMesh(
		FRHICommandList& RHICmdList,
		const ShaderRHIParamRef ShaderRHI,
		const FVertexFactory* VertexFactory,
		const FSceneView& View,
		const FPrimitiveSceneProxy* Proxy,
		const FMeshBatchElement& BatchElement,
		float DitheredLODTransitionValue,
		uint32 DataFlags = 0
	);

	/**
	 * Retrieves the fade uniform buffer parameter from a FSceneViewState for the primitive
	 * This code was moved from SetMesh() to work around the template first-use vs first-seen differences between MSVC and others
	 */
	FUniformBufferRHIParamRef GetPrimitiveFadeUniformBufferParameter(const FSceneView& View, const FPrimitiveSceneProxy* Proxy);

	// FShader interface.
	virtual const FVertexFactoryParameterRef* GetVertexFactoryParameterRef() const override { return &VertexFactoryParameters; }
	virtual bool Serialize(FArchive& Ar) override;
	virtual uint32 GetAllocatedSize() const override;

private:
	FVertexFactoryParameterRef VertexFactoryParameters;
	FShaderParameter NonInstancedDitherLODFactorParameter;
};
