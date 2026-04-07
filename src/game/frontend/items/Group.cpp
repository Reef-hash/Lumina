#include "Items.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/backend/FiberPool.hpp"
#include "game/frontend/Menu.hpp"

namespace YimMenu
{
	Group::Group(const std::string& name, int items_per_row) :
	    m_Name(name),
	    m_ItemsPerRow(items_per_row)
	{
	}

	void Group::Draw()
	{
		if (m_Name.empty())
		{
			int item_count = 0;
			ImGui::BeginGroup();
			for (auto& item : m_Items)
			{
				if (!item->CanDraw())
					continue;
				item->Draw();
				item_count++;
				if (m_ItemsPerRow != -1 && item_count % m_ItemsPerRow == 0)
				{
					ImGui::EndGroup();
					ImGui::SameLine();
					ImGui::BeginGroup();
				}
			}
			ImGui::EndGroup();
			return;
		}

		// BeginChild with rounded border + MenuBar for title
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.25f, 0.15f, 0.40f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.05f, 0.09f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.03f, 0.02f, 0.05f, 1.0f));

		ImGuiWindowFlags childFlags = ImGuiWindowFlags_MenuBar;

		// Use a unique ID per group name
		std::string childId = "##grp_" + m_Name;
		ImGui::BeginChild(childId.c_str(), ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, childFlags);

		// MenuBar as group header
		if (ImGui::BeginMenuBar())
		{
			ImFont* titleFont = Menu::Font::g_DefaultFont ? Menu::Font::g_DefaultFont : ImGui::GetFont();
			ImGui::PushFont(titleFont);

			// Center the title in the menu bar
			float barW = ImGui::GetContentRegionAvail().x;
			ImVec2 titleSize = ImGui::CalcTextSize(m_Name.c_str());
			float offset = (barW - titleSize.x) * 0.5f;
			if (offset > 0)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

			ImGui::TextColored(ImVec4(0.85f, 0.78f, 0.95f, 1.0f), "%s", m_Name.c_str());
			ImGui::PopFont();
			ImGui::EndMenuBar();
		}

		// Draw items inside
		int item_count = 0;
		ImGui::BeginGroup();
		for (auto& item : m_Items)
		{
			if (!item->CanDraw())
				continue;
			item->Draw();
			item_count++;
			if (m_ItemsPerRow != -1 && item_count % m_ItemsPerRow == 0)
			{
				ImGui::EndGroup();
				ImGui::SameLine();
				ImGui::BeginGroup();
			}
		}
		ImGui::EndGroup();

		ImGui::EndChild();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);

		ImGui::Spacing();
	}
}