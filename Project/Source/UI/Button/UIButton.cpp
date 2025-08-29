#include "UIButton.h"

#include "../../Direct2DLib.h"
#include "../../Input/Input.h"

#include "../../Font/FontPool.h"

void ButtonParts::Init(ButtonParam buttonParam) {
	m_BMP.Load(L".\\Data\\UI\\Button.png");
	m_buttonParam = buttonParam;
	FontPool::Instance()->Add(m_buttonParam.textSize, FontCenterX::Center, FontCenterY::Center);
}

void ButtonParts::Update() {
	if (m_buttonStatus == ButtonStatus::Inactive) {
		return;
	}

	bool OnMouse = HitPointToRectangle(
		static_cast<int>(Input::Instance()->GetMousePosition().x), static_cast<int>(Input::Instance()->GetMousePosition().y),
		m_buttonParam.xmin, m_buttonParam.ymin, m_buttonParam.xmax, m_buttonParam.ymax
	);
	bool Trigger = OnMouse;
	switch (m_buttonParam.m_ButtonType) {
	case ButtonType::Once:
		Trigger &= Input::Instance()->GetKeyTrigger(VK_LBUTTON);
		break;
	case ButtonType::Repeat:
		Trigger &= Input::Instance()->GetKeyRepeat(VK_LBUTTON);
		break;
	default:
		break;
	}
	bool Press = OnMouse && Input::Instance()->GetKeyPress(VK_LBUTTON);

	if (m_buttonStatus != ButtonStatus::Enable) {
		OnMouse = false;
		Trigger = false;
		Press = false;
	}

	m_BGColor = m_buttonParam.m_BaseColor;
	if (m_buttonStatus != ButtonStatus::Enable) {
		m_BGColor = D2D1::ColorF(0.4f, 0.4f, 0.4f);
	}
	//‘I‘ðŽž
	if (OnMouse) {
		m_BGColor = D2D1::ColorF(0.8f, 0.8f, 0.8f);
		//
		HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
		SetCursor(hCursor);
	}
	//‰Ÿ‰ºŽž
	if (Press) {
		m_BGColor = D2D1::ColorF(0.5f, 0.5f, 0.5f);
	}
	if (Trigger) {
		if (m_buttonParam.m_PressEvent) {
			m_buttonParam.m_PressEvent();
		}
	}
}

void ButtonParts::Draw() {
	//•\Ž¦•¨Ý’è
	Direct2DLib::Instance()->GetDrawSystem()->SetRect(static_cast<float>(m_buttonParam.xmin), static_cast<float>(m_buttonParam.ymin), static_cast<float>(m_buttonParam.xmax), static_cast<float>(m_buttonParam.ymax),
		m_BGColor);
	Direct2DLib::Instance()->GetDrawSystem()->Set9SliceBMP(static_cast<float>(m_buttonParam.xmin), static_cast<float>(m_buttonParam.ymin), static_cast<float>(m_buttonParam.xmax), static_cast<float>(m_buttonParam.ymax),
		24.0f, 24.0f, 24.0f, 24.0f, m_BMP);
	Direct2DLib::Instance()->GetDrawSystem()->SetFont(static_cast<float>(m_buttonParam.xmin), static_cast<float>(m_buttonParam.ymin), static_cast<float>(m_buttonParam.xmax), static_cast<float>(m_buttonParam.ymax),
		m_buttonParam.m_ButtonText.c_str(),
		D2D1::ColorF(D2D1::ColorF::Black), FontPool::Instance()->GetFont(m_buttonParam.textSize, FontCenterX::Center, FontCenterY::Center).get());
}
