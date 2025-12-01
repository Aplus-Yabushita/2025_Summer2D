#pragma once

#include "../PageParent.h"
#include "../../UI/Button/UIButton.h"
#include "../../Network/Net.hpp"

struct RigidBodyMoveData {
	Algorithm::Vector3DX	Pos{};
	Algorithm::Vector3DX	Vec{};
	float					Radius{};
};

struct FixedLineData {
	Algorithm::Vector3DX	Pos1{};
	Algorithm::Vector3DX	Pos2{};
	float					Width{};
};

class MainMenu :public PageParent {
	std::array<RigidBodyMoveData, 2> RigidBody;
	std::array<FixedLineData, 2> FixedLine;
public:
	MainMenu(void) noexcept {}
	MainMenu(const MainMenu&) {}
	MainMenu(MainMenu&&) noexcept {}
	MainMenu& operator=(const MainMenu&) {}
	MainMenu& operator=(MainMenu&&) noexcept {}
	virtual ~MainMenu(void) noexcept {}
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEndPage() override { return false; }
	void Dispose()  override;
};
