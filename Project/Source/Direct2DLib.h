#pragma once
//
#include "Util.h"
#include "Image/BitMap.h"
#include "Font/Font.h"

#include "DrawSystem.h"

#include <thread>
#include <chrono>
//
class Direct2DLib : public SingletonBase<Direct2DLib> {
private:
	friend class SingletonBase<Direct2DLib>;
private:
	class WICSystem {
		IWICImagingFactory* m_pIWICFactory{ nullptr };
	public:
		const auto& GetFactory() const { return m_pIWICFactory; }
	public:
		WICSystem() {
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pIWICFactory));
		}
		virtual ~WICSystem() {
			SafeRelease(&m_pIWICFactory);
			CoUninitialize();
		}
	};
	class MainWindowProc {
	public:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			MainWindowProc* pThis = nullptr;

			if (uMsg == WM_NCCREATE) {
				CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
				pThis = (MainWindowProc*)pCreate->lpCreateParams;
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

				pThis->m_hwnd = hwnd;
			}
			else {
				pThis = (MainWindowProc*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			}
			if (pThis) {
				return pThis->HandleMessage(uMsg, wParam, lParam);
			}
			else {
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}
		//ウィンドウメッセージを処理する
		static bool PeekProcessMessage() {
			MSG msg = { };
			while (true) {
				//PM_REMOVEはメッセージ処理後に削除することを表す。基本PM_REMOVEでOK
				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
					if (msg.message == WM_QUIT) {
						return false;
					}
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else {
					break;
				}
			}
			return true;
		}
	private:
		HWND m_hwnd{ NULL };
	public:
		HWND Window() const { return m_hwnd; }
	public:
		MainWindowProc() {}
	public:
		bool Create(
			LPCSTR lpWindowName,
			DWORD dwStyle,
			DWORD dwExStyle = 0,
			int x = CW_USEDEFAULT,
			int y = CW_USEDEFAULT,
			int nWidth = CW_USEDEFAULT,
			int nHeight = CW_USEDEFAULT,
			HWND hWndParent = 0,
			HMENU hMenu = 0
		) {
			WNDCLASS wc = { 0 };

			wc.lpfnWndProc = MainWindowProc::WindowProc;
			wc.hInstance = GetModuleHandle(nullptr);
			wc.lpszClassName = "Winter2D Class";

			RegisterClass(&wc);

			m_hwnd = CreateWindowEx(
				dwExStyle, "Winter2D Class", lpWindowName, dwStyle, x, y,
				nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(nullptr), this
			);

			return (m_hwnd ? true : false);
		}
	private:
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) const;
	};
private:
	std::unique_ptr<WICSystem> m_WIC;
	std::unique_ptr<MainWindowProc> m_win;
	std::unique_ptr<Direct2DSystem> m_Direct2DSystem{};
	long long PrevTime{};
private:
	Direct2DLib(void) noexcept {}
	Direct2DLib(const Direct2DLib&) = delete;
	Direct2DLib(Direct2DLib&&) = delete;
	Direct2DLib& operator=(const Direct2DLib&) = delete;
	Direct2DLib& operator=(Direct2DLib&&) = delete;
	virtual ~Direct2DLib() {
		m_Direct2DSystem.reset();
		m_WIC.reset();
		m_win.reset();
	}
public:
	const auto& GetWICFactory() const { return m_WIC->GetFactory(); }
	const auto& GetDrawSystem() const { return m_Direct2DSystem; }
public:
	//現在時刻を取得する
	static long long currentTime() {
		std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
	}
	//ウィンドウサイズを指定する
	static void SetClientRect(HWND hwnd, int cx, int cy) {
		RECT rc;
		SetRect(&rc, 0, 0, cx, cy);

		DWORD dwStyle = (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE);

		AdjustWindowRect(&rc, dwStyle, FALSE);
		SetWindowPos(hwnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE);
	}
public:
	//初期化時に行うもの
	bool Init(LPCSTR lpWindowName, int  nCmdShow, int ScreenWidth, int ScreenHeight) {
		m_WIC = std::make_unique<WICSystem>();
		m_win = std::make_unique<MainWindowProc>();
		if (!m_win->Create(lpWindowName,
			WS_OVERLAPPED | 
			WS_CAPTION | 
			WS_SYSMENU | 
			WS_THICKFRAME | 
			WS_MINIMIZEBOX
		)) { return false; }
		m_Direct2DSystem = std::make_unique<Direct2DSystem>(m_win->Window());
		if (!m_Direct2DSystem->IsActive()) { return false; }
		ShowWindow(m_win->Window(), nCmdShow);
		SetClientRect(m_win->Window(), ScreenWidth, ScreenHeight);
		PrevTime = currentTime();
		return true;
	}
	//再描画を指定したのち、特定FPSになるように待機する+次のループを実行できるかチェックする
	bool ScreenFlip(long long FPS) {
		InvalidateRect(m_win->Window(), nullptr, false);
		//16.67ミリ秒スレッド待機
		std::this_thread::sleep_for(std::chrono::microseconds(1000000 / FPS - (currentTime() - PrevTime)));
		PrevTime = currentTime();
		return MainWindowProc::PeekProcessMessage();
	}
};
