#include "DrawSystem.h"

#include "Algorithm.h"
//
void Direct2DSystem::Draw9SliceBMP(const DrawParam& param) const {
	auto pBitMap = const_cast<Direct2DBitMap*>(reinterpret_cast<const Direct2DBitMap*>(param.m_AnyParam));
	auto* m_pD2DBitmap = pBitMap->Get(pRenderTarget);
	if (m_pD2DBitmap) {
		// •`‰æ‹éŒ`
		float XSize = Mathf::Max(param.m_FloatParam.at(2) - param.m_FloatParam.at(0), param.m_FloatParam.at(4) + param.m_FloatParam.at(6));
		float YSize = Mathf::Max(param.m_FloatParam.at(3) - param.m_FloatParam.at(1), param.m_FloatParam.at(5) + param.m_FloatParam.at(7));
		// À•W•ÏŠ·i‰ñ“]j
		pRenderTarget->SetTransform(
			D2D1::Matrix3x2F::Rotation(0.0f, D2D1::Point2F(XSize / 2, YSize / 2))
			* D2D1::Matrix3x2F::Translation(param.m_FloatParam.at(0), param.m_FloatParam.at(1))
		);
		//•`‰æ
		auto Size = m_pD2DBitmap->GetSize();
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(0.0f, 0.0f, param.m_FloatParam.at(4), param.m_FloatParam.at(5)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0.0f, 0.0f, param.m_FloatParam.at(4), param.m_FloatParam.at(5)));
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(param.m_FloatParam.at(4), 0.0f, XSize - param.m_FloatParam.at(6), param.m_FloatParam.at(5)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(param.m_FloatParam.at(4), 0.0f, Size.width - param.m_FloatParam.at(6), param.m_FloatParam.at(5)));
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(XSize - param.m_FloatParam.at(6), 0.0f, XSize, param.m_FloatParam.at(5)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(Size.width - param.m_FloatParam.at(6), 0.0f, Size.width, param.m_FloatParam.at(5)));
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(0.0f, param.m_FloatParam.at(5), param.m_FloatParam.at(4), YSize - param.m_FloatParam.at(7)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0.0f, param.m_FloatParam.at(5), param.m_FloatParam.at(4), Size.height - param.m_FloatParam.at(7)));
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(param.m_FloatParam.at(4), param.m_FloatParam.at(5), XSize - param.m_FloatParam.at(6), YSize - param.m_FloatParam.at(7)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(param.m_FloatParam.at(4), param.m_FloatParam.at(5), Size.width - param.m_FloatParam.at(6), Size.height - param.m_FloatParam.at(7)));
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(XSize - param.m_FloatParam.at(6), param.m_FloatParam.at(5), XSize, YSize - param.m_FloatParam.at(7)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(Size.width - param.m_FloatParam.at(6), param.m_FloatParam.at(5), Size.width, Size.height - param.m_FloatParam.at(7)));
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(0.0f, YSize - param.m_FloatParam.at(7), param.m_FloatParam.at(4), YSize), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0.0f, Size.height - param.m_FloatParam.at(7), param.m_FloatParam.at(4), Size.height));
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(param.m_FloatParam.at(4), YSize - param.m_FloatParam.at(7), XSize - param.m_FloatParam.at(6), YSize), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(param.m_FloatParam.at(4), Size.height - param.m_FloatParam.at(7), Size.width - param.m_FloatParam.at(6), Size.height));
		pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(XSize - param.m_FloatParam.at(6), YSize - param.m_FloatParam.at(7), XSize, YSize), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(Size.width - param.m_FloatParam.at(6), Size.height - param.m_FloatParam.at(7), Size.width, Size.height));
		// À•W•ÏŠ·‚ðŒ³‚É–ß‚·
		pRenderTarget->SetTransform(
			D2D1::Matrix3x2F::Identity()
		);
	}
}
void Direct2DSystem::DrawFont(const DrawParam& param) const {
	ID2D1SolidColorBrush* pBrush{ nullptr };
	pRenderTarget->CreateSolidColorBrush(param.m_ColorParam.at(0), &pBrush);
	if (pBrush) {
		// À•W•ÏŠ·i‰ñ“]j
		pRenderTarget->SetTransform(
			D2D1::Matrix3x2F::Rotation(0.0f, D2D1::Point2F((param.m_FloatParam.at(2) - param.m_FloatParam.at(0)) / 2, (param.m_FloatParam.at(3) - param.m_FloatParam.at(1)) / 2))
			* D2D1::Matrix3x2F::Translation(param.m_FloatParam.at(0), param.m_FloatParam.at(1))
		);
		//•`‰æ
		pRenderTarget->DrawText(
			param.m_StringParam.at(0).c_str(),
			(UINT32)wcslen(param.m_StringParam.at(0).c_str()),
			(reinterpret_cast<const Direct2DFont*>(param.m_AnyParam))->Get(),
			D2D1::RectF(0, 0, param.m_FloatParam.at(2) - param.m_FloatParam.at(0), param.m_FloatParam.at(3) - param.m_FloatParam.at(1)),
			pBrush
		);

		// À•W•ÏŠ·‚ðŒ³‚É–ß‚·
		pRenderTarget->SetTransform(
			D2D1::Matrix3x2F::Identity()
		);
	}
	SafeRelease(&pBrush);

}
