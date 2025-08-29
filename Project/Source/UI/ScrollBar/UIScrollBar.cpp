#include "UIScrollBar.h"

#include "../../Direct2DLib.h"
#include "../../Input/Input.h"

#include "../../Algorithm.h"

void ScrollBarParts::Init(ButtonParam buttonParam) {
	m_buttonParam = buttonParam;
}

void ScrollBarParts::Update(int ScrollOnce, int ScrollMax, float* pScrollNow) {
	bool OnMouse = HitPointToRectangle(
		static_cast<int>(Input::Instance()->GetMousePosition().x), static_cast<int>(Input::Instance()->GetMousePosition().y),
		m_buttonParam.xmin, m_buttonParam.ymin, m_buttonParam.xmax, m_buttonParam.ymax
	);
	bool Trigger = Input::Instance()->GetKeyTrigger(VK_LBUTTON);
	bool Press = Input::Instance()->GetKeyPress(VK_LBUTTON);

	if (!OnPress) {
		if (OnMouse && Trigger) {
			OnPress = true;
		}
	}
	else {
		if (!Press) {
			OnPress = false;
		}
	}

	D2D1::ColorF BGColor = m_buttonParam.m_BaseColor;
	//選択時
	if (OnMouse) {
		BGColor = D2D1::ColorF(0.8f, 0.8f, 0.8f);
		//
		if (!OnPress) {
			HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
			SetCursor(hCursor);
		}
	}
	//押下時
	if (OnPress) {
		BGColor = D2D1::ColorF(0.5f, 0.5f, 0.5f);
		//
		HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
		SetCursor(hCursor);
	}
	Direct2DLib::Instance()->GetDrawSystem()->SetRect(
		static_cast<float>(m_buttonParam.xmin), static_cast<float>(m_buttonParam.ymin),
		static_cast<float>(m_buttonParam.xmax), static_cast<float>(m_buttonParam.ymax),
		D2D1::ColorF(0.0f, 0.0f, 0.0f));
	;
	Direct2DLib::Instance()->GetDrawSystem()->SetRect(
		static_cast<float>(m_buttonParam.xmin + 1), static_cast<float>(m_buttonParam.ymin + static_cast<int>(static_cast<float>(m_buttonParam.ymax - m_buttonParam.ymin) * *pScrollNow / static_cast<float>(ScrollMax)) + 1),
		static_cast<float>(m_buttonParam.xmax - 1), static_cast<float>(m_buttonParam.ymin + static_cast<int>(static_cast<float>(m_buttonParam.ymax - m_buttonParam.ymin) * (*pScrollNow + static_cast<float>(Mathf::Min(ScrollOnce, ScrollMax))) / static_cast<float>(ScrollMax)) - 1),
		BGColor);

	//スクロール値の反映
	if (OnPress) {
		*pScrollNow = static_cast<float>(ScrollMax) * static_cast<float>((static_cast<int>(Input::Instance()->GetMousePosition().y) - m_buttonParam.ymin) / (m_buttonParam.ymax - m_buttonParam.ymin)) - static_cast<float>(ScrollOnce) / 2;
	}
	*pScrollNow -= static_cast<float>(Input::Instance()->GetMouseWheelRotVel()) / 960.0f;
	*pScrollNow = Mathf::Clamp(*pScrollNow, 0.0f, Mathf::Max(0.0f, static_cast<float>(ScrollMax - ScrollOnce)));
}