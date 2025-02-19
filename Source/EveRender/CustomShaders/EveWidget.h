#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UEvePanelWidget;
class EveRenderer;

class SEveWidget : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SEveWidget)
		{
		}

	SLATE_END_ARGS()

	SEveWidget();

	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	void UpdateResource(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren);

	FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const;

	FChildren* GetChildren();

	TSharedPtr<EveRenderer, ESPMode::ThreadSafe> Renderer;

	struct FSlot : public TSlotBase<FSlot>
	{
		FSlot() : TSlotBase<FSlot>()
		{
		}

		virtual ~FSlot()
		{
		}
	};

	FSlot& AddSlot();

	TPanelChildren<FSlot> Children;

	TWeakObjectPtr<UEvePanelWidget> OwnerUWidget;

	UWorld* World;
};