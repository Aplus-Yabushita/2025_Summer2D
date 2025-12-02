#include "MainMenu.h"

#include "../../Direct2DLib.h"

#include "../../Page/PageManager.h"

#include "../../Font/FontPool.h"
#include "../../Input/Input.h"

// 関数 Segment_Segment_Analyse の結果を受け取る為の構造体
struct SEGMENT_SEGMENT_RESULT {
	float					SegA_SegB_MinDist_Square;		// 線分Ａと線分Ｂが最も接近する座標間の距離の二乗
	float					SegA_MinDist_Pos1_Pos2_t;		// 線分Ａと線分Ｂに最も接近する座標の線分Ａの t ( 0.0f ～ 1.0f 、最近点座標 = ( SegAPos2 - SegAPos1 ) * t + SegAPos1 )
	float					SegB_MinDist_Pos1_Pos2_t;		// 線分Ｂが線分Ａに最も接近する座標の線分Ｂの t ( 0.0f ～ 1.0f 、最近点座標 = ( SegBPos2 - SegBPos1 ) * t + SegBPos1 )
	Algorithm::Vector3DX					SegA_MinDist_Pos;				// 線分Ａが線分Ｂに最も接近する線分Ａ上の座標
	Algorithm::Vector3DX					SegB_MinDist_Pos;				// 線分Ｂが線分Ａに最も接近する線分Ｂ上の座標
};
// 関数 Segment_Point_Analyse の結果を受け取る為の構造体
struct SEGMENT_POINT_RESULT {
	float					Seg_Point_MinDist_Square;		// 線分と点が最も接近する座標間の距離の二乗
	float					Seg_MinDist_Pos1_Pos2_t;		// 線分が点に最も接近する座標の線分の t ( 0.0f ～ 1.0f 、最近点座標 = ( SegPos2 - SegPos1 ) * t + SegPos1 )
	Algorithm::Vector3DX	Seg_MinDist_Pos;				// 線分が点に最も接近する線分上の座標
};

void Segment_Point_Analyse(const Algorithm::Vector3DX& SegmentPos1, const Algorithm::Vector3DX& SegmentPos2, const Algorithm::Vector3DX& PointPos, SEGMENT_POINT_RESULT* Result)
{
	Algorithm::Vector3DX Seg1_2 = SegmentPos2 - SegmentPos1;
	float DotSeg1_2_Pnt = Algorithm::Vector3DX::VDot(Seg1_2, PointPos - SegmentPos1);
	float SizeSquSeg1_2 = Seg1_2.VSquareSize();
	if (DotSeg1_2_Pnt <= 0.0f) {
		Result->Seg_MinDist_Pos1_Pos2_t = 0.0f;
		Result->Seg_MinDist_Pos = SegmentPos1;
	}
	else if (SizeSquSeg1_2 == 0.0f) {
		Result->Seg_MinDist_Pos1_Pos2_t = 0.0f;
		Result->Seg_MinDist_Pos = SegmentPos1;
	}
	else if (SizeSquSeg1_2 < DotSeg1_2_Pnt) {
		Result->Seg_MinDist_Pos1_Pos2_t = 1.0f;
		Result->Seg_MinDist_Pos = SegmentPos2;
	}
	else {
		Result->Seg_MinDist_Pos1_Pos2_t = DotSeg1_2_Pnt / SizeSquSeg1_2;
		Result->Seg_MinDist_Pos = Seg1_2 * Result->Seg_MinDist_Pos1_Pos2_t + SegmentPos1;
	}
	Result->Seg_Point_MinDist_Square = (PointPos - Result->Seg_MinDist_Pos).VSquareSize();
}

void Segment_Segment_Analyse(const Algorithm::Vector3DX& SegmentAPos1, const Algorithm::Vector3DX& SegmentAPos2, const Algorithm::Vector3DX& SegmentBPos1, const Algorithm::Vector3DX& SegmentBPos2, SEGMENT_SEGMENT_RESULT* Result)
{
	float t = 0.0f;

	Algorithm::Vector3DX segA1_2 = SegmentAPos2 - SegmentAPos1;
	Algorithm::Vector3DX segB1_2 = SegmentBPos2 - SegmentBPos1;
	Algorithm::Vector3DX segB1_A1 = SegmentAPos1 - SegmentBPos1;
	float a = Algorithm::Vector3DX::VDot(segA1_2, segA1_2);
	float b = -Algorithm::Vector3DX::VDot(segA1_2, segB1_2);
	float c = -b;
	float d = -Algorithm::Vector3DX::VDot(segB1_2, segB1_2);
	float e = -Algorithm::Vector3DX::VDot(segA1_2, segB1_A1);
	float f = -Algorithm::Vector3DX::VDot(segB1_2, segB1_A1);

	// SegmentAPos1 - SegmentAPos2 または SegmentBPos1 - SegmentBPos2 の距離が限りなくゼロに近いかどうかのチェック
	float tmpA = a < 0.0f ? -a : a;
	float tmpB = d < 0.0f ? -d : d;
	if (tmpA < 0.00000001f)
	{
		Result->SegA_MinDist_Pos1_Pos2_t = 0.0f;
		Result->SegA_MinDist_Pos = SegmentAPos1;
		if (tmpB < 0.00000001f)
		{
			Result->SegB_MinDist_Pos1_Pos2_t = 0.0f;
			Result->SegB_MinDist_Pos = SegmentBPos1;
			Result->SegA_SegB_MinDist_Square = segB1_A1.VSquareSize();
		}
		else
		{
			SEGMENT_POINT_RESULT spres;
			Segment_Point_Analyse(SegmentBPos1, SegmentBPos2, SegmentAPos1, &spres);
			Result->SegB_MinDist_Pos1_Pos2_t = spres.Seg_MinDist_Pos1_Pos2_t;
			Result->SegB_MinDist_Pos = spres.Seg_MinDist_Pos;
			Result->SegA_SegB_MinDist_Square = spres.Seg_Point_MinDist_Square;
		}
		return;
	}
	else
		if (tmpB < 0.00000001f)
		{
			Result->SegB_MinDist_Pos1_Pos2_t = 0.0f;
			Result->SegB_MinDist_Pos = SegmentBPos1;

			SEGMENT_POINT_RESULT spres;
			Segment_Point_Analyse(SegmentAPos1, SegmentAPos2, SegmentBPos1, &spres);
			Result->SegA_MinDist_Pos1_Pos2_t = spres.Seg_MinDist_Pos1_Pos2_t;
			Result->SegA_MinDist_Pos = spres.Seg_MinDist_Pos;
			Result->SegA_SegB_MinDist_Square = spres.Seg_Point_MinDist_Square;

			return;
		}

	float s = -0.1f;
	float w = a * d - b * c;
	if (w <= -0.00000001f)
	{
		// クラーメルの公式
		s = (e * d - b * f) / w;
		t = (a * f - e * c) / w;
	}

	// 二つの線分が縮退していたら点と見なして点同士の距離を返す
	if (a <= 0.0f && -d <= 0.0f)
	{
		Result->SegA_MinDist_Pos1_Pos2_t = 0.0f;
		Result->SegB_MinDist_Pos1_Pos2_t = 0.0f;
		Result->SegA_MinDist_Pos = SegmentAPos1;
		Result->SegB_MinDist_Pos = SegmentBPos1;
		Result->SegA_SegB_MinDist_Square = segB1_A1.VSquareSize();

		return;
	}

	if (a <= 0.0f)
	{
		s = 0.0f;
		if (t > 1.0f || t < 0.0f)
			t = t > 1.0f ? 1.0f : 0.0f;
	}
	else if (-d <= 0.0f)
	{
		t = 0.0f;
		if (s > 1.0f || s < 0.0f)
			s = s > 1.0f ? 1.0f : 0.0f;
	}
	else
	{
		if (s > 1.0f || s < 0.0f)
		{
			s = s > 1.0f ? 1.0f : 0.0f;

			t = (-b * s + -f) / -d;		// ガウス消去法
			if (t > 1.0f || t < 0.0f)
			{
				t = t > 1.0f ? 1.0f : 0.0f;
				s = (-b * t - -e) / a;	// ガウス消去法
				if (s > 1.0f) s = 1.0f;
				else if (s < 0.0f) s = 0.0f;
			}
		}
		else if (t > 1.0f || t < 0.0f)
		{
			t = t > 1.0f ? 1.0f : 0.0f;
			s = (-b * t - -e) / a;		// ガウス消去法

			if (s > 1.0f || s < 0.0f)
			{
				s = s > 1.0f ? 1.0f : 0.0f;
				t = (-b * s + -f) / -d;	// ガウス消去法
				if (t > 1.0f) t = 1.0f;
				else if (t < 0.0f) t = 0.0f;
			}
		}
	}

	Result->SegA_MinDist_Pos1_Pos2_t = s;
	Result->SegB_MinDist_Pos1_Pos2_t = t;

	Result->SegA_MinDist_Pos = segA1_2 * s;
	Result->SegA_MinDist_Pos = Result->SegA_MinDist_Pos + SegmentAPos1;

	Result->SegB_MinDist_Pos = segB1_2 * t;
	Result->SegB_MinDist_Pos = Result->SegB_MinDist_Pos + SegmentBPos1;

	Result->SegA_SegB_MinDist_Square = (Result->SegA_MinDist_Pos - Result->SegB_MinDist_Pos).VSquareSize();

	return;
}

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
			if (true) {

			}
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
