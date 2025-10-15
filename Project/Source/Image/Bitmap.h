#pragma once

#include "../Util.h"

#include <wincodec.h>

#include <d2d1.h>
#include <d2d1helper.h>
#pragma comment(lib, "d2d1")

//
class Direct2DBitMap {
	ID2D1Bitmap* m_pD2DBitmap{ nullptr };
	IWICFormatConverter* m_pConvertedSourceBitmap{ nullptr };
public:
	Direct2DBitMap() {}
	Direct2DBitMap(const Direct2DBitMap&) = delete;
	Direct2DBitMap(Direct2DBitMap&&) = delete;
	Direct2DBitMap& operator=(const Direct2DBitMap&) = delete;
	Direct2DBitMap& operator=(Direct2DBitMap&&) = delete;
	virtual ~Direct2DBitMap() {
		//SafeRelease(&m_pD2DBitmap);
		//SafeRelease(&m_pConvertedSourceBitmap);
		m_pD2DBitmap = nullptr;
		m_pConvertedSourceBitmap = nullptr;
	}
public:
	void Load(const WCHAR* szFileName);

	ID2D1Bitmap* Get(ID2D1HwndRenderTarget* pRenderTarget) {
		// D2DBitmap はデバイスの紛失により解放された可能性があります。存在する場合は、ソース ビットマップから再作成します
		if (m_pConvertedSourceBitmap && !m_pD2DBitmap) {
			HRESULT hr = pRenderTarget->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
			if (FAILED(hr)) {
				MessageBox(NULL, "Failed WicBitmap Create", "", MB_OK);
				exit(-1);
			}
		}
		return m_pD2DBitmap;
	}
};
