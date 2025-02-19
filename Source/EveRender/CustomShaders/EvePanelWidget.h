// wyh

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "EvePanelWidget.generated.h"

class UImage;
class SEveWidget;
/**
 * 
 */
UCLASS()
class EVERENDER_API UEvePanelWidget : public UPanelWidget
{
	GENERATED_BODY()

public:
	TSharedPtr<SEveWidget> SWidget;

	virtual TSharedRef<::SWidget> RebuildWidget() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UPROPERTY()
	UMaterialInstanceDynamic* Material;

	UPROPERTY()
	UTextureRenderTarget2D* OutRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UImage* Img;
};