// wyh

#include "EveWidget.h"
#include "EveRenderer.h"

SEveWidget::SEveWidget(): Children(this)
{
}

void SEveWidget::Construct(const FArguments& InArgs)
{
}

int32 SEveWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 NewLayerId = LayerId;
	if (Renderer.IsValid())
	{
		NewLayerId = Renderer->Draw(this, Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	}

	return SPanel::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, NewLayerId, InWidgetStyle, bParentEnabled);;
}

void SEveWidget::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	SEveWidget* MutableThis = const_cast<SEveWidget*>(this);
	MutableThis->UpdateResource(AllottedGeometry, ArrangedChildren);
}

void SEveWidget::UpdateResource(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren)
{
	if (!Renderer.IsValid())
	{
		Renderer = MakeShared<EveRenderer, ESPMode::ThreadSafe>();
	}
}

FVector2D SEveWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(1920.f, 1080.f);
}

FChildren* SEveWidget::GetChildren()
{
	return &Children;
}

SEveWidget::FSlot& SEveWidget::AddSlot()
{
	FSlot& NewSlot = *(new FSlot());
	Children.Add(&NewSlot);
	return NewSlot;
}