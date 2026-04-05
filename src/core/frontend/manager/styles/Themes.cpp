#include "Themes.hpp"
#include "core/commands/ColorCommand.hpp"
#include "game/frontend/submenus/Settings/GUISettings.hpp"

namespace YimMenu
{
	void DefaultStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		// Lumina Theme - Neon Purple & Yellow
		// Colors: Neon Purple #9D4EDD (0.616, 0.306, 0.867), Dark Purple #7B2CBF (0.482, 0.173, 0.749), Neon Yellow #FFBE0B (1.00, 0.745, 0.043)
		
		// Text
		style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

		// Backgrounds
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.95f);

		// Borders - Neon Purple accent
		style.Colors[ImGuiCol_Border] = ImVec4(0.482f, 0.173f, 0.749f, 0.35f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// Frames - Neon Purple theme
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.482f, 0.173f, 0.749f, 0.65f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.616f, 0.306f, 0.867f, 0.85f);

		// Title bars
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.04f, 0.05f, 1.00f);

		// Menus/tabs - Neon Purple
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.482f, 0.173f, 0.749f, 0.75f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.616f, 0.306f, 0.867f, 0.85f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.40f, 0.15f, 0.60f, 0.60f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.482f, 0.173f, 0.749f, 0.75f);

		// Scrollbar - Purple tinted
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.80f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Checkmarks, sliders - Yellow highlight
		style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Buttons - Neon Purple
		style.Colors[ImGuiCol_Button] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Headers - Neon Purple
		style.Colors[ImGuiCol_Header] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Separators - Purple theme
		style.Colors[ImGuiCol_Separator] = ImVec4(0.482f, 0.173f, 0.749f, 0.50f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.616f, 0.306f, 0.867f, 0.78f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);

		// Resize grips - Purple
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.482f, 0.173f, 0.749f, 0.30f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.616f, 0.306f, 0.867f, 0.60f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.745f, 0.043f, 0.90f);

		// Plots - Yellow highlight
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.616f, 0.306f, 0.867f, 0.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.745f, 0.043f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.482f, 0.173f, 0.749f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);

		// Drag & drop - Yellow accent
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.745f, 0.043f, 0.90f);

		// Navigation highlighting - Purple
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.616f, 0.306f, 0.867f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);

		// Modal/background dimming
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.35f);

		// Rounding
		style.GrabRounding = style.FrameRounding = style.ChildRounding = style.WindowRounding = 8.0f;
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
