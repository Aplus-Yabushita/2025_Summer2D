//#include <WinSock2.h> // Windows.hをインクルードしたい場合は、Windows.ｈよりもWinsock2.hを先にインクルードすること
//#include <ws2def.h>
//#include <Windows.h>
#include <ws2tcpip.h>
#include <windowsx.h>

#pragma comment(lib, "ws2_32.lib")
//#pragma warning(disable:4996) // inet_addr()関数で警告が出る場合は以下で警告を無効化する。

#include "Net.hpp"

#define NOMINMAX

#include <algorithm>
#include <array>
#include <stdio.h>
#include <iostream>
#include <string>



namespace NetWork {
	//サーバー専用
	void ServerControl::Update(PlayerNetWork* pPlayerNetwork) noexcept {
		bool AllReady = true;
		for (int loop = 0; loop < Player_num; ++loop) {
			// サーバープレイヤーのデータをアップデート
			if (GetIsServerPlayer(static_cast<PlayerID>(loop))) {
				pPlayerNetwork->SetLastServerDataBuffer().UpdatePlayerData(pPlayerNetwork->GetLocalPlayerData());
				continue;
			}
			//それ以外のプレイヤーデータの受信と反映
			auto& net = this->m_PlayerUDPPhase[loop];
			auto IsDataUpdated = net.RecvData(&this->m_LastPlayerData[loop]);
			switch (net.GetPhase()) {
			case ClientPhase::WaitConnect:// 無差別受付
				if (IsDataUpdated) {// 該当ソケットにクライアントからなにか受信したら
					net.SetPhase(ClientPhase::Ready);
					pPlayerNetwork->SetLastServerDataBuffer().SetReady(static_cast<PlayerID>(loop));
				}
				break;
			case ClientPhase::Ready:
				//得たプレイヤーのデータをアップデート
				if (IsDataUpdated) {
					if (this->m_LastPlayerData[loop].GetID() == loop) {
						pPlayerNetwork->SetLastServerDataBuffer().UpdatePlayerData(this->m_LastPlayerData[loop]);
					}
				}
				// クライアントにデータを送る
				if (pPlayerNetwork->GetTickUpdateFlag()) {
					pPlayerNetwork->SetLastServerDataBuffer().SetCheckSum();
					net.SendData(pPlayerNetwork->GetLastServerDataBuffer());
				}
				break;
			default:
				break;
			}
			if (net.GetPhase() != ClientPhase::Ready) {
				AllReady = false;
			}
		}
		pPlayerNetwork->SetLastServerDataBuffer().Update(AllReady);
	}
	//クライアント専用
	void ClientControl::Update(PlayerNetWork* pPlayerNetwork) noexcept {
		auto& net = this->m_PlayerUDPPhase;
		bool IsDataUpdated = net.RecvData(&pPlayerNetwork->SetLastServerDataBuffer());
		switch (net.GetPhase()) {
		case ClientPhase::WaitConnect:
			if (IsDataUpdated) {
				this->m_CannotConnectTimer = Direct2DLib::Instance()->currentTime();
				int EmptyID = pPlayerNetwork->SetLastServerDataBuffer().GetEmptyID();
				if (EmptyID != InvalidID) {
					//席が空いていて、自分のIDをもらえたので次へ
					pPlayerNetwork->SetLocalPlayerID(static_cast<PlayerID>(EmptyID));
					net.SetPhase(ClientPhase::Client_WaitReady);
				}
			}
			else {
				auto NowTime = Direct2DLib::Instance()->currentTime();
				auto Time = NowTime - this->m_CannotConnectTimer;
				auto Diff = Time - (static_cast<long long>(1000) * 1000);
				if (Diff >= 0) {
					this->m_CannotConnectTimer = NowTime - Diff;
					++this->m_NetWorkSelect;
					net.Dispose();
					if (this->m_NetWorkSelect < Player_num) {
						net.Init(false, this->m_Port + this->m_NetWorkSelect, this->m_IP);//ポートをずらして再試行
					}
					else {
						net.SetPhase(ClientPhase::NotConnect);	//満タンなので接続失敗
					}
				}
			}
			// 自身のデータを送信
			if (pPlayerNetwork->GetTickUpdateFlag()) {
				net.SendData(pPlayerNetwork->GetLocalPlayerData());
			}
			break;
		case ClientPhase::Client_WaitReady:
			if (pPlayerNetwork->GetLastServerDataBuffer().GetServerPhase() == ServerPhase::Ingame) {
				//マッチ済なので次へ
				net.SetPhase(ClientPhase::Ready);
			}
			// 自身のデータを送信
			if (pPlayerNetwork->GetTickUpdateFlag()) {
				net.SendData(pPlayerNetwork->GetLocalPlayerData());
			}
			break;
		case ClientPhase::Ready:
			// 自身のデータを送信
			if (pPlayerNetwork->GetTickUpdateFlag()) {
				net.SendData(pPlayerNetwork->GetLocalPlayerData());
			}
			break;
		default:
			break;
		}
	}
	//通信
	void NetWorkController::Update(const PlayerSendData& Players) noexcept {
		//ローカルデータ更新
		this->m_PlayerNet.UpdateLocalData(Players, IsInGame());
		if (this->m_IsServer) {
			//閲覧するプレイヤーIDを確定
			auto GetViewID = this->m_ServerCtrl.GetViewPlayerID();
			if (GetViewID != InvalidID) {
				this->m_PlayerNet.SetLocalPlayerID((PlayerID)GetViewID);
			}
			this->m_ServerCtrl.Update(&this->m_PlayerNet);
		}
		else {
			this->m_ClientCtrl.Update(&this->m_PlayerNet);
		}
		this->m_PlayerNet.Update();
		//PING計測
		if (this->m_PlayerNet.IsNeedUpdatePing()) {
			CalcPing(this->m_PlayerNet.GetPingTime());
		}
	}

	void UDPCore::InitUDP(int Port) {
		int ans{};
		WSADATA wsaData{};
		WSACleanup();
		ans = (int)WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと

		this->m_Handle = (SOCKET)socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_STREAMはUDP通信、0は？
	}
	//ハンドルを破棄
	void UDPCore::DisposeUDP() {
		closesocket(this->m_Handle);
		this->m_Handle = InvalidSOCKETID;
		WSACleanup();
		return;
	}
	//受信チェック
	int UDPCore::CheckRecvUDP(int checkPort) {
		if (!IsActive()) { return FALSE; }
		// アドレス等格納
		SOCKADDR_IN Addr;
		Addr.sin_family = AF_INET;  //IPv4
		Addr.sin_port = htons(checkPort);   //通信ポート番号設定
		Addr.sin_addr.S_un.S_addr = INADDR_ANY; // INADDR_ANYはすべてのアドレスからのパケットを受信する
		return (bind(this->m_Handle, (sockaddr*)&Addr, sizeof(Addr)) == 0) ? TRUE : FALSE;//こっちは成功するが
	}
	//データを受信
	int UDPCore::RecvDataUDP(IPDATA* RecvIP, int SendPort, int* RecvPort, void* Buffer, int Length, bool Peek) {
		if (!IsActive()) { return InvalidID; }
		// ここでノンブロッキングに設定しています
		u_long val = 1;
		ioctlsocket(this->m_Handle, FIONBIO, &val);

		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(this->m_Handle, &readfds);

		timeval timeout{};
		timeout.tv_sec = 0;
		timeout.tv_usec = 16000;//16ms待つ

		int ret = select(0, &readfds, NULL, NULL, &timeout);
		if (ret > 0 && FD_ISSET(this->m_Handle, &readfds)) {
			// recvfromで何か受け取れる
			SOCKADDR_IN Addr;
			Addr.sin_family = AF_INET;  //IPv4
			Addr.sin_port = htons(SendPort);   //通信ポート番号設定
			Addr.sin_addr.S_un.S_addr = INADDR_ANY; // INADDR_ANYはすべてのアドレスからのパケットを受信する
			int ADDRLen = sizeof(Addr);
			int Ansewr = recvfrom(this->m_Handle, (char*)Buffer, Length, (Peek) ? MSG_PEEK : 0, (SOCKADDR*)&Addr, &ADDRLen);
			int Error = WSAGetLastError();
			if (Ansewr >= 1) {
				*RecvPort = Addr.sin_port;
				RecvIP->d1 = Addr.sin_addr.S_un.S_un_b.s_b1;
				RecvIP->d2 = Addr.sin_addr.S_un.S_un_b.s_b2;
				RecvIP->d3 = Addr.sin_addr.S_un.S_un_b.s_b3;
				RecvIP->d4 = Addr.sin_addr.S_un.S_un_b.s_b4;
				return Ansewr;
			}
		}
		else if (ret == 0) {
			// タイムアウト
		}
		else {
			// エラー
			int Error = WSAGetLastError();
		}
		return InvalidID;
	}
	//データを送信
	int UDPCore::SendDataUDP(IPDATA SendIP, int SendPort, const void* Buffer, int Length) {
		if (!IsActive()) { return FALSE; }
		SOCKADDR_IN Addr;
		Addr.sin_family = AF_INET;
		Addr.sin_port = htons(SendPort);// 待ち受けポート番号
		// 送信アドレスを設定
		Addr.sin_addr.S_un.S_un_b.s_b1 = SendIP.d1;
		Addr.sin_addr.S_un.S_un_b.s_b2 = SendIP.d2;
		Addr.sin_addr.S_un.S_un_b.s_b3 = SendIP.d3;
		Addr.sin_addr.S_un.S_un_b.s_b4 = SendIP.d4;

		sendto(this->m_Handle, (char*)Buffer, Length, 0, (sockaddr*)&Addr, sizeof(Addr));//Addrに文字列送信
		return FALSE;
	}
}
