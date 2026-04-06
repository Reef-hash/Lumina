#include "game/pointers/Pointers.hpp"
#include "game/frontend/Menu.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "game/frontend/submenus/Settings/GUISettings.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include <cmath>
#include <chrono>

namespace YimMenu
{
	// Cherax-style layout constants
	static constexpr float SIDEBAR_W   = 60.0f;
	static constexpr float CAT_BAR_H   = 42.0f;
	static constexpr float WIN_ROUNDING = 8.0f;

	// Animated gradient bar at top of window (like Cherax header bar)
	static void DrawGradientBar(ImDrawList* dl, ImVec2 p, ImVec2 sz)
	{
		static auto t0 = std::chrono::steady_clock::now();
		float t = std::chrono::duration<float>(std::chrono::steady_clock::now() - t0).count();
		float shift = std::fmod(t * 0.4f, 6.28f);

		ImU32 colL = ImGui::GetColorU32(ImVec4(
			0.10f + 0.15f * std::sin(shift),
			0.05f,
			0.30f + 0.20f * std::cos(shift), 1.0f));
		ImU32 colR = ImGui::GetColorU32(ImVec4(
			0.40f + 0.10f * std::cos(shift),
			0.10f,
			0.60f + 0.10f * std::sin(shift), 1.0f));

		dl->AddRectFilledMultiColor(p, ImVec2(p.x + sz.x, p.y + sz.y), colL, colR, colR, colL);
	}

	void RenderModernTheme()
	{
		YimMenu::SyncColorCommandsToStyle();

		float scrW = (float)*YimMenu::Pointers.ScreenResX;
		float scrH = (float)*YimMenu::Pointers.ScreenResY;

		// Window sizing — Cherax style: fixed size, centered
		float winW = std::min(scrW * 0.50f, 780.0f);
		float winH = std::min(scrH * 0.55f, 520.0f);

		ImGui::SetNextWindowSize(ImVec2(winW, winH), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(
			ImVec2((scrW - winW) * 0.5f, (scrH - winH) * 0.5f),
			ImGuiCond_FirstUseEver);

		// Override window style for this frame
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, WIN_ROUNDING);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.07f, 0.06f, 0.10f, 0.97f));

		ImGuiWindowFlags wflags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse;

		if (!ImGui::Begin("##LuminaMain", nullptr, wflags))
		{
			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar(3);
			return;
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);

		ImDrawList* dl = ImGui::GetWindowDrawList();
		ImVec2 wPos  = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();

		// ── Animated gradient header bar (4px) ──────────────────────
		DrawGradientBar(dl, wPos, ImVec2(wSize.x, 4.0f));

		const auto& submenus   = YimMenu::UIManager::GetSubmenus();
		auto activeSubmenu     = YimMenu::UIManager::GetActiveSubmenu();

		// ════════════════════════════════════════════════════════════
		//  LEFT SIDEBAR  (icon tabs, dark background)
		// ════════════════════════════════════════════════════════════
		ImGui::SetCursorPos(ImVec2(0, 4.0f));

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.04f, 0.08f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 8));

		if (ImGui::BeginChild("##sidebar", ImVec2(SIDEBAR_W, 0), true,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			// Lumina brand "L" at top
			ImGui::PushFont(YimMenu::Menu::Font::g_DefaultFont);
			{
				const char* brand = "L";
				ImVec2 ts = ImGui::CalcTextSize(brand);
				ImGui::SetCursorPosX((SIDEBAR_W - ts.x) * 0.5f);
				ImGui::SetCursorPosY(6.0f);
				ImGui::TextColored(ImVec4(0.55f, 0.25f, 0.85f, 1.0f), "%s", brand);
			}
			ImGui::PopFont();

			ImGui::Dummy(ImVec2(0, 6));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 4));

			// Sidebar tab icons
			ImGui::PushFont(YimMenu::Menu::Font::g_AwesomeFont);
			for (size_t i = 0; i < submenus.size(); ++i)
			{
				auto& sub = submenus[i];
				bool active = (sub == activeSubmenu);
				const char* ico = sub->m_Icon.empty() ? "?" : sub->m_Icon.c_str();

				// Highlight bar for active tab
				if (active)
				{
					ImVec2 cPos = ImGui::GetCursorScreenPos();
					dl->AddRectFilled(
						cPos,
						ImVec2(cPos.x + 3, cPos.y + 28),
						ImGui::GetColorU32(ImVec4(0.55f, 0.25f, 0.85f, 1.0f)),
						1.0f);
				}

				ImVec2 ts = ImGui::CalcTextSize(ico);
				ImGui::SetCursorPosX((SIDEBAR_W - ts.x) * 0.5f);

				if (active)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.45f, 1.0f, 1.0f));
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.40f, 0.55f, 1.0f));

				ImGui::PushID((int)i);
				if (ImGui::Selectable(ico, active, ImGuiSelectableFlags_None, ImVec2(SIDEBAR_W, 28)))
				{
					YimMenu::UIManager::SetActiveSubmenu(sub);
					YimMenu::UIManager::SetShowContentWindow(true);
				}

				// Tooltip with submenu name on hover
				if (ImGui::IsItemHovered())
				{
					ImGui::PopFont(); // back to default font for tooltip
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(sub->m_Name.c_str());
					ImGui::EndTooltip();
					ImGui::PushFont(YimMenu::Menu::Font::g_AwesomeFont);
				}

				ImGui::PopID();
				ImGui::PopStyleColor();
			}
			ImGui::PopFont();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		// ════════════════════════════════════════════════════════════
		//  RIGHT CONTENT AREA
		// ════════════════════════════════════════════════════════════
		ImGui::SameLine();

		float contentW = wSize.x - SIDEBAR_W - 2;
		float contentH = wSize.y - 4.0f; // minus gradient bar

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 6));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.07f, 0.11f, 1.0f));

		if (ImGui::BeginChild("##content_area", ImVec2(contentW, contentH), false, ImGuiWindowFlags_NoScrollbar))
		{
			ImDrawList* cdl = ImGui::GetWindowDrawList();
			ImVec2 winP = ImGui::GetWindowPos();

			// ── LUMINA Header Bar (edge-to-edge) ────────────────
			{
				float headerH = 48.0f;

				// Dark header background
				cdl->AddRectFilled(winP,
					ImVec2(winP.x + contentW, winP.y + headerH),
					ImGui::GetColorU32(ImVec4(0.04f, 0.03f, 0.06f, 1.0f)));

				// Purple accent line at bottom of header
				cdl->AddRectFilled(
					ImVec2(winP.x, winP.y + headerH - 2),
					ImVec2(winP.x + contentW, winP.y + headerH),
					ImGui::GetColorU32(ImVec4(0.45f, 0.20f, 0.70f, 0.6f)));

				// Centered "LUMINA" text — use largest available font
				ImGui::PushFont(YimMenu::Menu::Font::g_HeaderFont
					? YimMenu::Menu::Font::g_HeaderFont
					: YimMenu::Menu::Font::g_OverlayFont);
				ImVec2 ts = ImGui::CalcTextSize("LUMINA");
				ImGui::SetCursorScreenPos(ImVec2(
					winP.x + (contentW - ts.x) * 0.5f,
					winP.y + (headerH - 2 - ts.y) * 0.5f));
				ImGui::TextColored(ImVec4(0.65f, 0.35f, 0.90f, 1.0f), "LUMINA");
				ImGui::PopFont();

				// Position cursor below header
				ImGui::SetCursorScreenPos(ImVec2(winP.x + 10, winP.y + headerH + 4));
			}

			if (activeSubmenu)
			{
				// ── Submenu name ────────────────────────────────────
				ImGui::PushFont(YimMenu::Menu::Font::g_DefaultFont);
				ImGui::TextColored(ImVec4(0.55f, 0.40f, 0.80f, 1.0f), "%s", activeSubmenu->m_Name.c_str());
				ImGui::PopFont();

				// ── Category tab bar ────────────────────────────────
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.05f, 0.09f, 1.0f));
				if (ImGui::BeginChild("##cat_bar", ImVec2(0, CAT_BAR_H), true, ImGuiWindowFlags_NoScrollbar))
				{
					activeSubmenu->DrawCategorySelectors();
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();

				ImGui::Spacing();

				// ── Options content ─────────────────────────────────
				if (ImGui::BeginChild("##options", ImVec2(0, 0), false))
				{
					auto optFont = YimMenu::UIManager::GetOptionsFont();
					if (optFont)
						ImGui::PushFont(optFont);

					activeSubmenu->Draw();

					if (optFont)
						ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::End();
	}
}
