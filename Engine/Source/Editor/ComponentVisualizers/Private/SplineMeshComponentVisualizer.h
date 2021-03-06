// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ComponentVisualizer.h"
#include "Components/SplineMeshComponent.h"

/** Base class for clickable spline mesh component editing proxies */
struct HSplineMeshVisProxy : public HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HSplineMeshVisProxy(const UActorComponent* InComponent)
	: HComponentVisProxy(InComponent, HPP_Wireframe)
	{}
};

/** Proxy for a spline mesh component key */
struct HSplineMeshKeyProxy : public HSplineMeshVisProxy
{
	DECLARE_HIT_PROXY();

	HSplineMeshKeyProxy(const UActorComponent* InComponent, int32 InKeyIndex) 
		: HSplineMeshVisProxy(InComponent)
		, KeyIndex(InKeyIndex)
	{}

	int32 KeyIndex;
};

/** Proxy for a tangent handle */
struct HSplineMeshTangentHandleProxy : public HSplineMeshVisProxy
{
	DECLARE_HIT_PROXY();

	HSplineMeshTangentHandleProxy(const UActorComponent* InComponent, int32 InKeyIndex, bool bInArriveTangent)
		: HSplineMeshVisProxy(InComponent)
		, KeyIndex(InKeyIndex)
		, bArriveTangent(bInArriveTangent)
	{}

	int32 KeyIndex;
	bool bArriveTangent;
};

/** SplineMeshComponent visualizer/edit functionality */
class FSplineMeshComponentVisualizer : public FComponentVisualizer
{
public:
	FSplineMeshComponentVisualizer();
	virtual ~FSplineMeshComponentVisualizer();

	// Begin FComponentVisualizer interface
	virtual void OnRegister() override;
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual bool VisProxyHandleClick(FLevelEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	virtual void EndEditing() override;
	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
	virtual bool GetCustomInputCoordinateSystem(const FEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const override;
	virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override;
	virtual bool HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual TSharedPtr<SWidget> GenerateContextMenu() const override;
	// End FComponentVisualizer interface

	/** Get the spline component we are currently editing */
	USplineMeshComponent* GetEditedSplineMeshComponent() const;

private:

	/** Get a spline object for the specified spline mesh component */
	FInterpCurveVector GetSpline(const USplineMeshComponent* SplineMeshComp) const;

	/** Syncs changes made by the visualizer in the actual component */
	void NotifyComponentModified();

	/** Actor that owns the currently edited spline */
	TWeakObjectPtr<AActor> SplineMeshOwningActor;

	/** Name of property on the actor that references the spline we are editing */
	FName SplineMeshCompPropName;

	/** Index of the key we selected */
	int32 SelectedKey;

	/** Index of tangent handle we have selected */
	int32 SelectedTangentHandle;

	struct ESelectedTangentHandle
	{
		enum Type
		{
			None,
			Leave,
			Arrive
		};
	};

	/** The type of the selected tangent handle */
	ESelectedTangentHandle::Type SelectedTangentHandleType;
};
