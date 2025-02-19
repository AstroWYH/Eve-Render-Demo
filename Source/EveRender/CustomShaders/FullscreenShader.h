#pragma once

#include "GlobalShader.h"

// 顶点着色器类
class FFullscreenVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FFullscreenVS, Global);

public:
	FFullscreenVS()
	{
	}

	FFullscreenVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetParameters(FRHICommandList& RHICmdList, const FMatrix& TransformValue1, const FMatrix& TransformValue2, const FMatrix& TransformValue3);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		UE_LOG(LogTemp, Log, TEXT("[wyh] VS成功"));
		return true;
	}

private:
	LAYOUT_FIELD(FShaderParameter, Transform1)
	LAYOUT_FIELD(FShaderParameter, Transform2)
	LAYOUT_FIELD(FShaderParameter, Transform3)
};

// 像素着色器类
class FFullscreenPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FFullscreenPS, Global);

public:
	FFullscreenPS()
	{
	}

	FFullscreenPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		UE_LOG(LogTemp, Log, TEXT("[wyh] PS成功"));
		return true;
	}
};