#pragma once

#include "../PageParent.h"
#include "../../UI/Button/UIButton.h"

namespace Algorithm {
	struct Vector3D {
		float x;
		float y;
		float z;
	public:
		Vector3D operator+(const Vector3D& o) const noexcept {
			Vector3D Answer{};
			Answer.x = this->x + o.x;
			Answer.y = this->y + o.y;
			Answer.z = this->z + o.z;
			return Answer;
		}
		Vector3D operator-(const Vector3D& o) const noexcept {
			Vector3D Answer{};
			Answer.x = this->x - o.x;
			Answer.y = this->y - o.y;
			Answer.z = this->z - o.z;
			return Answer;
		}
		Vector3D operator*(float per) const noexcept {
			Vector3D Answer{};
			Answer.x = this->x * per;
			Answer.y = this->y * per;
			Answer.z = this->z * per;
			return Answer;
		}
	public:
		// ベクトルのサイズの２乗
		inline float		VSquareSize() const {
			return this->x * this->x + this->y * this->y + this->z * this->z;
		}
		// ベクトルのサイズの２乗
		inline float		VSize() const {
			return std::sqrt(VSquareSize());
		}
		// ベクトルの正規化
		Vector3D VNorm() const {
			float Square = VSquareSize();
			if (Square < 0.0000001f)
			{
				Vector3D Answer{};
				Answer.x = 0.f;
				Answer.y = 0.f;
				Answer.z = 0.f;
				return Answer;
			}
			return *this * (1.f / std::sqrt(Square));
		}
		// ベクトルの内積
		static float		VDot(const Vector3D& In1, const Vector3D& In2) {
			return In1.x * In2.x + In1.y * In2.y + In1.z * In2.z;
		}
		// ベクトルの外積
		static Vector3D		VCross(const Vector3D& In1, const Vector3D& In2) {
			Vector3D Result{};
			Result.x = In1.y * In2.z - In1.z * In2.y;
			Result.y = In1.z * In2.x - In1.x * In2.z;
			Result.z = In1.x * In2.y - In1.y * In2.x;
			return Result;
		}
	};
}

struct RigidBodyMoveData {
	Algorithm::Vector3D	Pos{};
	Algorithm::Vector3D	Vec{};
	float					Radius{};
	float					CoefficientofRestitution{};
};

struct FixedLineData {
	Algorithm::Vector3D	Pos1{};
	Algorithm::Vector3D	Pos2{};
	float					Width{};
};

class MainMenu :public PageParent {
	std::vector<RigidBodyMoveData> RigidBody;
	std::array<FixedLineData, 7> FixedLine;
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
