// wyh

#include "EveRenderer.h"

#include "ClearQuad.h"
#include "CommonRenderResources.h"
#include "EvePanelWidget.h"
#include "EveWidget.h"
#include "FullscreenShader.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

void EveRenderer::DrawRenderThread(class FRHICommandListImmediate& RHICmdList, const void* RenderTarget)
{
	// 开始RenderPass
	if (!OutRenderTarget) return;
	auto RT = OutRenderTarget->GetRenderTargetResource()->TextureRHI;
	if (!RT.IsValid()) return;
	RHICmdList.Transition(FRHITransitionInfo(RT, ERHIAccess::Unknown, ERHIAccess::RTV));
	FRHIRenderPassInfo RPInfo(RT.GetReference(), ERenderTargetActions::Load_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("RenderFullscreenQuadToViewport"));
	RHICmdList.SetViewport(0, 0, 0.0f, RT->GetDesc().Extent.X, RT->GetDesc().Extent.Y, 1.0f);
	// RHICmdList.SetViewport(0, 0, 0.0f, OutRenderTarget->SizeX, OutRenderTarget->SizeY, 1.0f);

	// 清除渲染目标
	DrawClearQuad(RHICmdList, FLinearColor::Transparent);
	RHICmdList.SetScissorRect(false, 0, 0, 0, 0);

	// 获取着色器
	TShaderMapRef<FFullscreenVS> VertexShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FFullscreenPS> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	// 设置图形管线状态
	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	// 创建顶点声明
	FVertexDeclarationElementList VertexElements;
	VertexElements.Add(FVertexElement(0, STRUCT_OFFSET(FScreenVertex, Position), VET_Float4, 0, sizeof(FScreenVertex)));
	VertexElements.Add(FVertexElement(0, STRUCT_OFFSET(FScreenVertex, UV), VET_Float2, 1, sizeof(FScreenVertex)));

	FVertexDeclarationRHIRef VertexDeclarationRHI = RHICreateVertexDeclaration(VertexElements);
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	// 创建顶点缓冲区
	FRHIResourceCreateInfo VertexBufferCreateInfo(TEXT("FullscreenQuadVertexBuffer"));
	FBufferRHIRef VertexBufferRHI = RHICreateVertexBuffer(sizeof(FScreenVertex) * 4, BUF_Static, VertexBufferCreateInfo);
	// 上传顶点数据
	void* VertexBufferData = RHILockBuffer(VertexBufferRHI, 0, sizeof(FScreenVertex) * 4, RLM_WriteOnly);
	FPlatformMemory::Memcpy(VertexBufferData, Vertices, sizeof(FScreenVertex) * 4);
	RHIUnlockBuffer(VertexBufferRHI);

	// 创建索引缓冲区
	FRHIResourceCreateInfo IndexBufferCreateInfo(TEXT("FullscreenQuadIndexBuffer"));
	FBufferRHIRef IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16), sizeof(uint16) * 6, BUF_Static, IndexBufferCreateInfo);
	// 上传索引数据
	void* IndexBufferData = RHILockBuffer(IndexBufferRHI, 0, sizeof(uint16) * 6, RLM_WriteOnly);
	FPlatformMemory::Memcpy(IndexBufferData, Indices, sizeof(uint16) * 6);
	RHIUnlockBuffer(IndexBufferRHI);

	ProjectTransform = CalcBaseTransform2D(Width, Height);
	// TileTransform = FScaleMatrix(FVector(Width, Height, 1)) * FTranslationMatrix(FVector(0, 0, 0));
	VertexShader->SetParameters(RHICmdList, ProjectTransform, WorldTransform, TileTransform);

	// 设置顶点缓冲区和索引缓冲区
	RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);
	RHICmdList.DrawIndexedPrimitive(IndexBufferRHI, 0, // BaseVertexIndex
	                                0, // MinIndex
	                                4, // NumVertices
	                                0, // StartIndex
	                                2, // NumPrimitives
	                                1 // NumInstances
	);

	// 结束RenderPass
	RHICmdList.EndRenderPass();
}

FMatrix EveRenderer::CalcBaseTransform2D(uint32 ViewSizeX, uint32 ViewSizeY)
{
	// Guard against division by zero.
	// ViewSizeX = FMath::Max<uint32>(ViewSizeX, 1.f);
	// ViewSizeY = FMath::Max<uint32>(ViewSizeY, 1.f);
	//
	// return AdjustProjectionMatrixForRHI(FTranslationMatrix(FVector(0, 0, 0)) * FMatrix(FPlane(1.0f / (ViewSizeX / 2.0f), 0.0, 0.0f, 0.0f), FPlane(0.0f, -1.0f / (ViewSizeY / 2.0f), 0.0f, 0.0f), FPlane(0.0f, 0.0f, 1.0f, 0.0f), FPlane(-1.0f, 1.0f, 0.0f, 1.0f)));

	// const float Left = 0.0f;
	// const float Right = ViewSizeX;
	// const float Top = 0.0f;
	// const float Bottom = ViewSizeY;
	//
	// return FMatrix(FPlane(2.0f / (Right - Left), 0.0f, 0.0f, 0.0f), FPlane(0.0f, -2.0f / (Bottom - Top), 0.0f, 0.0f), FPlane(0.0f, 0.0f, 1.0f, 0.0f), FPlane(-(Right + Left) / (Right - Left), (Bottom + Top) / (Bottom - Top), 0.0f, 1.0f));

	// return FScaleMatrix(FVector(ViewSizeX, ViewSizeY, 1.0f)) * FTranslationMatrix(FVector(-0.5f, -0.5f, 0)); // 修正偏移

	// 将 [0, 1] 映射到 [-1, 1]
	// const float ScaleX = 2.0f;
	// const float ScaleY = -2.0f; // Y轴反转
	//
	// // 将 [0, 1] 的中心点对齐到 NDC 的中心点
	// const float OffsetX = -1.0f;
	// const float OffsetY = 1.0f;
	//
	// return FMatrix(
	// 	FPlane(ScaleX, 0.0f, 0.0f, 0.0f),
	// 	FPlane(0.0f, ScaleY, 0.0f, 0.0f),
	// 	FPlane(0.0f, 0.0f, 1.0f, 0.0f),
	// 	FPlane(OffsetX, OffsetY, 0.0f, 1.0f)
	// );

	// return {FMatrix::Identity};

	const float Left = 0.0f;
	const float Right = ViewSizeX;
	const float Top = 0.0f;
	const float Bottom = ViewSizeY;

	// 将 [0, ViewSizeX] 映射到 [-1, 1]
	const float ScaleX = 2.0f / (Right - Left);
	const float ScaleY = -2.0f / (Bottom - Top); // Y轴反转

	// 将 [0, ViewSizeX] 的中心点对齐到 NDC 的中心点
	const float OffsetX = -(Right + Left) / (Right - Left);
	const float OffsetY = (Bottom + Top) / (Bottom - Top);

	return FMatrix(FPlane(ScaleX, 0.0f, 0.0f, 0.0f), FPlane(0.0f, ScaleY, 0.0f, 0.0f), FPlane(0.0f, 0.0f, 1.0f, 0.0f), FPlane(OffsetX, OffsetY, 0.0f, 1.0f));
}

// int32 EveRenderer::Draw(const SEveWidget* Widget, const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)
// {
//
// 	UGameViewportClient* ViewportClient = Widget->World->GetGameViewport();
// 	if (!ViewportClient) return LayerId;
// 	
// 	float Scale = UWidgetLayoutLibrary::GetViewportScale(Widget->World);
// 	FVector2D ScreenSize;
// 	ViewportClient->GetViewportSize(ScreenSize);
// 	Width = ScreenSize.X;
// 	Height = ScreenSize.Y;
//
// 	// Width = AllottedGeometry.GetLocalSize().X * Scale;
// 	// Height = AllottedGeometry.GetLocalSize().Y * Scale;
//
// 	FVector2D ScreenPosition(0, 0);
// 	FSlateRect ScreenRect(ScreenPosition, ScreenPosition + ScreenSize);
//
// 	FSlateDrawElement::MakeCustom(OutDrawElements, LayerId, AsShared());
// 	OutRenderTarget = Widget->OwnerUWidget->OutRenderTarget;
// 	OutRenderTarget->ResizeTarget(Width, Height);
// 	OutRenderTarget->UpdateResource();
// 	Widget->OwnerUWidget->Material->SetTextureParameterValue("OutTexture", OutRenderTarget);
//
// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("RT Size: %dx%d %f"), OutRenderTarget->SizeX, OutRenderTarget->SizeY, 1.0f));
//
// 	Brush.SetResourceObject(Widget->OwnerUWidget->Material);
// 	Brush.ImageSize = FVector2D(Width, Height);
// 	Brush.DrawAs = ESlateBrushDrawType::Image;
//
// 	auto ChildGeo = AllottedGeometry.MakeChild(ScreenRect.GetTopLeft(), ScreenRect.GetSize(), 1 / Scale);
// 	// FGeometry ChildGeo = AllottedGeometry.MakeChild(ScreenSize, FSlateLayoutTransform(FVector2D::ZeroVector), FVector2D::ZeroVector, FVector2f(1.0f, 1.0f));
//
// 	const FLinearColor FinalColorAndOpacity = InWidgetStyle.GetColorAndOpacityTint();
// 	FSlateDrawElement::MakeBox(OutDrawElements, LayerId + 1, ChildGeo.ToPaintGeometry(), &Brush, ESlateDrawEffect::NoPixelSnapping | ESlateDrawEffect::NoGamma, FinalColorAndOpacity);
//
// 	return LayerId + 1;
// }

int32 EveRenderer::Draw(const SEveWidget* Widget, const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)
{
	FSlateDrawElement::MakeCustom(OutDrawElements, LayerId, AsShared());
	OutRenderTarget = Widget->OwnerUWidget->OutRenderTarget;
	Width = 1920.f;
	Height = 1080.f;
	OutRenderTarget->ResizeTarget(Width, Height);
	OutRenderTarget->UpdateResource();
	Widget->OwnerUWidget->Material->SetTextureParameterValue("OutTexture", OutRenderTarget);
	if (!Widget->OwnerUWidget->Img) return LayerId;
	// Widget->OwnerUWidget->Img->SetBrushFromMaterial(Widget->OwnerUWidget->Material);
	Brush.SetResourceObject(Widget->OwnerUWidget->Material);
	Brush.ImageSize = FVector2D(Width, Height);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Widget->OwnerUWidget->Img->SetBrush(Brush);

	// // 获取Viewport信息
	// UGameViewportClient* ViewportClient = Widget->World->GetGameViewport();
	// if (!ViewportClient) return LayerId;
	//
	// float Scale = UWidgetLayoutLibrary::GetViewportScale(Widget->World);
	// FVector2D ScreenSize;
	// ViewportClient->GetViewportSize(ScreenSize);
	// Width = ScreenSize.X;
	// Height = ScreenSize.Y;
	//
	// // 将Widget局部坐标系转换到Viewport坐标系
	// FVector2D ViewportPosition = AllottedGeometry.LocalToAbsolute(FVector2D::ZeroVector); // Widget左上角在Viewport中的位置
	// FVector2D ViewportSize = AllottedGeometry.GetLocalSize() * Scale; // Widget在Viewport中的实际尺寸
	//
	// // 创建基于Viewport坐标系的绘制几何体
	// FSlateLayoutTransform LayoutTransform(1.0f);
	// FPaintGeometry PaintGeo = AllottedGeometry.ToPaintGeometry(FVector2f(100.0f, 100.0f), LayoutTransform);
	//
	// // 更新渲染目标
	// FSlateDrawElement::MakeCustom(OutDrawElements, LayerId, AsShared());
	// OutRenderTarget = Widget->OwnerUWidget->OutRenderTarget;
	// OutRenderTarget->ResizeTarget(Width, Height);
	// OutRenderTarget->UpdateResource();
	// Widget->OwnerUWidget->Material->SetTextureParameterValue("OutTexture", OutRenderTarget);
	//
	// // 调试信息
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("RT Size: %dx%d, Scale: %f"), OutRenderTarget->SizeX, OutRenderTarget->SizeY, Scale));
	//
	// // 设置绘制参数
	// Brush.SetResourceObject(Widget->OwnerUWidget->Material);
	// Brush.ImageSize = FVector2D(Width, Height);
	// Brush.DrawAs = ESlateBrushDrawType::Image;
	//
	// // 绘制到Viewport
	// const FLinearColor FinalColorAndOpacity = InWidgetStyle.GetColorAndOpacityTint();
	// FSlateDrawElement::MakeBox(OutDrawElements, LayerId + 1, PaintGeo, &Brush, ESlateDrawEffect::NoPixelSnapping | ESlateDrawEffect::NoGamma, FLinearColor::Yellow);

	return LayerId + 1;
}