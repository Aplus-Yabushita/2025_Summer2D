#pragma once

#include <array>
#include <memory>

#include "../Algorithm.h"

class PageParent {
	std::array<const std::unique_ptr<PageParent>*, 10>	m_NextPage{};
	size_t												m_NextPageID{ 0 };
public:
	PageParent() {}
	PageParent(const PageParent&) = delete;
	PageParent(PageParent&&) = delete;
	PageParent& operator=(const PageParent&) = delete;
	PageParent& operator=(PageParent&&) = delete;
	virtual ~PageParent() {}
public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual bool IsEndPage() = 0;
	virtual void Dispose() = 0;
public:
	void SetNextPage(size_t ID, std::unique_ptr<PageParent>* FirstPage) {
		m_NextPage.at(ID) = FirstPage;
	}
	bool HasNextScene() const { return m_NextPage.at(m_NextPageID) != nullptr; }
	const std::unique_ptr<PageParent>* NextPage() { return m_NextPage.at(m_NextPageID); }
protected:
	void SetNextPageID(size_t ID) { m_NextPageID = Mathf::Clamp<size_t>(ID, 0, static_cast<size_t>(m_NextPage.size()) - 1); }
};