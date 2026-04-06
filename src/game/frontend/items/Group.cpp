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

		// Cherax-style boxed group with dark header
		ImDrawList* dl = ImGui::GetWindowDrawList();
		float availW = ImGui::GetContentRegionAvail().x;
		ImVec2 boxStart = ImGui::GetCursorScreenPos();
		float startCursorY = ImGui::GetCursorPosY();

		ImFont* titleFont = Menu::Font::g_DefaultFont ? Menu::Font::g_DefaultFont : ImGui::GetFont();
		ImGui::PushFont(titleFont);
		float headerH = ImGui::GetFontSize() + 12.0f;

		// Dark header background (top corners rounded)
		dl->AddRectFilled(boxStart,
			ImVec2(boxStart.x + availW, boxStart.y + headerH),
			ImGui::GetColorU32(ImVec4(0.03f, 0.02f, 0.05f, 1.0f)),
			4.0f, ImDrawFlags_RoundCornersTop);

		// Title text centered in header
		ImVec2 titleSize = ImGui::CalcTextSize(m_Name.c_str());
		float titleX = boxStart.x + (availW - titleSize.x) * 0.5f;
		float titleY = boxStart.y + (headerH - titleSize.y) * 0.5f;
		dl->AddText(titleFont, ImGui::GetFontSize(),
			ImVec2(titleX, titleY),
			ImGui::GetColorU32(ImVec4(0.85f, 0.78f, 0.95f, 1.0f)),
			m_Name.c_str());
		ImGui::PopFont();

		// Position below header
		ImGui::SetCursorPosY(startCursorY + headerH + 6.0f);

		// Draw items
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

		ImGui::Dummy(ImVec2(0, 6.0f));

		// Border around full box
		ImVec2 boxEnd(boxStart.x + availW, ImGui::GetCursorScreenPos().y);
		dl->AddRect(boxStart, boxEnd,
			ImGui::GetColorU32(ImVec4(0.25f, 0.15f, 0.40f, 0.5f)),
			4.0f, 0, 1.0f);

		ImGui::Spacing();
		ImGui::Spacing();
	}
}