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
	// Constants for modern UI layout
	constexpr float SIDEBAR_WIDTH = 180.0f;
	constexpr float SIDEBAR_PADDING = 15.0f;
	constexpr float MENU_ITEM_HEIGHT = 40.0f;
	constexpr float HEADER_HEIGHT = 70.0f;
	constexpr float CORNER_ROUNDING = 14.0f;
	constexpr float GLOW_THICKNESS = 2.5f;
	
	// Helper function to draw rotating neon glow border
	static void DrawAnimatedGlowBorder(ImDrawList* drawList, ImVec2 min, ImVec2 max, float thickness = GLOW_THICKNESS)
	{
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto now = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float>(now - start_time).count();
		
		// Rotating hue for glow
		float hue = std::fmod(elapsed * 0.5f, 6.28f);
		
		// Create gradient glow color (purple to pink to purple)
		ImVec4 glow_color = ImVec4(
			0.616f + std::sin(hue) * 0.2f,           // Purple base + variation
			0.173f + std::cos(hue * 1.5f) * 0.15f,
			0.867f + std::sin(hue * 0.7f) * 0.1f,
			0.8f
		);
		
		ImU32 glow_col = ImGui::GetColorU32(glow_color);
		
		// Draw multiple layers for smooth glow effect
		for (int i = 3; i >= 1; --i)
		{
			ImVec4 layer_color = glow_color;
			layer_color.w = 0.4f / i;
			ImU32 layer_col = ImGui::GetColorU32(layer_color);
			
			drawList->AddRect(
				ImVec2(min.x - i * 1.5f, min.y - i * 1.5f),
				ImVec2(max.x + i * 1.5f, max.y + i * 1.5f),
				layer_col,
				CORNER_ROUNDING,
				0,
				1.0f
			);
		}
		
		// Main border
		drawList->AddRect(min, max, glow_col, CORNER_ROUNDING, 0, thickness);
	}
	
	// Draw sidebar menu item
	static void DrawMenuItemButton(ImDrawList* drawList, ImVec2 pos, ImVec2 size, const char* label, bool selected)
	{
		ImVec2 min = pos;
		ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);
		
		// Background
		ImVec4 bg_color = selected ? 
			ImVec4(0.482f, 0.173f, 0.749f, 0.9f) :  // Active purple
			ImVec4(0.12f, 0.10f, 0.18f, 0.6f);       // Inactive dark purple
		
		drawList->AddRectFilled(min, max, ImGui::GetColorU32(bg_color), 8.0f);
		
		// Border
		ImVec4 border_color = selected ?
			ImVec4(0.616f, 0.306f, 0.867f, 1.0f) :   // Active neon purple
			ImVec4(0.482f, 0.173f, 0.749f, 0.4f);    // Inactive purple
		
		drawList->AddRect(min, max, ImGui::GetColorU32(border_color), 8.0f, 0, 2.0f);
		
		// Text
		ImVec2 text_size = ImGui::CalcTextSize(label);
		ImVec2 text_pos = ImVec2(
			pos.x + (size.x - text_size.x) / 2,
			pos.y + (size.y - text_size.y) / 2
		);
		
		ImVec4 text_color = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
		drawList->AddText(text_pos, ImGui::GetColorU32(text_color), label);
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
		// SIDEBAR PANEL (Floating on left)
		// ═══════════════════════════════════════════════════════════════════
		
		static ImVec2 sidebar_offset = ImVec2(10, 100);
		static bool dragging_sidebar = false;
		static ImVec2 drag_start;
		
		ImVec2 sidebar_min = sidebar_offset;
		ImVec2 sidebar_max = ImVec2(sidebar_offset.x + SIDEBAR_WIDTH, sidebar_offset.y + screen_height - 150);
		ImVec2 sidebar_size = sidebar_max - sidebar_min;
		
		// Sidebar background with glow
		drawList->AddRectFilled(
			sidebar_min, 
			sidebar_max, 
			ImGui::GetColorU32(ImVec4(0.08f, 0.07f, 0.12f, 0.95f)),
			CORNER_ROUNDING
		);
		
		// Animated sidebar border glow
		DrawAnimatedGlowBorder(drawList, sidebar_min, sidebar_max);
		
		// Lumina logo/title in sidebar
		ImVec2 logo_pos = ImVec2(sidebar_min.x + SIDEBAR_PADDING, sidebar_min.y + 15);
		drawList->AddText(YimMenu::Menu::Font::g_AwesomeFont, 28.0f, logo_pos, ImGui::GetColorU32(ImVec4(0.616f, 0.306f, 0.867f, 1.0f)), "ⓛ");
		
		// Sidebar menu items
		float menu_y = sidebar_min.y + 60;
		for (size_t i = 0; i < submenus.size(); ++i)
		{
			auto& submenu = submenus[i];
			ImVec2 item_pos = ImVec2(sidebar_min.x + SIDEBAR_PADDING / 2, menu_y);
			ImVec2 item_size = ImVec2(SIDEBAR_WIDTH - SIDEBAR_PADDING, MENU_ITEM_HEIGHT - 5);
			
			bool is_selected = (submenu == activeSubmenu);
			DrawMenuItemButton(drawList, item_pos, item_size, submenu->m_Name.c_str(), is_selected);
			
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
		ImGui::InvisibleButton("##sidebar_drag", ImVec2(SIDEBAR_WIDTH, 50));
		if (ImGui::IsItemActive())
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				ImVec2 delta = io.MouseDelta;
				sidebar_offset += delta;
			}
		}
		
		// ═══════════════════════════════════════════════════════════════════
		// MAIN CONTENT WINDOW (Right of sidebar)
		// ═══════════════════════════════════════════════════════════════════
		
		if (YimMenu::UIManager::ShowingContentWindow() && activeSubmenu)
		{
			float content_start_x = sidebar_max.x + 15;
			float content_width = screen_width - content_start_x - 20;
			float content_start_y = 80;
			float content_height = screen_height - content_start_y - 20;
			
			ImVec2 content_min = ImVec2(content_start_x, content_start_y);
			ImVec2 content_max = ImVec2(content_start_x + content_width, content_start_y + content_height);
			
			// Content window background
			drawList->AddRectFilled(
				content_min,
				content_max,
				ImGui::GetColorU32(ImVec4(0.08f, 0.07f, 0.12f, 0.92f)),
				CORNER_ROUNDING
			);
			
			// Animated content border
			DrawAnimatedGlowBorder(drawList, content_min, content_max);
			
			// ─────────────────────────────────────────────────────────
			// HEADER SECTION (Category tabs)
			// ─────────────────────────────────────────────────────────
			
			ImVec2 header_min = ImVec2(content_min.x + 10, content_min.y + 10);
			ImVec2 header_max = ImVec2(content_max.x - 10, content_min.y + HEADER_HEIGHT);
			
			// Header background
			drawList->AddRectFilled(
				header_min,
				header_max,
				ImGui::GetColorU32(ImVec4(0.10f, 0.08f, 0.15f, 0.7f)),
				10.0f
			);
			
			// Header border with subtle glow
			drawList->AddRect(
				header_min,
				header_max,
				ImGui::GetColorU32(ImVec4(0.482f, 0.173f, 0.749f, 0.6f)),
				10.0f,
				0,
				2.0f
			);
			
			// Category tabs in header
			ImGui::SetCursorScreenPos(header_min + ImVec2(15, 10));
			if (ImGui::BeginChild("##header_tabs", ImVec2(header_max.x - header_min.x - 30, HEADER_HEIGHT - 20), false, ImGuiWindowFlags_NoBackground))
			{
				if (activeSubmenu)
					activeSubmenu->DrawCategorySelectors();
				
				ImGui::EndChild();
			}
			
			// ─────────────────────────────────────────────────────────
			// CONTENT AREA (Options panels)
			// ─────────────────────────────────────────────────────────
			
			ImVec2 content_area_min = ImVec2(content_min.x + 10, header_max.y + 15);
			ImVec2 content_area_max = ImVec2(content_max.x - 10, content_max.y - 10);
			
			ImGui::SetCursorScreenPos(content_area_min);
			if (ImGui::BeginChild("##options_content", content_area_max - content_area_min, false))
			{
				ImFont* optionsFont = YimMenu::UIManager::GetOptionsFont();
				if (optionsFont)
					ImGui::PushFont(optionsFont);
				
				if (activeSubmenu)
					activeSubmenu->Draw();
				
				if (optionsFont)
					ImGui::PopFont();
				
				ImGui::EndChild();
			}
		}
		
		// ═══════════════════════════════════════════════════════════════════
		// LOGO/BRAND ELEMENT (Top left corner)
		// ═══════════════════════════════════════════════════════════════════
		
		// Lumina logo in top left
		ImVec2 logo_corner_pos = ImVec2(15, 15);
		ImVec4 logo_color = ImVec4(0.616f, 0.306f, 0.867f, 1.0f);
		drawList->AddText(YimMenu::Menu::Font::g_AwesomeFont, 40.0f, logo_corner_pos, ImGui::GetColorU32(logo_color), "ⓛ");
	}
}
