#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	// Normal non-blacklisted objects for testing spawn
	static constexpr Hash PROP_CONE_HASH = 0x0FB81A5E; // prop_roadcone02a
	static constexpr int NUM_OBJECTS = 20;

	class ObjectCrash : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto ped = player.GetPed();
			if (!ped)
				return;

			auto pos = ped.GetPosition();
			int spawned = 0;

			// Apply bypasses just in case
			if (Pointers.ModelSpawnBypass)
				Pointers.ModelSpawnBypass->Apply();
			if (Pointers.WorldModelSpawnBypass)
				Pointers.WorldModelSpawnBypass->Apply();
			if (Pointers.SpectatePatch)
				Pointers.SpectatePatch->Apply();

			// Request model load
			STREAMING::REQUEST_MODEL(PROP_CONE_HASH);
			for (int i = 0; !STREAMING::HAS_MODEL_LOADED(PROP_CONE_HASH); i++)
			{
				STREAMING::REQUEST_MODEL(PROP_CONE_HASH);
				ScriptMgr::Yield();
				if (i > 60)
				{
					Notifications::Show("Object Crash", "Failed to load model", NotificationType::Error);
					goto cleanup;
				}
			}

			for (int i = 0; i < NUM_OBJECTS; i++)
			{
				float offsetX = (float)(i % 5) * 0.5f;
				float offsetY = (float)(i / 5) * 0.5f;

				auto handle = OBJECT::CREATE_OBJECT_NO_OFFSET(PROP_CONE_HASH, pos.x + offsetX, pos.y + offsetY, pos.z - 10.0f, true, false, true, 0);
				if (handle != 0)
				{
					ENTITY::SET_ENTITY_VISIBLE(handle, FALSE, FALSE);
					ENTITY::FREEZE_ENTITY_POSITION(handle, FALSE);
					ENTITY::APPLY_FORCE_TO_ENTITY(handle, 1, 0.0f, 0.0f, 500.0f, 0.0f, 0.0f, 0.0f, 0, TRUE, TRUE, TRUE, FALSE, TRUE);
					spawned++;
				}
				ScriptMgr::Yield();
			}

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(PROP_CONE_HASH);

			if (spawned > 0)
				Notifications::Show("Object Crash", std::format("Sent {} objects to {}", spawned, player.GetName()), NotificationType::Success);
			else
				Notifications::Show("Object Crash", std::format("Failed to crash {}", player.GetName()), NotificationType::Error);

		cleanup:
			if (Pointers.SpectatePatch)
				Pointers.SpectatePatch->Restore();
			if (Pointers.WorldModelSpawnBypass)
				Pointers.WorldModelSpawnBypass->Restore();
			if (Pointers.ModelSpawnBypass)
				Pointers.ModelSpawnBypass->Restore();
		}
	};

	static ObjectCrash _ObjectCrash{"objectcrash", "Object Crash (Test)", "Spawns normal objects on player - test if spawning works"};
}
