#include "MainMenu.h"

#include "../../Direct2DLib.h"

#include "../../Page/PageManager.h"

#include "../../Font/FontPool.h"
#include "../../Input/Input.h"

void MainMenu::Init() {
	for (int loop = 0; loop < Player.size(); ++loop) {
		Player[loop].m_move.pos.x = 100.f + loop * 100.f;
		Player[loop].m_move.pos.y = 100.f;
		Player[loop].m_move.vec.x = 0.f;
		Player[loop].m_move.vec.x = 0.f;
		Player[loop].m_move.repos = Player[loop].m_move.pos;
	}
}

void MainMenu::Update() {

	auto GetID = [&]() {
		return 0;
		};

	Player[GetID()].m_move.vec.x = 0.f;
	Player[GetID()].m_move.vec.y = 0.f;
	Player[GetID()].m_move.vec.z = 0.f;

	Player[GetID()].m_Input.KeyFlag = 0;
	if (Input::Instance()->GetKeyPress(VK_DOWN)) {
		Player[GetID()].m_Input.KeyFlag |= 1 << 0;
		Player[GetID()].m_move.vec.y = 1.f;
	}
	if (Input::Instance()->GetKeyPress(VK_UP)) {
		Player[GetID()].m_Input.KeyFlag |= 1 << 1;
		Player[GetID()].m_move.vec.y = -1.f;
	}
	if (Input::Instance()->GetKeyPress(VK_LEFT)) {
		Player[GetID()].m_Input.KeyFlag |= 1 << 2;
		Player[GetID()].m_move.vec.x = -1.f;
	}
	if (Input::Instance()->GetKeyPress(VK_RIGHT)) {
		Player[GetID()].m_Input.KeyFlag |= 1 << 3;
		Player[GetID()].m_move.vec.x = 1.f;
	}
	Player[GetID()].m_move.repos = Player[GetID()].m_move.pos;
	Player[GetID()].m_move.pos = Player[GetID()].m_move.pos + Player[GetID()].m_move.vec * (1000.f / 60.f);
}

void MainMenu::Draw() {
	Direct2DLib::Instance()->GetDrawSystem()->SetClearScreen(D2D1::ColorF(D2D1::ColorF::SkyBlue));

	Direct2DLib::Instance()->GetDrawSystem()->SetFont(
		4.0f, 4.0f,
		256.0f, 256.0f,
		L"メインページ",
		D2D1::ColorF(D2D1::ColorF::Black), FontPool::Instance()->GetFont(25, FontCenterX::Left, FontCenterY::Top).get());

	for (int loop = 0; loop < Player.size(); ++loop) {
		Direct2DLib::Instance()->GetDrawSystem()->SetEllipse(Player[loop].m_move.pos.x, Player[loop].m_move.pos.y, 50.f, 50.f,
			(loop == 0) ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red));
	}
}

void MainMenu::Dispose() {
}
