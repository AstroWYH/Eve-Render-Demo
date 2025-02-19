// wyh

#pragma once

#include "CoreMinimal.h"

class SEveWidget;

class EveRenderer : public TSharedFromThis<EveRenderer, ESPMode::ThreadSafe>, public ICustomSlateElement
{
public:
	virtual ~EveRenderer()
	{
	};

	virtual void DrawRenderThread(class FRHICommandListImmediate& RHICmdList, const void* RenderTarget) override;
	
	FMatrix CalcBaseTransform2D(uint32 ViewSizeX, uint32 ViewSizeY);

	int32 Draw(const SEveWidget* Widget, const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled);

	FSlateBrush Brush;

	UPROPERTY()
	UTextureRenderTarget2D* OutRenderTarget;

	// 定义顶点数据
	struct FScreenVertex
	{
		FVector4 Position;
		FVector2f UV;
	};

	// FScreenVertex Vertices[4] = {
	// 	{FVector4f(0.0f, 0.0f, 0.0f, 1.0f), FVector2f(0.0f, 0.0f)}, // 左上
	// 	{FVector4f(1.0f, 0.0f, 0.0f, 1.0f), FVector2f(1.0f, 0.0f)}, // 右上
	// 	{FVector4f(0.0f, 1.0f, 0.0f, 1.0f), FVector2f(0.0f, 1.0f)}, // 左下
	// 	{FVector4f(1.0f, 1.0f, 0.0f, 1.0f), FVector2f(1.0f, 1.0f)}, // 右下
	// };

	FScreenVertex Vertices[4] = {
		{FVector4(-1.0f, 1.0f, 0.0f, 1.0f), FVector2f(0.0f, 0.0f)}, // 左上
		{FVector4(1.0f, 1.0f, 0.0f, 1.0f), FVector2f(1.0f, 0.0f)}, // 右上
		{FVector4(-1.0f, -1.0f, 0.0f, 1.0f), FVector2f(0.0f, 1.0f)}, // 左下
		{FVector4(1.0f, -1.0f, 0.0f, 1.0f), FVector2f(1.0f, 1.0f)} // 右下
	};

	uint16 Indices[6] = {0, 1, 2, 2, 1, 3};

	FMatrix ProjectTransform = {FMatrix::Identity};

	FMatrix WorldTransform = {FMatrix::Identity};

	FMatrix TileTransform = {FMatrix::Identity};

	float Width = 0.0f;
	float Height = 0.0f;
};