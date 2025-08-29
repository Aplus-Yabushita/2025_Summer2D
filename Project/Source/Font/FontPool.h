#pragma once

#include "../Util.h"
#include "../Algorithm.h"

#include "Font.h"

#include <memory>
//センタリング用EnumとDirect2D対応のEnumとの変換関数
enum class FontCenterX {
	Left,
	Center,
	Right,
};
enum class FontCenterY {
	Top,
	Center,
	Bottom,
};
static DWRITE_TEXT_ALIGNMENT GetCenterX(FontCenterX centerX) {
	switch (centerX) {
	case FontCenterX::Left:
		return DWRITE_TEXT_ALIGNMENT_LEADING;
	case FontCenterX::Center:
		return DWRITE_TEXT_ALIGNMENT_CENTER;
	case FontCenterX::Right:
		return DWRITE_TEXT_ALIGNMENT_TRAILING;
	default:
		return DWRITE_TEXT_ALIGNMENT_LEADING;
	}
}
static DWRITE_PARAGRAPH_ALIGNMENT GetCenterY(FontCenterY centerY) {
	switch (centerY) {
	case FontCenterY::Top:
		return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
	case FontCenterY::Center:
		return DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
	case FontCenterY::Bottom:
		return DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	default:
		return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
	}
}
//
class FontPool : public SingletonBase<FontPool> {
private:
	friend class SingletonBase<FontPool>;
private:
	List<std::unique_ptr<Direct2DFont>> m_List;
private:
	FontPool() {}
	FontPool(const FontPool&) = delete;
	FontPool(FontPool&&) = delete;
	FontPool& operator=(const FontPool&) = delete;
	FontPool& operator=(FontPool&&) = delete;
	virtual ~FontPool() {
		m_List.Clear();
	}
public:
	const std::unique_ptr<Direct2DFont>& Add(int Size, FontCenterX centerX, FontCenterY centerY) {
		auto* pPtr = m_List.AddBack();
		(*pPtr) = std::make_unique<Direct2DFont>();
		(*pPtr)->Load(L"MSゴシック", Size, GetCenterX(centerX), GetCenterY(centerY));
		return (*pPtr);
	}
	const std::unique_ptr<Direct2DFont>& GetFont(int Size, FontCenterX centerX, FontCenterY centerY) {
		const auto* pPtr = m_List.Get([&](const std::unique_ptr<Direct2DFont>& A) { return A->Equal(Size, GetCenterX(centerX), GetCenterY(centerY)); });
		if (pPtr) {
			return (*pPtr);
		}
		return Add(Size, centerX, centerY);
	}
};
