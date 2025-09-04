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

//#include "Network/Net.hpp"

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
		//描画更新を指定したのち16.67ms待機
		if (!Direct2DLib::Instance()->ScreenFlip(60)) { break; }
		//終了判定が出たのでメインループを抜ける
		if (PageManager::Instance()->IsEnd()) { break; }
	}
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
