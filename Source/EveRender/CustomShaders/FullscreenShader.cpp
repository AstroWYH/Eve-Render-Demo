#include "FullscreenShader.h"

FFullscreenVS::FFullscreenVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	Transform1.Bind(Initializer.ParameterMap, TEXT("ProjectTransform"), SPF_Mandatory);
	Transform2.Bind(Initializer.ParameterMap, TEXT("WorldTransform"), SPF_Mandatory);
	Transform3.Bind(Initializer.ParameterMap, TEXT("TileTransform"), SPF_Mandatory);
}

void FFullscreenVS::SetParameters(FRHICommandList& RHICmdList, const FMatrix& TransformValue1, const FMatrix& TransformValue2, const FMatrix& TransformValue3)
{
	SetShaderValue(RHICmdList, RHICmdList.GetBoundVertexShader(), Transform1, FMatrix44f(TransformValue1));
	SetShaderValue(RHICmdList, RHICmdList.GetBoundVertexShader(), Transform2, FMatrix44f(TransformValue2));
	SetShaderValue(RHICmdList, RHICmdList.GetBoundVertexShader(), Transform3, FMatrix44f(TransformValue3));
}

IMPLEMENT_SHADER_TYPE(, FFullscreenVS, TEXT("/Engine/Private/FullscreenShader.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FFullscreenPS, TEXT("/Engine/Private/FullscreenShader.usf"), TEXT("MainPS"), SF_Pixel);