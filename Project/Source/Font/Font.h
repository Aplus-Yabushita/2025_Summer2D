#pragma once

#include "../Util.h"

#include <dwrite.h>
#pragma comment(lib, "dwrite")
//
class Direct2DFont {
	IDWriteFactory* m_pDWriteFactory{ nullptr };
	IDWriteTextFormat* m_pTextFormat{ nullptr };

	int m_Size{};
	DWRITE_TEXT_ALIGNMENT m_TextAlign{};
	DWRITE_PARAGRAPH_ALIGNMENT m_ParaAlign{};
public:
	Direct2DFont() {}
	virtual ~Direct2DFont() {
		SafeRelease(&m_pTextFormat);
		SafeRelease(&m_pDWriteFactory);
	}
public:
	bool Equal(int Size, DWRITE_TEXT_ALIGNMENT TextAlign, DWRITE_PARAGRAPH_ALIGNMENT ParaAlign) const {
		return
			(m_Size == Size) &&
			(m_TextAlign == TextAlign) &&
			(m_ParaAlign == ParaAlign);
	}
	void Load(const WCHAR* msc_fontName, int msc_fontSize, DWRITE_TEXT_ALIGNMENT TextAlign, DWRITE_PARAGRAPH_ALIGNMENT ParaAlign) {
		m_Size = msc_fontSize;
		m_TextAlign = TextAlign;
		m_ParaAlign = ParaAlign;

		// Create a DirectWrite factory.
		HRESULT hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);
		if (FAILED(hr)) { return; }
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			static_cast<float>(m_Size),
			L"", //locale
			&m_pTextFormat
		);
		if (FAILED(hr)) { return; }
		// Center the text horizontally and vertically.
		m_pTextFormat->SetTextAlignment(TextAlign);

		m_pTextFormat->SetParagraphAlignment(ParaAlign);
	}

	IDWriteTextFormat* Get() const {
		return m_pTextFormat;
	}
};
