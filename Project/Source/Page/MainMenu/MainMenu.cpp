#include "MainMenu.h"

#include "../../Direct2DLib.h"

#include "../../Page/PageManager.h"

#include "../../Font/FontPool.h"
#include "../../Input/Input.h"

void MainMenu::Init() {
	for (int loop = 0; loop < RigidBody.size(); ++loop) {
		RigidBody[loop].Pos.x = 10.f + loop * 10.f;
		RigidBody[loop].Pos.y = 10.f;
		RigidBody[loop].Vec.x = 0.f;
		RigidBody[loop].Vec.y = 0.f;
		RigidBody[loop].Radius = 1.f;
	}
	FixedLine[0].Pos1.x = 5.f;
	FixedLine[0].Pos1.y = 20.f;
	FixedLine[0].Pos2.x = 30.f;
	FixedLine[0].Pos2.y = 30.f;
	FixedLine[0].Width = 1.f;

	FixedLine[1].Pos1.x = 40.f;
	FixedLine[1].Pos1.y = 20.f;
	FixedLine[1].Pos2.x = 15.f;
	FixedLine[1].Pos2.y = 30.f;
	FixedLine[1].Width = 1.f;
}

void MainMenu::Update() {

	auto GetID = [&]() {
		return 0;
		};
	//*
	if (Input::Instance()->GetKeyPress(VK_DOWN)) {
		RigidBody[GetID()].Vec.y = 1.f;
	}
	if (Input::Instance()->GetKeyPress(VK_UP)) {
		RigidBody[GetID()].Vec.y = -1.f;
	}
	if (Input::Instance()->GetKeyPress(VK_LEFT)) {
		RigidBody[GetID()].Vec.x -= 1.f;
	}
	if (Input::Instance()->GetKeyPress(VK_RIGHT)) {
		RigidBody[GetID()].Vec.x += 1.f;
	}
	//*/
	RigidBody[GetID()].Vec.y += 9.8f;

	auto Target = RigidBody[GetID()].Pos + RigidBody[GetID()].Vec * ((1000.f / 60.f) / (60.f * 60.f));
	{
		//壁判定//TODO
		for (auto& f : FixedLine) {
		}
	}
	RigidBody[GetID()].Pos = Target;
}

void MainMenu::Draw() {
	Direct2DLib::Instance()->GetDrawSystem()->SetClearScreen(D2D1::ColorF(D2D1::ColorF::SkyBlue));

	Direct2DLib::Instance()->GetDrawSystem()->SetFont(
		4.0f, 4.0f,
		256.0f, 256.0f,
		L"メインページ",
		D2D1::ColorF(D2D1::ColorF::Black), FontPool::Instance()->GetFont(25, FontCenterX::Left, FontCenterY::Top).get());

	float scale = 10.f;

	for (auto& r : RigidBody) {
		Direct2DLib::Instance()->GetDrawSystem()->SetEllipse(r.Pos.x * scale, r.Pos.y * scale, r.Radius * scale, r.Radius * scale,
			D2D1::ColorF(D2D1::ColorF::Green));
	}
	for (auto& f : FixedLine) {
		Direct2DLib::Instance()->GetDrawSystem()->SetLine(
			f.Pos1.x * scale, f.Pos1.y * scale,
			f.Pos2.x * scale, f.Pos2.y * scale,
			f.Width * scale,
			D2D1::ColorF(D2D1::ColorF::Red)
		);
	}
}

void MainMenu::Dispose() {
}
