#include "Themes.hpp"
#include "core/commands/ColorCommand.hpp"
#include "game/frontend/submenus/Settings/GUISettings.hpp"

namespace YimMenu
{
	void DefaultStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		// Lumina Theme - Neon Purple & Yellow with Glow Effects
		// Colors: Neon Purple #9D4EDD (0.616, 0.306, 0.867), Dark Purple #7B2CBF (0.482, 0.173, 0.749), Neon Yellow #FFBE0B (1.00, 0.745, 0.043)
		
		// Text
		style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

		// Backgrounds - Dark with slight purple tint for elegance
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.07f, 0.12f, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.08f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.06f, 0.11f, 0.95f);

		// Borders - Neon Purple with glow
		style.Colors[ImGuiCol_Border] = ImVec4(0.482f, 0.173f, 0.749f, 0.45f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.616f, 0.306f, 0.867f, 0.15f);

		// Frames - Neon Purple theme with glow on hover
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.10f, 0.18f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.482f, 0.173f, 0.749f, 0.65f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.616f, 0.306f, 0.867f, 0.85f);

		// Title bars - Purple glow
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.05f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.03f, 0.06f, 1.00f);

		// Menus/tabs - Neon Purple
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.07f, 0.12f, 1.00f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.482f, 0.173f, 0.749f, 0.70f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.616f, 0.306f, 0.867f, 0.90f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.40f, 0.15f, 0.60f, 0.50f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.482f, 0.173f, 0.749f, 0.70f);

		// Scrollbar - Purple with glow
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.04f, 0.08f, 0.80f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Checkmarks, sliders - Purple filled checkbox like Cherax
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.85f, 0.85f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Buttons - Neon Purple with glow effect
		style.Colors[ImGuiCol_Button] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Headers - Neon Purple
		style.Colors[ImGuiCol_Header] = ImVec4(0.482f, 0.173f, 0.749f, 0.80f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.616f, 0.306f, 0.867f, 0.90f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Separators - Purple theme
		style.Colors[ImGuiCol_Separator] = ImVec4(0.482f, 0.173f, 0.749f, 0.50f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.616f, 0.306f, 0.867f, 0.78f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Resize grips - Purple with yellow on active
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.482f, 0.173f, 0.749f, 0.30f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.616f, 0.306f, 0.867f, 0.70f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.745f, 0.043f, 0.90f);

		// Plots - Yellow highlight with purple accents
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Drag & drop - Yellow accent with glow
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.745f, 0.043f, 0.90f);

		// Navigation highlighting - Purple
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.15f, 0.10f, 0.20f, 0.30f);

		// Modal/background dimming - Purple tinted
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.05f, 0.15f, 0.50f);

		// Rounding - Smooth and modern
		style.GrabRounding = style.FrameRounding = 4.0f;
		style.ChildRounding = style.WindowRounding = 10.0f;
		style.TabRounding = 10.0f;
		
		// Padding & spacing — compact like Cherax
		style.FramePadding = ImVec2(6.0f, 4.0f);
		style.ItemSpacing = ImVec2(10.0f, 5.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
		style.TouchExtraPadding = ImVec2(4.0f, 4.0f);
	}

	void SetupStyle()
	{
		// Apply default style first
		DefaultStyle();

		// Initialize the color/rounding commands and load saved settings
		InitializeColorCommands(); // This will call LoadSettings internally

		// Apply loaded colors/rounding to ImGui
		ApplyThemeToImGui();
	}
}
