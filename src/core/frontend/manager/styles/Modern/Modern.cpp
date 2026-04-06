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
			// Lumina brand "L" at top (centered)
			{
				ImGui::PushFont(YimMenu::Menu::Font::g_DefaultFont);
				const char* brand = "L";
				ImVec2 ts = ImGui::CalcTextSize(brand);
				float padX = (SIDEBAR_W - ts.x) * 0.5f;
				ImGui::SetCursorPos(ImVec2(padX, 8.0f));
				ImGui::TextColored(ImVec4(0.55f, 0.25f, 0.85f, 1.0f), "%s", brand);
				ImGui::PopFont();
			}

			ImGui::Dummy(ImVec2(0, 4));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 4));

			// Sidebar tab icons — draw centered in each row
			constexpr float ICON_ROW_H = 36.0f;

			for (size_t i = 0; i < submenus.size(); ++i)
			{
				auto& sub = submenus[i];
				bool active = (sub == activeSubmenu);
				const char* ico = sub->m_Icon.empty() ? "?" : sub->m_Icon.c_str();

				ImVec2 rowStart = ImGui::GetCursorScreenPos();

				// Active highlight — full-width purple box
				if (active)
				{
					dl->AddRectFilled(
						rowStart,
						ImVec2(rowStart.x + SIDEBAR_W, rowStart.y + ICON_ROW_H),
						ImGui::GetColorU32(ImVec4(0.30f, 0.15f, 0.50f, 0.6f)),
						4.0f);
					// Left accent bar
					dl->AddRectFilled(
						rowStart,
						ImVec2(rowStart.x + 3, rowStart.y + ICON_ROW_H),
						ImGui::GetColorU32(ImVec4(0.55f, 0.25f, 0.85f, 1.0f)));
				}

				// Invisible selectable for click detection
				ImGui::PushID((int)i);
				if (ImGui::InvisibleButton("##tab", ImVec2(SIDEBAR_W, ICON_ROW_H)))
				{
					YimMenu::UIManager::SetActiveSubmenu(sub);
					YimMenu::UIManager::SetShowContentWindow(true);
				}

				// Tooltip on hover
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(sub->m_Name.c_str());
					ImGui::EndTooltip();

					// Hover highlight
					if (!active)
					{
						dl->AddRectFilled(
							rowStart,
							ImVec2(rowStart.x + SIDEBAR_W, rowStart.y + ICON_ROW_H),
							ImGui::GetColorU32(ImVec4(0.20f, 0.12f, 0.35f, 0.4f)),
							4.0f);
					}
				}
				ImGui::PopID();

				// Draw icon text centered over the invisible button
				ImGui::PushFont(YimMenu::Menu::Font::g_AwesomeFont);
				ImVec2 ts = ImGui::CalcTextSize(ico);
				ImVec2 iconPos(
					rowStart.x + (SIDEBAR_W - ts.x) * 0.5f,
					rowStart.y + (ICON_ROW_H - ts.y) * 0.5f);
				ImU32 iconCol = active
					? ImGui::GetColorU32(ImVec4(0.80f, 0.55f, 1.0f, 1.0f))
					: ImGui::GetColorU32(ImVec4(0.50f, 0.42f, 0.60f, 1.0f));
				dl->AddText(YimMenu::Menu::Font::g_AwesomeFont, ImGui::GetFontSize(),
					iconPos, iconCol, ico);
				ImGui::PopFont();
			}
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

				// ── Category tabs (Cherax-style text tabs) ────────────
				{
					ImGui::PushFont(YimMenu::Menu::Font::g_OptionsFont
						? YimMenu::Menu::Font::g_OptionsFont
						: YimMenu::Menu::Font::g_DefaultFont);

					auto& categories = activeSubmenu->GetCategories();
					auto activeCategory = activeSubmenu->GetActiveCategory();

					for (size_t ci = 0; ci < categories.size(); ++ci)
					{
						auto& cat = categories[ci];
						if (!cat) continue;
						bool catActive = (cat == activeCategory);

						if (catActive)
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.55f, 1.0f, 1.0f));
						else
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 0.42f, 0.60f, 1.0f));

						ImGui::PushID((int)ci);
						if (ImGui::Selectable(cat->m_Name.c_str(), catActive, 0,
							ImVec2(ImGui::CalcTextSize(cat->m_Name.c_str()).x + 16, 22)))
						{
							activeSubmenu->SetActiveCategory(cat);
						}

						// Underline active tab
						if (catActive)
						{
							ImVec2 mn = ImGui::GetItemRectMin();
							ImVec2 mx = ImGui::GetItemRectMax();
							ImGui::GetWindowDrawList()->AddLine(
								ImVec2(mn.x, mx.y),
								ImVec2(mx.x, mx.y),
								ImGui::GetColorU32(ImVec4(0.55f, 0.25f, 0.85f, 1.0f)),
								2.0f);
						}
						ImGui::PopID();
						ImGui::PopStyleColor();

						if (ci + 1 < categories.size())
							ImGui::SameLine();
					}
					ImGui::PopFont();
				}

				ImGui::Separator();
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
