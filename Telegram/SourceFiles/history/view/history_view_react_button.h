/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "ui/effects/animations.h"
#include "ui/widgets/inner_dropdown.h"

namespace Ui {
struct ChatPaintContext;
} // namespace Ui

namespace Data {
struct Reaction;
class DocumentMedia;
} // namespace Data

namespace HistoryView {
using PaintContext = Ui::ChatPaintContext;
struct TextState;
} // namespace HistoryView

namespace HistoryView::Reactions {

enum class ButtonStyle {
	Bubble,
};

struct ButtonParameters {
	[[nodiscard]] ButtonParameters translated(QPoint delta) const {
		auto result = *this;
		result.center += delta;
		result.pointer += delta;
		return result;
	}

	FullMsgId context;
	QPoint center;
	QPoint pointer;
	ButtonStyle style = ButtonStyle::Bubble;
	bool inside = false;
	bool active = false;
	bool outbg = false;
};

enum class ButtonState {
	Hidden,
	Shown,
	Active,
};

class Button final {
public:
	Button(Fn<void(QRect)> update, ButtonParameters parameters);
	~Button();

	void applyParameters(ButtonParameters parameters);

	using State = ButtonState;
	void applyState(State state);

	[[nodiscard]] bool outbg() const;
	[[nodiscard]] bool isHidden() const;
	[[nodiscard]] QRect geometry() const;
	[[nodiscard]] float64 currentScale() const;
	[[nodiscard]] static float64 ScaleForState(State state);
	[[nodiscard]] static float64 OpacityForScale(float64 scale);

private:
	const Fn<void(QRect)> _update;
	State _state = State::Hidden;
	Ui::Animations::Simple _scaleAnimation;

	QRect _geometry;
	bool _outbg = false;

};

class Selector final {
public:
	Selector(
		QWidget *parent,
		const std::vector<Data::Reaction> &list);

	void showAround(QRect area);
	void toggle(bool shown, anim::type animated);

	[[nodiscard]] rpl::producer<QString> chosen() const;

	[[nodiscard]] rpl::lifetime &lifetime();

private:
	struct Element {
		QString emoji;
		QRect geometry;
	};
	Ui::InnerDropdown _dropdown;
	rpl::event_stream<QString> _chosen;
	std::vector<Element> _elements;
	bool _fromTop = true;
	bool _fromLeft = true;

};

class Manager final : public base::has_weak_ptr {
public:
	Manager(QWidget *selectorParent, Fn<void(QRect)> buttonUpdate);
	~Manager();

	void applyList(std::vector<Data::Reaction> list);

	void showButton(ButtonParameters parameters);
	void paintButtons(Painter &p, const PaintContext &context);
	[[nodiscard]] TextState buttonTextState(QPoint position) const;
	void remove(FullMsgId context);

	void showSelector(Fn<QPoint(QPoint)> mapToGlobal);
	void showSelector(FullMsgId context, QRect globalButtonArea);

	void hideSelectors(anim::type animated);

	struct Chosen {
		FullMsgId context;
		QString emoji;
	};
	[[nodiscard]] rpl::producer<Chosen> chosen() const {
		return _chosen.events();
	}

private:
	static constexpr auto kFramesCount = 30;

	[[nodiscard]] bool overCurrentButton(QPoint position) const;

	void removeStaleButtons();
	void paintButton(
		Painter &p,
		const PaintContext &context,
		not_null<Button*> button);
	void paintButton(
		Painter &p,
		const PaintContext &context,
		not_null<Button*> button,
		int frame,
		float64 scale);

	void setMainReactionImage(QImage image);
	void applyPatternedShadow(const QColor &shadow);
	[[nodiscard]] QRect cacheRect(int frameIndex, int columnIndex) const;
	QRect validateShadow(
		int frameIndex,
		float64 scale,
		const QColor &shadow);
	QRect validateEmoji(int frameIndex, float64 scale);
	QRect validateFrame(
		bool outbg,
		int frameIndex,
		float64 scale,
		const QColor &background,
		const QColor &shadow);
	QRect validateMask(int frameIndex, float64 scale);
	void validateCacheForPattern(
		int frameIndex,
		float64 scale,
		const QRect &geometry,
		const PaintContext &context);

	rpl::event_stream<Chosen> _chosen;
	std::vector<Data::Reaction> _list;
	QSize _outer;
	QRectF _inner;
	QRect _innerActive;
	QImage _cacheInOut;
	QImage _cacheParts;
	QImage _cacheForPattern;
	QImage _shadowBuffer;
	std::array<bool, kFramesCount> _validIn = { { false } };
	std::array<bool, kFramesCount> _validOut = { { false } };
	std::array<bool, kFramesCount> _validShadow = { { false } };
	std::array<bool, kFramesCount> _validEmoji = { { false } };
	std::array<bool, kFramesCount> _validMask = { { false } };
	QColor _backgroundIn;
	QColor _backgroundOut;
	QColor _shadow;

	std::shared_ptr<Data::DocumentMedia> _mainReactionMedia;
	QImage _mainReactionImage;
	rpl::lifetime _mainReactionLifetime;

	const Fn<void(QRect)> _buttonUpdate;
	std::unique_ptr<Button> _button;
	std::vector<std::unique_ptr<Button>> _buttonHiding;
	FullMsgId _buttonContext;
	ClickHandlerPtr _buttonLink;

	QWidget *_selectorParent = nullptr;
	std::unique_ptr<Selector> _selector;
	std::vector<std::unique_ptr<Selector>> _selectorHiding;
	FullMsgId _selectorContext;

};

} // namespace HistoryView