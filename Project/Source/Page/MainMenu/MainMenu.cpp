#include "MainMenu.h"

#include "../../Direct2DLib.h"

#include "../../Page/PageManager.h"

#include "../../Font/FontPool.h"

void MainMenu::Init() {
	m_IsEndPage = false;
	m_Button.clear();
	m_Button.reserve(3);

	ButtonParam buttonParam;
	buttonParam.xmin = 640 / 2 - 128;
	buttonParam.ymin = 300;
	buttonParam.xmax = buttonParam.xmin + 256;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(0.7f, 0.7f, 0.7f);
	buttonParam.m_ButtonText = L"インベントリ画面";
	buttonParam.m_PressEvent = [&] {
		m_IsEndPage = true;
		SetNextPageID(0);
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);

	buttonParam.xmin = 640 / 2 - 128;
	buttonParam.ymin = 300 + 80;
	buttonParam.xmax = buttonParam.xmin + 256;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(0.7f, 0.7f, 0.7f);
	buttonParam.m_ButtonText = L"ショップ画面";
	buttonParam.m_PressEvent = [&] {
		m_IsEndPage = true;
		SetNextPageID(1);
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);

	buttonParam.xmin = 640 - 82 - 4;
	buttonParam.ymin = 4;
	buttonParam.xmax = buttonParam.xmin + 82;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(1.0f, 0.0f, 0.0f);
	buttonParam.m_ButtonText = L"終了";
	buttonParam.m_PressEvent = [&] {
		PageManager::Instance()->SetEnd();
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);
}

void MainMenu::Update() {
	for (auto& b : m_Button) {
		b.Update();
	}
}

void MainMenu::Draw() {
	Direct2DLib::Instance()->GetDrawSystem()->SetClearScreen(D2D1::ColorF(D2D1::ColorF::SkyBlue));

	Direct2DLib::Instance()->GetDrawSystem()->SetFont(
		4.0f, 4.0f,
		256.0f, 256.0f,
		L"メインメニュー",
		D2D1::ColorF(D2D1::ColorF::Black), FontPool::Instance()->GetFont(25, FontCenterX::Left, FontCenterY::Top).get());

	for (auto& b : m_Button) {
		b.Draw();
	}
}

void MainMenu::Dispose() {
	m_Button.clear();
}
