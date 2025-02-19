// wyh


#include "EvePanelWidget.h"

#include "EveWidget.h"
#include "Engine/TextureRenderTarget2D.h"

TSharedRef<SWidget> UEvePanelWidget::RebuildWidget()
{
	Material = UMaterialInstanceDynamic::Create((LoadObject<UMaterialInterface>(nullptr, TEXT("/Script/Engine.Material'/Game/TestM.TestM'"))), nullptr);
	
	if (OutRenderTarget == nullptr)
	{
		OutRenderTarget = NewObject<UTextureRenderTarget2D>(GetTransientPackage());
		OutRenderTarget->ClearColor = FLinearColor::Transparent;
		OutRenderTarget->RenderTargetFormat = RTF_RGBA8;
	}

	SWidget = SNew(SEveWidget);
	SWidget->OwnerUWidget = this;
	SWidget->World = GetWorld();

	return SWidget.ToSharedRef();
}

void UEvePanelWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	SWidget.Reset();
}