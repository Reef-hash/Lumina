#include "UIManager.hpp"
#include "game/pointers/Pointers.hpp"
#include "game/frontend/Menu.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/BoolCommand.hpp"

namespace YimMenu::Features
{
	// Transparency control for GUI
	static FloatCommand _GUITransparency{
		"guitransparency",
		"GUI Transparency",
		"Adjust GUI window transparency (0.0-1.0)",
		1.0f};
	
	// Neon glow effect toggle
	static BoolCommand _NeronGlowEffect{
		"neonglow",
		"Neon Glow Effect",
		"Enable rotating neon glow border effect",
		true};
}

namespace YimMenu
{
	void UIManager::AddSubmenuImpl(const std::shared_ptr<Submenu>&& submenu)
	{
		if (!m_ActiveSubmenu)
			m_ActiveSubmenu = submenu;

		m_Submenus.push_back(std::move(submenu));
	}

	void UIManager::SetActiveSubmenuImpl(const std::shared_ptr<Submenu> submenu)
	{
		m_ActiveSubmenu = submenu;
	}

	void UIManager::DrawImpl()
	{
		// Lumina uses Modern theme exclusively
		RenderModernTheme();
	}

	std::shared_ptr<Submenu> UIManager::GetActiveSubmenuImpl()
	{
		return m_ActiveSubmenu;
	}

	std::shared_ptr<Category> UIManager::GetActiveCategoryImpl()
	{
		if (m_ActiveSubmenu)
		{
			return m_ActiveSubmenu->GetActiveCategory();
		}

		return nullptr;
	}
}