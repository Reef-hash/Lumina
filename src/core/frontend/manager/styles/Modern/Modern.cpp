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
	// Constants for modern UI layout - OPTIMIZED SIZING
	constexpr float SIDEBAR_WIDTH = 140.0f;          // Reduced from 180
	constexpr float SIDEBAR_PADDING = 12.0f;         // Reduced padding
	constexpr float MENU_ITEM_HEIGHT = 38.0f;
	constexpr float HEADER_HEIGHT = 55.0f;           // Reduced from 70
	constexpr float CORNER_ROUNDING = 12.0f;
	constexpr float GLOW_THICKNESS = 2.0f;
	
	// Icon mapping for tabs
	struct TabIcon {
		const char* name;
		const char* icon;  // Unicode icon
	};
	
	static const TabIcon TabIcons[] = {
		{"Self", "⚡"},
		{"Vehicle", "🚗"},
		{"Teleport", "📍"},
		{"Network", "🌐"},
		{"Players", "👥"},
		{"World", "🌍"},
		{"Recovery", "💰"},
		{"Settings", "⚙️"},
		{"Debug", "🐛"}
	};
	
	// Helper function to draw rotating neon glow border
	static void DrawAnimatedGlowBorder(ImDrawList* drawList, ImVec2 min, ImVec2 max, float thickness = GLOW_THICKNESS)
	{
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto now = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float>(now - start_time).count();
		
		// Rotating hue for glow
		float hue = std::fmod(elapsed * 0.5f, 6.28f);
		
		// Create darker purple glow (not bright neon)
		ImVec4 glow_color = ImVec4(
			0.35f + std::sin(hue) * 0.15f,           // Darker purple
			0.10f + std::cos(hue * 1.5f) * 0.1f,
			0.55f + std::sin(hue * 0.7f) * 0.15f,
			0.6f  // Lower alpha for subtlety
		);
		
		ImU32 glow_col = ImGui::GetColorU32(glow_color);
		
		// Draw multiple layers for smooth glow effect
		for (int i = 2; i >= 1; --i)
		{
			ImVec4 layer_color = glow_color;
			layer_color.w = 0.3f / i;
			ImU32 layer_col = ImGui::GetColorU32(layer_color);
			
			drawList->AddRect(
				ImVec2(min.x - i * 1.0f, min.y - i * 1.0f),
				ImVec2(max.x + i * 1.0f, max.y + i * 1.0f),
				layer_col,
				CORNER_ROUNDING,
				0,
				1.0f
			);
		}
		
		// Main border - darker
		drawList->AddRect(min, max, glow_col, CORNER_ROUNDING, 0, thickness);
	}
	
	// Draw sidebar menu item with icon
	static void DrawMenuItemButton(ImDrawList* drawList, ImVec2 pos, ImVec2 size, const char* icon, const char* label, bool selected)
	{
		ImVec2 min = pos;
		ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);
		
		// Background - darker colors
		ImVec4 bg_color = selected ? 
			ImVec4(0.25f, 0.12f, 0.40f, 0.85f) :  // Active darker purple
			ImVec4(0.10f, 0.08f, 0.15f, 0.5f);    // Inactive very dark
		
		drawList->AddRectFilled(min, max, ImGui::GetColorU32(bg_color), 8.0f);
		
		// Border - subtle
		ImVec4 border_color = selected ?
			ImVec4(0.40f, 0.20f, 0.65f, 0.8f) :   // Active border
			ImVec4(0.30f, 0.12f, 0.50f, 0.3f);    // Inactive border
		
		drawList->AddRect(min, max, ImGui::GetColorU32(border_color), 8.0f, 0, 1.5f);
		
		// Icon (larger)
		ImVec2 icon_size = ImGui::CalcTextSize(icon);
		ImVec2 icon_pos = ImVec2(
			pos.x + (size.x - icon_size.x) / 2,
			pos.y + 5
		);
		
		ImVec4 icon_color = selected ? 
			ImVec4(0.80f, 0.40f, 1.0f, 1.0f) :    // Active glow purple
			ImVec4(0.60f, 0.30f, 0.80f, 0.7f);    // Inactive darker purple
		
		drawList->AddText(icon_pos, ImGui::GetColorU32(icon_color), icon);
		
		// Label (small, below icon) - only for tooltip hover
		if (selected)
		{
			ImVec2 label_size = ImGui::CalcTextSize(label);
			ImVec2 label_pos = ImVec2(
				pos.x + (size.x - label_size.x) / 2,
				pos.y + size.y - 18
			);
			
			ImVec4 label_color = ImVec4(0.85f, 0.85f, 0.85f, 0.8f);
			drawList->AddText(label_pos, ImGui::GetColorU32(label_color), label);
		}
	}
	
	// Main modern theme render function
	void RenderModernTheme()
	{
		YimMenu::SyncColorCommandsToStyle();
		
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		
		float screen_width = *YimMenu::Pointers.ScreenResX;
		float screen_height = *YimMenu::Pointers.ScreenResY;
		
		const auto& submenus = YimMenu::UIManager::GetSubmenus();
		auto activeSubmenu = YimMenu::UIManager::GetActiveSubmenu();
		
		// ═══════════════════════════════════════════════════════════════════
		// SIDEBAR PANEL (Floating on left) - OPTIMIZED
		// ═══════════════════════════════════════════════════════════════════
		
		static ImVec2 sidebar_offset = ImVec2(15, 120);
		static bool dragging_sidebar = false;
		
		ImVec2 sidebar_min = sidebar_offset;
		ImVec2 sidebar_max = ImVec2(sidebar_offset.x + SIDEBAR_WIDTH, sidebar_offset.y + screen_height - 180);
		ImVec2 sidebar_size = sidebar_max - sidebar_min;
		
		// Sidebar background with glow - darker
		drawList->AddRectFilled(
			sidebar_min, 
			sidebar_max, 
			ImGui::GetColorU32(ImVec4(0.06f, 0.05f, 0.10f, 0.95f)),
			CORNER_ROUNDING
		);
		
		// Animated sidebar border glow
		DrawAnimatedGlowBorder(drawList, sidebar_min, sidebar_max);
		
		// Lumina logo/title in sidebar
		ImVec2 logo_pos = ImVec2(sidebar_min.x + SIDEBAR_PADDING, sidebar_min.y + 10);
		drawList->AddText(YimMenu::Menu::Font::g_AwesomeFont, 24.0f, logo_pos, ImGui::GetColorU32(ImVec4(0.50f, 0.25f, 0.85f, 0.9f)), "ⓛ");
		
		// Sidebar menu items - ICON BASED
		float menu_y = sidebar_min.y + 50;
		for (size_t i = 0; i < submenus.size(); ++i)
		{
			auto& submenu = submenus[i];
			const char* icon = (i < sizeof(TabIcons) / sizeof(TabIcons[0])) ? TabIcons[i].icon : "•";
			
			ImVec2 item_pos = ImVec2(sidebar_min.x + 5, menu_y);
			ImVec2 item_size = ImVec2(SIDEBAR_WIDTH - 10, MENU_ITEM_HEIGHT);
			
			bool is_selected = (submenu == activeSubmenu);
			DrawMenuItemButton(drawList, item_pos, item_size, icon, submenu->m_Name.c_str(), is_selected);
			
			// Click detection for sidebar items
			ImGui::SetCursorScreenPos(item_pos);
			ImGui::InvisibleButton(std::format("##sidebar_item_{}", i).c_str(), item_size);
			
			if (ImGui::IsItemClicked())
			{
				YimMenu::UIManager::SetActiveSubmenu(submenu);
				YimMenu::UIManager::SetShowContentWindow(true);
			}
			
			menu_y += MENU_ITEM_HEIGHT;
		}
		
		// Sidebar drag handle (top area)
		ImGui::SetCursorScreenPos(sidebar_min);
		ImGui::InvisibleButton("##sidebar_drag", ImVec2(SIDEBAR_WIDTH, 45));
		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			ImVec2 delta = io.MouseDelta;
			sidebar_offset += delta;
		}
		
		// ═══════════════════════════════════════════════════════════════════
		// MAIN CONTENT WINDOW (Right of sidebar) - FIXED SIZE
		// ═══════════════════════════════════════════════════════════════════
		
		if (YimMenu::UIManager::ShowingContentWindow() && activeSubmenu)
		{
			float content_start_x = sidebar_max.x + 12;
			float content_width = screen_width - content_start_x - 18;
			float content_start_y = 120;
			float content_height = screen_height - content_start_y - 30;
			
			ImVec2 content_min = ImVec2(content_start_x, content_start_y);
			ImVec2 content_max = ImVec2(content_start_x + content_width, content_start_y + content_height);
			
			// Content window background - darker
			drawList->AddRectFilled(
				content_min,
				content_max,
				ImGui::GetColorU32(ImVec4(0.06f, 0.05f, 0.10f, 0.93f)),
				CORNER_ROUNDING
			);
			
			// Animated content border
			DrawAnimatedGlowBorder(drawList, content_min, content_max);
			
			// ─────────────────────────────────────────────────────────
			// HEADER SECTION (Category tabs) - OPTIMIZED
			// ─────────────────────────────────────────────────────────
			
			ImVec2 header_min = ImVec2(content_min.x + 8, content_min.y + 8);
			ImVec2 header_max = ImVec2(content_max.x - 8, content_min.y + HEADER_HEIGHT);
			
			// Header background - darker
			drawList->AddRectFilled(
				header_min,
				header_max,
				ImGui::GetColorU32(ImVec4(0.08f, 0.06f, 0.12f, 0.7f)),
				9.0f
			);
			
			// Header border with subtle glow
			drawList->AddRect(
				header_min,
				header_max,
				ImGui::GetColorU32(ImVec4(0.30f, 0.12f, 0.50f, 0.5f)),
				9.0f,
				0,
				1.5f
			);
			
			// Category tabs in header - FIXED EndChild()
			ImGui::SetCursorScreenPos(header_min + ImVec2(12, 8));
			ImGui::BeginChild("##header_tabs", ImVec2(header_max.x - header_min.x - 24, HEADER_HEIGHT - 16), false, ImGuiWindowFlags_NoBackground);
			{
				if (activeSubmenu)
					activeSubmenu->DrawCategorySelectors();
			}
			ImGui::EndChild();
			
			// ─────────────────────────────────────────────────────────
			// CONTENT AREA (Options panels) - FIXED EndChild()
			// ─────────────────────────────────────────────────────────
			
			ImVec2 content_area_min = ImVec2(content_min.x + 8, header_max.y + 10);
			ImVec2 content_area_max = ImVec2(content_max.x - 8, content_max.y - 8);
			
			ImGui::SetCursorScreenPos(content_area_min);
			ImGui::BeginChild("##options_content", content_area_max - content_area_min, false, ImGuiWindowFlags_NoBackground);
			{
				ImFont* optionsFont = YimMenu::UIManager::GetOptionsFont();
				if (optionsFont)
					ImGui::PushFont(optionsFont);
				
				if (activeSubmenu)
					activeSubmenu->Draw();
				
				if (optionsFont)
					ImGui::PopFont();
			}
			ImGui::EndChild();
		}
	}
}
