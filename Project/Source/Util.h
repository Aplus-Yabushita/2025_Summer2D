#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <string_view>

template <class T> static void SafeRelease(T** ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = nullptr;
	}
}
// --------------------------------------------------------------------------------------------------
// シングルトン
// --------------------------------------------------------------------------------------------------
template <class T>
class SingletonBase {
private:
	static const T* m_Singleton;
public:
	static void Create(void) noexcept {
		m_Singleton = new T();
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
	static T* Instance(void) noexcept {
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
		// if (m_Singleton == nullptr) { m_Singleton = new T(); }
		return (T*)m_Singleton;
	}
protected:
	SingletonBase(void) noexcept {}
	virtual ~SingletonBase(void) noexcept {}
private:
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
	SingletonBase(SingletonBase&&) = delete;
	SingletonBase& operator=(SingletonBase&&) = delete;
};
// 子のサンプル
/*
class A : public SingletonBase<A> {
private:
	friend class SingletonBase<A>;
}
//*/

// 点と矩形との2D判定
static bool HitPointToRectangle(int xp, int yp, int x1, int y1, int x2, int y2) noexcept { return (xp >= x1 && xp <= x2 && yp >= y1 && yp <= y2); }

// ディレクトリ内のファイル走査
static void GetFileNamesInDirectory(const char* pPath, std::vector<WIN32_FIND_DATA>* pFileList) noexcept {
	pFileList->clear();
	WIN32_FIND_DATA win32fdt;
	HANDLE hFind = FindFirstFile(pPath, &win32fdt);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (win32fdt.cFileName[0] != '.') {
				pFileList->emplace_back(win32fdt);
			}

		} while (FindNextFile(hFind, &win32fdt));
	} // else{ return false; }
	FindClose(hFind);
}

/*wstringをstringへ変換*/
static std::string WStringToString(std::wstring_view oWString) noexcept {
	// wstring → SJIS
	int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), static_cast<int>(oWString.size() + 1), nullptr, 0, NULL, NULL);
	// バッファの取得
	CHAR* cpMultiByte = new CHAR[static_cast<size_t>(iBufferSize)];
	// wstring → SJIS
	WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), static_cast<int>(oWString.size() + 1), cpMultiByte, iBufferSize, NULL, NULL);
	// stringの生成
	std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
	// バッファの破棄
	delete[] cpMultiByte;
	// 変換結果を返す
	return oRet;
}
/*stringをwstringへ変換する*/
static std::wstring StringToWString(std::string_view oString, UINT CodePage = CP_ACP) noexcept {
	// SJIS → wstring
	int iBufferSize = MultiByteToWideChar(CodePage, 0, oString.data(), static_cast<int>(oString.size() + 1), nullptr, 0);
	// バッファの取得
	wchar_t* cpUCS2 = new wchar_t[static_cast<size_t>(iBufferSize)];
	// SJIS → wstring
	MultiByteToWideChar(CodePage, 0, oString.data(), static_cast<int>(oString.size() + 1), cpUCS2, iBufferSize);
	// stringの生成
	std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);
	// バッファの破棄
	delete[] cpUCS2;
	// 変換結果を返す
	return oRet;
}
