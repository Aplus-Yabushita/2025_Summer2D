#include "Direct2DLib.h"
#include "Input/Input.h"
#include "Font/FontPool.h"
#include "Page/PageManager.h"

#include "Page/MainMenu/MainMenu.h"
#include "Page/Inventory/Inventory.h"
#include "Page/Shop/Shop.h"

#include "DataBase/ItemData/ItemDataPool.h"
#include "DataBase/ItemImage/ItemImagePool.h"

#include "CharacterData/PlayerData.h"

#include "Network/Net.hpp"
#include "Network/NetBrowser.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int  nCmdShow) {
	//シングルトンの作成
	Direct2DLib::Create();
	Input::Create();
	FontPool::Create();
	//ウィンドウの初期化
	if (!Direct2DLib::Instance()->Init("Winter2D", nCmdShow, 640, 480)) { return 0; }
	//シングルトンの作成
	ItemImagePool::Create();
	ItemDataPool::Create();
	PlayerData::Create();
	PageManager::Create();
	//データのロード
	if (!PlayerData::Instance()->Load()) {
		//初期データ配布
		PlayerData::Instance()->GetMyCharacterData()->AddItem(0, ItemDataPool::Instance()->Find(L"Yellar"), 1000);//金配り
	}
	//ページを定義(所有権は子のローカル変数が保持する)
	std::unique_ptr<PageParent> MainMenuPage = std::make_unique<MainMenu>();
	std::unique_ptr<PageParent> InventoryPage = std::make_unique<Inventory>();
	std::unique_ptr<PageParent> ShopPage = std::make_unique<Shop>();
	//各ページの次に移動する先を指定
	MainMenuPage->SetNextPage(0, &InventoryPage);
	MainMenuPage->SetNextPage(1, &ShopPage);
	InventoryPage->SetNextPage(0, &MainMenuPage);
	ShopPage->SetNextPage(0, &MainMenuPage);
	//最初に表示するページを指定
	PageManager::Instance()->SetFirstPage(&MainMenuPage);
	//ページ初期化
	PageManager::Instance()->Init();


	NetWork::NetWorkController* NetController = nullptr;
	NetWork::PlayerSendData Send;

	auto GetID = [&]() {
		if (NetController) {
			if (NetController->IsInGame()) {
				return NetController->GetMyLocalPlayerID();
			}
		}
		return 0;
	};

	NetWork::NetWorkBrowser::Create();

	struct PlayerMoveData {
		Algorithm::InputControl				m_Input{};
		Algorithm::MoveInfo					m_move{};
		int32_t						m_FreeData[10]{};
	};

	std::array<PlayerMoveData, NetWork::Player_num> Player;

	for (int loop = 0; loop < NetWork::Player_num; ++loop) {
		Player[loop].m_move.pos.x = 100.f + loop*100.f;
		Player[loop].m_move.pos.y = 100.f;
		Player[loop].m_move.vec.x = 0.f;
		Player[loop].m_move.vec.x = 0.f;
		Player[loop].m_move.repos = Player[loop].m_move.pos;
	}
	//メインループ
	while (true) {
		//入力の更新
		Input::Instance()->Update();
		//基本的なマウスカーソルを指定しておく
		HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
		SetCursor(hCursor);
		//ページの更新
		PageManager::Instance()->Update();
		PageManager::Instance()->Draw();
		NetWork::NetWorkBrowser::Instance()->Draw();

		{

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
			Send.SetMyPlayer(Player[GetID()].m_Input, Player[GetID()].m_move, Player[GetID()].m_FreeData);
		}
		if (NetWork::NetWorkBrowser::Instance()->IsDataReady()) {
			NetController = new NetWork::NetWorkController(
				NetWork::NetWorkBrowser::Instance()->IsServer(),
				NetWork::NetWorkBrowser::Instance()->GetNetSetting().UsePort,
				NetWork::NetWorkBrowser::Instance()->GetNetSetting().IP,
				NetWork::NetWorkBrowser::Instance()->GetServerPlayer());
		}
		if (NetController) {
			NetController->Update(Send);
			if (NetController->IsInGame()) {
				//サーバーからのデータを反映する
				for (int loop = 0; loop < NetWork::Player_num; ++loop) {
					const auto& Data = NetController->GetServerPlayerData(loop).GetPlayerSendData();
					Player[loop].m_Input = Data.GetInput();
					Player[loop].m_move = Data.GetMove();
					for (int loop2 = 0; loop2 < 10; ++loop2) {
						Player[loop].m_FreeData[loop2] = Data.GetFreeData()[loop2];
					}
				}
			}
		}

		for (int loop = 0; loop < NetWork::Player_num; ++loop) {
			Direct2DLib::Instance()->GetDrawSystem()->SetEllipse(Player[loop].m_move.pos.x, Player[loop].m_move.pos.y, 5.f, 5.f,
				(loop == 0) ? D2D1::ColorF(D2D1::ColorF::Green) : D2D1::ColorF(D2D1::ColorF::Red));
		}

		//描画更新を指定したのち16.67ms待機
		if (!Direct2DLib::Instance()->ScreenFlip(60)) { break; }
		//終了判定が出たのでメインループを抜ける
		if (PageManager::Instance()->IsEnd()) { break; }
	}
	delete NetController;
	NetController = nullptr;

	NetWork::NetWorkBrowser::Release();
	//
	PlayerData::Instance()->Save();
	//シングルトンの破棄
	Direct2DLib::Release();
	Input::Release();
	FontPool::Release();
	ItemImagePool::Release();
	ItemDataPool::Release();
	PlayerData::Release();
	PageManager::Release();
	return 0;
}
