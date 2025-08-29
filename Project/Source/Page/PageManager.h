#pragma once

#include "../Util.h"
#include "PageParent.h"

class PageManager : public SingletonBase<PageManager> {
private:
	friend class SingletonBase<PageManager>;
private:
	PageManager() {}
	PageManager(const PageManager&) = delete;
	PageManager(PageManager&&) = delete;
	PageManager& operator=(const PageManager&) = delete;
	PageManager& operator=(PageManager&&) = delete;
	virtual ~PageManager() {
		if (!m_Page) { return; }
		(*m_Page)->Dispose();
	}
private:
	const std::unique_ptr<PageParent>* m_Page{ nullptr };
	bool m_IsEnd = false;
public:
	void SetEnd() { m_IsEnd = true; }
	bool IsEnd() const { return m_IsEnd; }
public:
	void SetFirstPage(const std::unique_ptr<PageParent>* FirstPage) {
		m_Page = FirstPage;
	}
	void Init() {
		if (!m_Page) { return; }
		(*m_Page)->Init();
	}
	void Update() {
		if (!m_Page) { return; }
		(*m_Page)->Update();
		if ((*m_Page)->IsEndPage()) {
			(*m_Page)->Dispose();
			if ((*m_Page)->HasNextScene()) {
				m_Page = (*m_Page)->NextPage();
				Init();
			}
			else {
				SetEnd();
			}
		}
	}
	void Draw() {
		if (!m_Page) { return; }
		(*m_Page)->Draw();
	}
};
