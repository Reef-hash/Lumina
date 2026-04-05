#include "Notifications.hpp"

#include "core/logger/LogHelper.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/util/Joaat.hpp"

#include <mutex>

namespace YimMenu
{

	Notification Notifications::ShowImpl(std::string title, std::string message, NotificationType type, int duration, std::function<void()> context_function, std::string context_function_name)
	{
		if (title.empty() || message.empty())
			return {};

		auto message_id = Joaat(title + message);

		auto exists = std::find_if(m_Notifications.begin(), m_Notifications.end(), [&](auto& notification) {
			return notification.second.m_Identifier == message_id;
		});

		if (exists != m_Notifications.end())
		{
			exists->second.m_CreatedOn = std::chrono::system_clock::now();
			return {};
		}

		Notification notification{};
		notification.m_Title = title;
		notification.m_Message = message;
		notification.m_Type = type;
		notification.m_CreatedOn = std::chrono::system_clock::now();
		notification.m_Duration = duration;
		notification.m_Identifier = message_id;

		if (context_function)
		{
			notification.m_ContextFunc = context_function;
			notification.m_ContextFuncName = context_function_name.empty() ? "Context Function" : context_function_name;
		}

		std::lock_guard<std::mutex> lock(m_mutex);
		auto result = m_Notifications.insert(std::make_pair(title + message, notification));

		return notification;
	}

	bool Notifications::EraseImpl(Notification notification)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		for (auto& [id, n] : m_Notifications)
		{
			if (n.m_Identifier == notification.m_Identifier)
			{
				n.m_Erasing = true;
				return true;
			}
		}

		return false;
	}

	static void DrawNotification(Notification& notification, int position)
	{
		float y_pos = position * 100;
		float x_pos = 10;
		ImVec2 cardSize(m_CardSizeX, m_CardSizeY);

		ImGui::SetNextWindowSize(cardSize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(x_pos + notification.m_AnimationOffset, y_pos + 10), ImGuiCond_Always);

		std::string windowTitle = std::to_string(position);
		ImGui::Begin(windowTitle.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoFocusOnAppearing);

		auto timeElapsed = (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - notification.m_CreatedOn).count();

		auto depletionProgress = 1.0f - (timeElapsed / (float)notification.m_Duration);

		// Determine color based on notification type
		ImVec4 borderColor;
		ImVec4 accentColor;
		const char* icon = "ℹ️";
		
		if (notification.m_Type == NotificationType::Info)
		{
			borderColor = ImVec4(0.2f, 0.6f, 1.0f, 0.8f);      // Cyan-blue
			accentColor = ImVec4(0.3f, 0.7f, 1.0f, 1.0f);     // Bright cyan
			icon = "ℹ️";
		}
		else if (notification.m_Type == NotificationType::Success)
		{
			borderColor = ImVec4(0.0f, 0.8f, 0.2f, 0.8f);      // Green
			accentColor = ImVec4(0.2f, 1.0f, 0.4f, 1.0f);     // Bright green
			icon = "✓";
		}
		else if (notification.m_Type == NotificationType::Warning)
		{
			borderColor = ImVec4(1.0f, 0.7f, 0.0f, 0.8f);      // Orange
			accentColor = ImVec4(1.0f, 0.85f, 0.2f, 1.0f);     // Bright orange
			icon = "⚠️";
		}
		else if (notification.m_Type == NotificationType::Error)
		{
			borderColor = ImVec4(1.0f, 0.2f, 0.2f, 0.8f);      // Red
			accentColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);     // Bright red
			icon = "✕";
		}

		// Draw glowing border effect
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowEnd = ImVec2(windowPos.x + cardSize.x, windowPos.y + cardSize.y);
		
		// Glow effect (multiple layers with decreasing alpha)
		for (int i = 3; i >= 1; --i)
		{
			ImVec4 glowColor = borderColor;
			glowColor.w = 0.3f / i;
			drawList->AddRect(
				ImVec2(windowPos.x - i, windowPos.y - i),
				ImVec2(windowEnd.x + i, windowEnd.y + i),
				ImGui::GetColorU32(glowColor),
				8.0f
			);
		}
		
		// Main border
		drawList->AddRect(windowPos, windowEnd, ImGui::GetColorU32(borderColor), 8.0f, 0, 2.0f);

		// Progress bar with neon purple color
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.616f, 0.306f, 0.867f, 1.0f));
		ImGui::ProgressBar(depletionProgress, ImVec2(-1, 4.0f), "");
		ImGui::PopStyleColor();

		// Title with icon
		ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
		ImGui::TextColored(accentColor, "%s %s", icon, notification.m_Title.c_str());
		ImGui::PopStyleColor();

		ImGui::Separator();

		ImGui::TextWrapped("%s", notification.m_Message.c_str());

		if (notification.m_ContextFunc)
		{
			ImGui::Spacing();
			if (ImGui::Selectable(notification.m_ContextFuncName.c_str()))
				FiberPool::Push([notification] {
					notification.m_ContextFunc();
				});
		}

		ImGui::End();
	}

	void Notifications::DrawImpl()
	{
		std::vector<std::string> keys_to_erase;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			int position = 0;

			for (auto& [id, notification] : m_Notifications)
			{
				DrawNotification(notification, position);

				if (!notification.m_Erasing)
				{
					if (notification.m_AnimationOffset < 0)
						notification.m_AnimationOffset += m_CardAnimationSpeed;

					//Need this to account for changes in card size (x dimension), custom increments might result in odd numbers
					if (notification.m_AnimationOffset > 0)
						notification.m_AnimationOffset = 0.f;
				}
				else
				{
					notification.m_AnimationOffset -= m_CardAnimationSpeed;
					if (notification.m_AnimationOffset <= -m_CardSizeX)
						keys_to_erase.push_back(id);
				}


				if ((float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - notification.m_CreatedOn).count() >= notification.m_Duration)
					keys_to_erase.push_back(id);

				position++;
			}
		}
		std::lock_guard<std::mutex> lock(m_mutex);
		for (const auto& key : keys_to_erase)
		{
			m_Notifications.erase(key);
		}
	}
}
