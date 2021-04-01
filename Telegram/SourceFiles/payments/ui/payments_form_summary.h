/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "ui/rp_widget.h"
#include "payments/ui/payments_panel_data.h"
#include "base/object_ptr.h"

namespace Ui {
class ScrollArea;
class FadeShadow;
class RoundButton;
class VerticalLayout;
} // namespace Ui

namespace Payments::Ui {

using namespace ::Ui;

class PanelDelegate;

class FormSummary final : public RpWidget {
public:
	FormSummary(
		QWidget *parent,
		const Invoice &invoice,
		const RequestedInformation &current,
		const PaymentMethodDetails &method,
		const ShippingOptions &options,
		not_null<PanelDelegate*> delegate);

	void updateThumbnail(const QImage &thumbnail);

private:
	void resizeEvent(QResizeEvent *e) override;

	void setupControls();
	[[nodiscard]] not_null<Ui::RpWidget*> setupContent();
	void setupCover(not_null<VerticalLayout*> layout);
	void setupPrices(not_null<VerticalLayout*> layout);
	void setupSections(not_null<VerticalLayout*> layout);
	void updateControlsGeometry();

	[[nodiscard]] QString formatAmount(int64 amount) const;
	[[nodiscard]] int64 computeTotalAmount() const;

	const not_null<PanelDelegate*> _delegate;
	Invoice _invoice;
	PaymentMethodDetails _method;
	ShippingOptions _options;
	RequestedInformation _information;
	object_ptr<ScrollArea> _scroll;
	object_ptr<FadeShadow> _topShadow;
	object_ptr<FadeShadow> _bottomShadow;
	object_ptr<RoundButton> _submit;
	object_ptr<RoundButton> _cancel;
	rpl::event_stream<QImage> _thumbnails;

};

} // namespace Payments::Ui