#pragma once

#include "Algorithm.h"

#include <array>
#include <string>

#include "Image/BitMap.h"
#include "Font/Font.h"
//
enum class DrawType {
	ClearScreen,
	DrawEllipse,
	DrawRect,
	DrawExtendBMP,
	Draw9SliceBMP,
	DrawFont,
};

struct DrawParam {
	DrawType m_DrawType{};
	std::array<float, 10> m_FloatParam{};
	std::array<D2D1_COLOR_F, 1> m_ColorParam{};
	std::array<std::wstring, 1> m_StringParam{};
	const void* m_AnyParam{};//ビットマップなどの情報を渡すための汎用ポインタ reinterpret_castを使用しています
};

class Direct2DSystem {
private:
	List<DrawParam> m_List;
	ID2D1Factory* pFactory{ nullptr };
	ID2D1HwndRenderTarget* pRenderTarget{ nullptr };
public:
	bool IsActive(void) const { return pFactory != nullptr; }
	D2D1_SIZE_F GetSize(void) const { return pRenderTarget->GetSize(); }
public:
	Direct2DSystem(const HWND& hwnd) {
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
			pFactory = nullptr;
		}
		Create(hwnd);
	}
	virtual ~Direct2DSystem() {
		SafeRelease(&pRenderTarget);
		SafeRelease(&pFactory);
	}
public:
	void Create(const HWND& hwnd) {
		RECT rc;
		GetClientRect(hwnd, &rc);
		SafeRelease(&pRenderTarget);
		if (FAILED(pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(static_cast<UINT32>(rc.right), static_cast<UINT32>(rc.bottom))),
			&pRenderTarget))) {
			pRenderTarget = nullptr;
		}
	}
	void Resize(const HWND& hwnd) {
		RECT rc;
		GetClientRect(hwnd, &rc);
		pRenderTarget->Resize(D2D1::SizeU(static_cast<UINT32>(rc.right), static_cast<UINT32>(rc.bottom)));
	}
public:
	void Draw(const HWND& hwnd) {
		pRenderTarget->BeginDraw();
		for (auto& d : m_List.m_item) {
			switch (d.m_DrawType) {
			case DrawType::ClearScreen:
				ClearScreen(d);
				break;
			case DrawType::DrawEllipse:
				DrawEllipse(d);
				break;
			case DrawType::DrawRect:
				DrawRect(d);
				break;
			case DrawType::DrawExtendBMP:
				DrawExtendBMP(d);
				break;
			case DrawType::Draw9SliceBMP:
				Draw9SliceBMP(d);
				break;
			case DrawType::DrawFont:
				DrawFont(d);
				break;
			default:
				break;
			}
		}
		m_List.Clear();
		HRESULT hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			Create(hwnd);
		}
	}
private://実際にWN_PAINTで実行する描画命令
	void ClearScreen(const DrawParam& param) const {
		pRenderTarget->Clear(param.m_ColorParam.at(0));
	}
	void DrawEllipse(const DrawParam& param) const {
		D2D1_ELLIPSE		  ellipse{};
		ID2D1SolidColorBrush* pBrush{ nullptr };
		ellipse = D2D1::Ellipse(D2D1::Point2F(param.m_FloatParam.at(0), param.m_FloatParam.at(1)), param.m_FloatParam.at(2), param.m_FloatParam.at(3));
		pRenderTarget->CreateSolidColorBrush(param.m_ColorParam.at(0), &pBrush);
		if (pBrush) {
			pRenderTarget->FillEllipse(ellipse, pBrush);
		}
		SafeRelease(&pBrush);
	}
	void DrawRect(const DrawParam& param) const {
		D2D1_RECT_F		  rectangle{};
		ID2D1SolidColorBrush* pBrush{ nullptr };
		rectangle = D2D1::RectF(0, 0, param.m_FloatParam.at(2) - param.m_FloatParam.at(0), param.m_FloatParam.at(3) - param.m_FloatParam.at(1));
		pRenderTarget->CreateSolidColorBrush(param.m_ColorParam.at(0), &pBrush);
		if (pBrush) {
			// 座標変換（回転）
			pRenderTarget->SetTransform(
				D2D1::Matrix3x2F::Rotation(0.0f, D2D1::Point2F((param.m_FloatParam.at(2) - param.m_FloatParam.at(0)) / 2, (param.m_FloatParam.at(3) - param.m_FloatParam.at(1)) / 2))
				* D2D1::Matrix3x2F::Translation(param.m_FloatParam.at(0), param.m_FloatParam.at(1))
			);

			pRenderTarget->FillRectangle(rectangle, pBrush);

			// 座標変換を元に戻す
			pRenderTarget->SetTransform(
				D2D1::Matrix3x2F::Identity()
			);
		}
		SafeRelease(&pBrush);
	}
	void DrawExtendBMP(const DrawParam& param) const {
		auto pBitMap = const_cast<Direct2DBitMap*>(reinterpret_cast<const Direct2DBitMap*>(param.m_AnyParam));
		auto* m_pD2DBitmap = pBitMap->Get(pRenderTarget);
		if (m_pD2DBitmap) {
			// 座標変換（回転）
			pRenderTarget->SetTransform(
				D2D1::Matrix3x2F::Rotation(0.0f, D2D1::Point2F((param.m_FloatParam.at(2) - param.m_FloatParam.at(0)) / 2, (param.m_FloatParam.at(3) - param.m_FloatParam.at(1)) / 2))
				* D2D1::Matrix3x2F::Translation(param.m_FloatParam.at(0), param.m_FloatParam.at(1))
			);
			//描画
			pRenderTarget->DrawBitmap(m_pD2DBitmap, D2D1::RectF(0, 0, param.m_FloatParam.at(2) - param.m_FloatParam.at(0), param.m_FloatParam.at(3) - param.m_FloatParam.at(1)), param.m_FloatParam.at(4), D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			// 座標変換を元に戻す
			pRenderTarget->SetTransform(
				D2D1::Matrix3x2F::Identity()
			);
		}
	}
	void Draw9SliceBMP(const DrawParam& param) const;
	void DrawFont(const DrawParam& param) const;
public://外からパラメーターを登録する

	//画面全体を引数の色で塗りつぶす
	void SetClearScreen(D2D1_COLOR_F ClearColor) {
		auto* pPtr = m_List.AddBack();
		pPtr->m_DrawType = DrawType::ClearScreen;
		pPtr->m_ColorParam.at(0) = ClearColor;
	}
	//楕円を書く
	void SetEllipse(float x, float y, float xradius, float yradius, D2D1_COLOR_F FillColor) {
		auto* pPtr = m_List.AddBack();
		pPtr->m_DrawType = DrawType::DrawEllipse;
		pPtr->m_FloatParam.at(0) = x;
		pPtr->m_FloatParam.at(1) = y;
		pPtr->m_FloatParam.at(2) = xradius;
		pPtr->m_FloatParam.at(3) = yradius;
		pPtr->m_ColorParam.at(0) = FillColor;
	}
	//矩形を書く
	void SetRect(float xmin, float ymin, float xmax, float ymax, D2D1_COLOR_F FillColor) {
		auto* pPtr = m_List.AddBack();
		pPtr->m_DrawType = DrawType::DrawRect;
		pPtr->m_FloatParam.at(0) = xmin;
		pPtr->m_FloatParam.at(1) = ymin;
		pPtr->m_FloatParam.at(2) = xmax;
		pPtr->m_FloatParam.at(3) = ymax;
		pPtr->m_ColorParam.at(0) = FillColor;
	}
	//引数のサイズに引き伸ばしたビットマップを描画
	void SetExtendBMP(float xmin, float ymin, float xmax, float ymax, float alpha, const Direct2DBitMap& bmp) {
		auto* pPtr = m_List.AddBack();
		pPtr->m_DrawType = DrawType::DrawExtendBMP;
		pPtr->m_FloatParam.at(0) = xmin;
		pPtr->m_FloatParam.at(1) = ymin;
		pPtr->m_FloatParam.at(2) = xmax;
		pPtr->m_FloatParam.at(3) = ymax;
		pPtr->m_FloatParam.at(4) = alpha;

		pPtr->m_AnyParam = reinterpret_cast<const void*>(&bmp);
	}
	//9スライスを適用したBMPを描画
	void Set9SliceBMP(float xmin, float ymin, float xmax, float ymax,
		float MinX, float MinY, float MaxX, float MaxY,//切り出す左上/右下座標
		const Direct2DBitMap& bmp) {
		auto* pPtr = m_List.AddBack();
		pPtr->m_DrawType = DrawType::Draw9SliceBMP;
		pPtr->m_FloatParam.at(0) = xmin;
		pPtr->m_FloatParam.at(1) = ymin;
		pPtr->m_FloatParam.at(2) = xmax;
		pPtr->m_FloatParam.at(3) = ymax;

		pPtr->m_FloatParam.at(4) = MinX;
		pPtr->m_FloatParam.at(5) = MinY;
		pPtr->m_FloatParam.at(6) = MaxX;
		pPtr->m_FloatParam.at(7) = MaxY;

		pPtr->m_AnyParam = reinterpret_cast<const void*>(&bmp);
	}
	//引数のフォントで文字を描画
	void SetFont(float xmin, float ymin, float xmax, float ymax, const WCHAR* Str, D2D1_COLOR_F FillColor, const Direct2DFont* Font) {
		auto* pPtr = m_List.AddBack();
		pPtr->m_DrawType = DrawType::DrawFont;
		pPtr->m_ColorParam.at(0) = FillColor;

		pPtr->m_FloatParam.at(0) = xmin;
		pPtr->m_FloatParam.at(1) = ymin;
		pPtr->m_FloatParam.at(2) = xmax;
		pPtr->m_FloatParam.at(3) = ymax;

		pPtr->m_StringParam.at(0) = Str;

		pPtr->m_AnyParam = reinterpret_cast<const void*>(Font);
	}
};
