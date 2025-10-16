#include	"NetBrowser.hpp"
#include "../Font/FontPool.h"
#include "../Input/Input.h"

const NetWork::NetWorkBrowser* SingletonBase<NetWork::NetWorkBrowser>::m_Singleton = nullptr;

namespace NetWork {

	//表示物設定
	static bool SetMsgClickBox(int xmin, int ymin, int xmax, int ymax, int fontsize, D2D1::ColorF Color, bool isloop, const wchar_t* Text) {
		bool OnMouse = HitPointToRectangle(
			static_cast<int>(Input::Instance()->GetMousePosition().x), static_cast<int>(Input::Instance()->GetMousePosition().y),
			xmin, ymin, xmax, ymax
		);

		Direct2DLib::Instance()->GetDrawSystem()->SetRect(static_cast<float>(xmin), static_cast<float>(ymin), static_cast<float>(xmax), static_cast<float>(ymax),
			OnMouse ? D2D1::ColorF(D2D1::ColorF::White) : Color);
		Direct2DLib::Instance()->GetDrawSystem()->SetFont(static_cast<float>(xmin), static_cast<float>(ymin), static_cast<float>(xmax), static_cast<float>(ymax),
			Text,
			D2D1::ColorF(D2D1::ColorF::Black), FontPool::Instance()->GetFont(fontsize, FontCenterX::Center, FontCenterY::Center).get());


		bool Trigger = OnMouse && Input::Instance()->GetKeyTrigger(VK_LBUTTON);
		bool Press = OnMouse && Input::Instance()->GetKeyPress(VK_LBUTTON);

		return Trigger;
	}

	static void SetString(int FintSize, FontCenterX FontX, FontCenterY FontY, int xmin, int ymin, int xmax, int ymax,
		D2D1::ColorF Color,
		const wchar_t* Text) {
		Direct2DLib::Instance()->GetDrawSystem()->SetFont(
			static_cast<float>(xmin), static_cast<float>(ymin), static_cast<float>(xmax), static_cast<float>(ymax),
			Text,
			Color, FontPool::Instance()->GetFont(FintSize, FontX, FontY).get());
	}

	void NetWorkBrowser::Draw(void) noexcept {
		int xp, yp, xs, ys;
		xp = 0;
		yp = 0;
		xs = 300;
		ys = 300;

		//ラムダ
		auto AddSubBox = [&](int xp, int yp, std::function<void()> UpFunc, std::function<void()> DownFunc) {
			int xp1, yp1;
			int xp2, yp2;
			{
				xp1 = xp - 25;
				yp1 = yp - 25 - 18 / 2;
				xp2 = xp + 25;
				yp2 = yp1 + 18;
				if (SetMsgClickBox(xp1, yp1, xp2, yp2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), true, L"+")) {
					UpFunc();
				}
			}
			{
				yp1 += 50;
				yp2 += 50;
				if (SetMsgClickBox(xp1, yp1, xp2, yp2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), true, L"-")) {
					DownFunc();
				}
			}
			};
		//
		{
			wchar_t Text[256] = L"";
			swprintf_s(Text, L" %d/%d", static_cast<int>(this->m_Sequence), static_cast<int>(BrowserSequence::Ready));

			Direct2DLib::Instance()->GetDrawSystem()->SetRect(static_cast<float>(xp - 10), static_cast<float>(yp - 10), static_cast<float>(xp + xs + 10), static_cast<float>(yp + ys + 10), D2D1::ColorF(0.25f, 0.25f, 0.25f));//背景
			SetString(18,
				FontCenterX::Right, FontCenterY::Top, xp + xs - 255, yp, xp + xs, yp + 64, D2D1::ColorF(D2D1::ColorF::White),
				Text);
		}
		auto Prev = this->m_Sequence;

		int y1p = yp + 50;
		switch (this->m_Sequence) {
		case BrowserSequence::SelectMode:
			if (SetMsgClickBox(xp, y1p, xp + xs, y1p + 18 * 2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), false, L"クライアントになる")) {
				BeClient();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			if (SetMsgClickBox(xp, y1p + 50, xp + xs, y1p + 50 + 18 * 2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), false, L"サーバーになる")) {
				BeServerPlayer();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			if (SetMsgClickBox(xp, y1p + 100, xp + xs, y1p + 100 + 18 * 2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), false, L"サーバーになる(非プレイヤー)")) {
				BeServer();
				this->m_Sequence = BrowserSequence::CheckPreset;
			}
			//
			if (SetMsgClickBox(xp, y1p + 150, xp + xs, y1p + 150 + 18 * 2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), false, L"クライアントで即プレイ")) {
				BeClient();
				this->m_NewWorkSettings.Load();
				ReadyConnect(this->m_NewWorkSettings.Get(0));
			}
			if (SetMsgClickBox(xp, y1p + 200, xp + xs, y1p + 200 + 18 * 2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), false, L"サーバーで即プレイ")) {
				BeServerPlayer();
				this->m_NewWorkSettings.Load();
				ReadyConnect(this->m_NewWorkSettings.Get(0));
			}
			break;
		case BrowserSequence::CheckPreset:
			SetString(18,
				FontCenterX::Left, FontCenterY::Top, xp, yp, xp + 255, yp + 64, D2D1::ColorF(D2D1::ColorF::White), L"プリセット設定");
			if (this->m_SeqFirst) {
				this->m_NewWorkSettings.Load();
			}
			for (int loop = 0, Num = this->m_NewWorkSettings.GetSize(); loop < Num + 1; ++loop) {
				if (loop < Num) {
					auto& netset = this->m_NewWorkSettings.Get(loop);

					wchar_t Text[256] = L"";
					swprintf_s(Text, L"[%d][%d,%d,%d,%d]", netset.UsePort, netset.IP.d1, netset.IP.d2, netset.IP.d3, netset.IP.d4);

					if (SetMsgClickBox(xp, y1p + 50 * loop, xp + xs, y1p + 50 * loop + 18 * 2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), false, Text)) {
						ReadyConnect(netset);
						break;
					}
				}
				else {
					if (SetMsgClickBox(xp, y1p + 50 * loop, xp + xs, y1p + 50 * loop + 18 * 2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), false, L"設定を追加する")) {
						this->m_Sequence = BrowserSequence::SetNewData;
						break;
					}
				}
			}
			break;
		case BrowserSequence::SetNewData:
			SetString(18,
				FontCenterX::Left, FontCenterY::Top, xp, yp, xp + 255, yp + 64, D2D1::ColorF(D2D1::ColorF::White), L"新規設定");
			if (this->m_SeqFirst) {
				this->m_NewWorkSettings.AddBack();
			}
			{
				wchar_t Text[256] = L"";
				swprintf_s(Text, L"ポート=[%d-%d]", this->m_NetSetting.UsePort, this->m_NetSetting.UsePort + NetWork::Player_num - 1);

				SetString(18,
					FontCenterX::Center, FontCenterY::Top, xp, y1p, xp + xs, y1p+64, D2D1::ColorF(D2D1::ColorF::White), Text);
				AddSubBox(xp + xs / 2, y1p + 18 / 2, [&]() { ++this->m_NetSetting.UsePort; }, [&]() { --this->m_NetSetting.UsePort; });
			}
			if (!this->m_IsServer) {//サーバ-はいらない
				int yp1 = y1p + 100;

				wchar_t Text[256] = L"";
				swprintf_s(Text, L"IP=[%d,%d,%d,%d]", this->m_NetSetting.IP.d1, this->m_NetSetting.IP.d2, this->m_NetSetting.IP.d3, this->m_NetSetting.IP.d4);

				SetString(18,
					FontCenterX::Center, FontCenterY::Top, xp, yp1, xp + xs, yp1+64, D2D1::ColorF(D2D1::ColorF::White), Text);
				for (int loop = 0; loop < 4; ++loop) {
					auto* ip_tmp = &this->m_NetSetting.IP.d1;
					switch (loop) {
					case 0:ip_tmp = &this->m_NetSetting.IP.d1; break;
					case 1:ip_tmp = &this->m_NetSetting.IP.d2; break;
					case 2:ip_tmp = &this->m_NetSetting.IP.d3; break;
					case 3:ip_tmp = &this->m_NetSetting.IP.d4; break;
					}
					AddSubBox(xp + xs / 2 + 70 * (loop - 2) + 35, yp1 + 18 / 2,
						[&]() {
							if (*ip_tmp == 255) { *ip_tmp = 0; }
							else { ++(*ip_tmp); }
						}, [&]() {
							if (*ip_tmp == 0) { *ip_tmp = 255; }
							else { --(*ip_tmp); }
							});
				}
			}
			{
				int yp1 = y1p + 200;
				if (SetMsgClickBox(xp + xs - 120, yp1, xp + xs, yp1 + 18 * 2, 18, D2D1::ColorF(0.75f, 0.75f, 0.75f), false, L"Set")) {
					this->m_NewWorkSettings.SetBack(this->m_NetSetting);
					this->m_NewWorkSettings.Save();
					ReadyConnect(this->m_NewWorkSettings.Get(this->m_NewWorkSettings.GetSize() - 1));
				}
			}
			break;
		case BrowserSequence::Ready:
			SetString(18,
				FontCenterX::Left, FontCenterY::Top, xp, yp, xp + 255, yp + 64, D2D1::ColorF(D2D1::ColorF::White), L"通信中!");
			{
				int yp1 = y1p;
				SetString(18,
					FontCenterX::Left, FontCenterY::Top, xp, yp1, xp + 255, yp1 + 64, D2D1::ColorF(D2D1::ColorF::White), this->m_IsServer ? L"種別[サーバー]" : L"種別[クライアント]"); yp1 += 24;

				wchar_t Text[256] = L"";
				swprintf_s(Text, L"使用ポート[%d-%d]", this->m_NetSetting.UsePort, this->m_NetSetting.UsePort + NetWork::Player_num - 1);

				SetString(18,
					FontCenterX::Left, FontCenterY::Top, xp, yp1, xp + 255, yp1 + 64, D2D1::ColorF(D2D1::ColorF::White), Text); yp1 += 24;

				swprintf_s(Text, L"IP=[%d,%d,%d,%d]", this->m_NetSetting.IP.d1, this->m_NetSetting.IP.d2, this->m_NetSetting.IP.d3, this->m_NetSetting.IP.d4);

				SetString(18,
					FontCenterX::Left, FontCenterY::Top, xp, yp1, xp + 255, yp1 + 64, D2D1::ColorF(D2D1::ColorF::White), Text); yp1 += 24;
			}
			break;
		default:
			break;
		}
		this->m_SeqFirst = (Prev != this->m_Sequence);
	}
}
