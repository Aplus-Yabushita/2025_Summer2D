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
	Algorithm::Vector3D					SegA_MinDist_Pos;				// 線分Ａが線分Ｂに最も接近する線分Ａ上の座標
	Algorithm::Vector3D					SegB_MinDist_Pos;				// 線分Ｂが線分Ａに最も接近する線分Ｂ上の座標
};
// 関数 Segment_Point_Analyse の結果を受け取る為の構造体
struct SEGMENT_POINT_RESULT {
	float					Seg_Point_MinDist_Square;		// 線分と点が最も接近する座標間の距離の二乗
	float					Seg_MinDist_Pos1_Pos2_t;		// 線分が点に最も接近する座標の線分の t ( 0.0f ～ 1.0f 、最近点座標 = ( SegPos2 - SegPos1 ) * t + SegPos1 )
	Algorithm::Vector3D	Seg_MinDist_Pos;				// 線分が点に最も接近する線分上の座標
};

static void Segment_Point_Analyse(const Algorithm::Vector3D& SegmentPos1, const Algorithm::Vector3D& SegmentPos2, const Algorithm::Vector3D& PointPos, SEGMENT_POINT_RESULT* Result)
{
	Algorithm::Vector3D Seg1_2 = SegmentPos2 - SegmentPos1;
	float DotSeg1_2_Pnt = Algorithm::Vector3D::VDot(Seg1_2, PointPos - SegmentPos1);
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

	//todo:物理挙動のために線分でなく直線の直交座標を求めている
	Result->Seg_MinDist_Pos1_Pos2_t = DotSeg1_2_Pnt / SizeSquSeg1_2;
	Result->Seg_MinDist_Pos = Seg1_2 * Result->Seg_MinDist_Pos1_Pos2_t + SegmentPos1;
}

static void Segment_Segment_Analyse(const Algorithm::Vector3D& SegmentAPos1, const Algorithm::Vector3D& SegmentAPos2, const Algorithm::Vector3D& SegmentBPos1, const Algorithm::Vector3D& SegmentBPos2, SEGMENT_SEGMENT_RESULT* Result)
{
	float t = 0.f;

	Algorithm::Vector3D segA1_2 = SegmentAPos2 - SegmentAPos1;
	Algorithm::Vector3D segB1_2 = SegmentBPos2 - SegmentBPos1;
	Algorithm::Vector3D segB1_A1 = SegmentAPos1 - SegmentBPos1;
	float a = Algorithm::Vector3D::VDot(segA1_2, segA1_2);
	float b = -Algorithm::Vector3D::VDot(segA1_2, segB1_2);
	float c = -b;
	float d = -Algorithm::Vector3D::VDot(segB1_2, segB1_2);
	float e = -Algorithm::Vector3D::VDot(segA1_2, segB1_A1);
	float f = -Algorithm::Vector3D::VDot(segB1_2, segB1_A1);

	// SegmentAPos1 - SegmentAPos2 または SegmentBPos1 - SegmentBPos2 の距離が限りなくゼロに近いかどうかのチェック
	float tmpA = a < 0.f ? -a : a;
	float tmpB = d < 0.f ? -d : d;
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



//Z==0を前提とした交点を求める
static bool Intersection2D(const Algorithm::Vector3D& a, const Algorithm::Vector3D& b, const Algorithm::Vector3D& c, const Algorithm::Vector3D& d, Algorithm::Vector3D* ans) {
	float deno = Algorithm::Vector3D::VCross(b - a, d - c).z;
	if (deno == 0.f) {
		// 線分が平行
		return false;
	}
	float s = Algorithm::Vector3D::VCross(c - a, d - c).z / deno;
	//線分の場合は必須
	/*
	float t = Algorithm::Vector3D::VCross(b - a, a - c).z / deno;
	if (s < 0.f || 1.f < s || t < 0.f || 1.f < t) {
		// 線分が交差していない
		return false;
	}
	//*/
	*ans = a + (b - a) * s;
	return true;
}

//
static Algorithm::Vector3D calcLineNormal(const Algorithm::Vector3D& a, const Algorithm::Vector3D& b) {
	auto dirVec = Algorithm::Vector3D(a.x - b.x, a.y - b.y, 0.f);
	auto normal = Algorithm::Vector3D(-dirVec.y, dirVec.x, 0.f);
	return normal.VNorm();
}

void MainMenu::Init() {
	float width = 8.f;
	RigidBody.resize(2);
	for (int loop = 0; loop < RigidBody.size(); ++loop) {
		RigidBody[loop].Pos.x = 15.f + (loop % static_cast<int>(20 / width)) * width;
		RigidBody[loop].Pos.y = 10.f + (loop / static_cast<int>(20 / width)) * width;
		RigidBody[loop].Pos.z = 0.f;
		RigidBody[loop].Vec.x = 0.f;
		RigidBody[loop].Vec.y = 0.f;
		RigidBody[loop].Vec.z = 0.f;
		RigidBody[loop].Radius = (width - 1.f) / 2.f;
		RigidBody[loop].CoefficientOfRestitution = 1.f;
	}

	FixedLine[0].Pos1.x = -1.f;
	FixedLine[0].Pos1.y = 0.f;
	FixedLine[0].Pos2.x = -1.f;
	FixedLine[0].Pos2.y = 48.f;
	FixedLine[0].Width = 1.f;

	FixedLine[1].Pos1.x = 65.f;
	FixedLine[1].Pos1.y = 48.f;
	FixedLine[1].Pos2.x = 65.f;
	FixedLine[1].Pos2.y = 0.f;
	FixedLine[1].Width = 1.f;

	FixedLine[2].Pos1.x = 64.f;
	FixedLine[2].Pos1.y = -1.f;
	FixedLine[2].Pos2.x = 0.f;
	FixedLine[2].Pos2.y = -1.f;
	FixedLine[2].Width = 1.f;

	FixedLine[3].Pos1.x = 0.f;
	FixedLine[3].Pos1.y = 49.f;
	FixedLine[3].Pos2.x = 64.f;
	FixedLine[3].Pos2.y = 49.f;
	FixedLine[3].Width = 1.f;

	FixedLine[4].Pos1.x = 5.f;
	FixedLine[4].Pos1.y = 20.f;
	FixedLine[4].Pos2.x = 30.f;
	FixedLine[4].Pos2.y = 40.f;
	FixedLine[4].Width = 1.f;

	FixedLine[5].Pos1.x = 15.f;
	FixedLine[5].Pos1.y = 40.f;
	FixedLine[5].Pos2.x = 40.f;
	FixedLine[5].Pos2.y = 20.f;
	FixedLine[5].Width = 1.f;


	FixedLine[6].Pos1.x = 10.f;
	FixedLine[6].Pos1.y = 10.f;
	FixedLine[6].Pos2.x = 10.f;
	FixedLine[6].Pos2.y = 35.f;
	FixedLine[6].Width = 1.f;

	FixedLine[7].Pos1.x = 35.f;
	FixedLine[7].Pos1.y = 35.f;
	FixedLine[7].Pos2.x = 35.f;
	FixedLine[7].Pos2.y = 10.f;
	FixedLine[7].Width = 1.f;

	FixedLine[8].Pos1.x = 1.f;
	FixedLine[8].Pos1.y = 1.f;
	FixedLine[8].Pos2.x = 1.f;
	FixedLine[8].Pos2.y = 45.f;
	FixedLine[8].Width = 1.f;

	FixedLine[9].Pos1.x = 50.f;
	FixedLine[9].Pos1.y = 45.f;
	FixedLine[9].Pos2.x = 50.f;
	FixedLine[9].Pos2.y = 1.f;
	FixedLine[9].Width = 1.f;

	FixedLine[10].Pos1.x = 1.f;
	FixedLine[10].Pos1.y = 45.f;
	FixedLine[10].Pos2.x = 50.f;
	FixedLine[10].Pos2.y = 45.f;
	FixedLine[10].Width = 1.f;
}

void MainMenu::Update() {
	for (auto& R1 : RigidBody) {
		size_t index = &R1 - &RigidBody.front();
		if (index == 0) {
			if (Input::Instance()->GetKeyPress(VK_DOWN)) {
				R1.Vec.y += 20.f;
			}
			if (Input::Instance()->GetKeyPress(VK_UP)) {
				R1.Vec.y -= 20.f;
			}
			if (Input::Instance()->GetKeyPress(VK_LEFT)) {
				R1.Vec.x -= 20.f;
			}
			if (Input::Instance()->GetKeyPress(VK_RIGHT)) {
				R1.Vec.x += 20.f;
			}
		}
		R1.Vec.y += 9.8f;

		auto Before = R1.Pos;
		auto After = Before + R1.Vec * ((1000.f / 60.f) / (60.f * 60.f));
		{
			//壁判定
			for (auto& f : FixedLine) {
				SEGMENT_SEGMENT_RESULT res;
				Segment_Segment_Analyse(f.Pos1, f.Pos2, Before, After, &res);
				float Radius = (f.Width / 2.f + R1.Radius);
				if (res.SegA_SegB_MinDist_Square < Radius * Radius) {
					Algorithm::Vector3D CrossPoint;
					if (Intersection2D(f.Pos1, f.Pos2, Before, After, &CrossPoint)) {
						auto V1 = (f.Pos2 - f.Pos1).VNorm();
						auto VectorR = (After - Before).VNorm();
						auto normal = calcLineNormal(f.Pos1, f.Pos2);
						if (Algorithm::Vector3D::VCross(VectorR, V1).z > 0.001f) {
							continue;//反対面なので無視
						}

						Before = CrossPoint - VectorR * (Radius / Algorithm::Vector3D::VCross(V1, VectorR).z);
						After = After + (normal * ((1.f + R1.CoefficientOfRestitution) * Algorithm::Vector3D::VDot((After - Before) * -1.f, normal)));
						if ((After - Before).VSize() < 0.001f) { break; }
					}
				}
			}
			//相互判定
			for (auto& R2 : RigidBody) {
				if (&R1 == &R2) { continue; }
				SEGMENT_POINT_RESULT res;
				Segment_Point_Analyse(Before, After, R2.Pos, &res);
				float Radius = (R1.Radius + R2.Radius);
				if (res.Seg_Point_MinDist_Square <= Radius * Radius) {
					auto VectorR = (After - Before).VNorm();
					auto VectorRSize = (After - Before).VSize();
					if (VectorRSize < 0.0000001f) { continue; }
					auto MinPosToR2PosLength = std::min((res.Seg_MinDist_Pos - R2.Pos).VSize(), Radius);

					Before = res.Seg_MinDist_Pos - VectorR * (Radius * sqrtf(1.f - powf(MinPosToR2PosLength / Radius, 2.f)));//接触座標

					auto normal = (Before - R2.Pos).VNorm();

					After = After + (normal * (Algorithm::Vector3D::VDot((After - Before) * -1.f, normal)));//ずりベクトルでの移動後の座標

					//After = Before;
					if ((After - Before).VSize() < 0.001f) { break; }
				}
			}
		}
		float keepsize = R1.Vec.VSize();
		R1.Vec = (After - Before) * (1.f / ((1000.f / 60.f) / (60.f * 60.f)));
		//R1.Vec = R1.Vec.VNorm() * std::min(R1.Vec.VSize(), keepsize);
		After = Before + R1.Vec * ((1000.f / 60.f) / (60.f * 60.f));
		R1.Pos = After;
	}
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
		Direct2DLib::Instance()->GetDrawSystem()->SetEllipse(static_cast<float>(r.Pos.x * scale), static_cast<float>(r.Pos.y * scale), static_cast<float>(r.Radius * scale), static_cast<float>(r.Radius * scale + 1.f),
			D2D1::ColorF(D2D1::ColorF::Black));
		Direct2DLib::Instance()->GetDrawSystem()->SetEllipse(static_cast<float>(r.Pos.x * scale), static_cast<float>(r.Pos.y * scale), static_cast<float>(r.Radius * scale), static_cast<float>(r.Radius * scale),
			D2D1::ColorF(D2D1::ColorF::Green));
	}
	for (auto& f : FixedLine) {
		Direct2DLib::Instance()->GetDrawSystem()->SetLine(
			static_cast<float>(f.Pos1.x * scale), static_cast<float>(f.Pos1.y * scale),
			static_cast<float>(f.Pos2.x * scale), static_cast<float>(f.Pos2.y * scale),
			static_cast<float>(f.Width * scale),
			D2D1::ColorF(D2D1::ColorF::Red)
		);
	}
}

void MainMenu::Dispose() {
}
