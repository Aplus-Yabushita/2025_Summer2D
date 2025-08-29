#include "Bitmap.h"

#include "../Direct2DLib.h"

void Direct2DBitMap::Load(const WCHAR* szFileName) {
	IWICBitmapDecoder* pDecoder = NULL;		// Create a decoder
	HRESULT hr = Direct2DLib::Instance()->GetWICFactory()->CreateDecoderFromFilename(
		szFileName,					  // Image to be decoded
		NULL,							// Do not prefer a particular vendor
		GENERIC_READ,					// Desired read access to the file
		WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
		&pDecoder						// Pointer to the decoder
	);

	IWICBitmapFrameDecode* pFrame = NULL;		// デコーダから画像の最初のフレームを取得します

	if (SUCCEEDED(hr)) {
		hr = pDecoder->GetFrame(0, &pFrame);
	}

	//ステップ 3: フレームを 32bppPBGRA にフォーマット変換する
	if (SUCCEEDED(hr)) {
		SafeRelease(&m_pConvertedSourceBitmap);
		hr = Direct2DLib::Instance()->GetWICFactory()->CreateFormatConverter(&m_pConvertedSourceBitmap);
	}

	if (SUCCEEDED(hr)) {
		hr = m_pConvertedSourceBitmap->Initialize(
			pFrame,						  // Input bitmap to convert
			GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
			WICBitmapDitherTypeNone,		 // Specified dither pattern
			NULL,							// Specify a particular palette 
			0.0f,							 // Alpha threshold
			WICBitmapPaletteTypeCustom	   // Palette translation type
		);
	}
	SafeRelease(&pDecoder);
	SafeRelease(&pFrame);
}
