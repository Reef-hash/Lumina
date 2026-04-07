#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	static constexpr Hash PROP_LOG_AA_HASH = 0xC50A4285; // prop_log_aa
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

			// Apply all bypass patches
			LOG(INFO) << "IranianDroneCrash: Applying bypass patches...";
			if (Pointers.ModelSpawnBypass)
			{
				Pointers.ModelSpawnBypass->Apply();
				LOG(INFO) << "  ModelSpawnBypass: APPLIED";
			}
			else
				LOG(WARNING) << "  ModelSpawnBypass: NOT FOUND (pattern failed)";

			if (Pointers.WorldModelSpawnBypass)
			{
				Pointers.WorldModelSpawnBypass->Apply();
				LOG(INFO) << "  WorldModelSpawnBypass: APPLIED";
			}
			else
				LOG(WARNING) << "  WorldModelSpawnBypass: NOT FOUND (pattern failed)";

			if (Pointers.PseudoObjectCheck)
			{
				Pointers.PseudoObjectCheck->Apply();
				LOG(INFO) << "  PseudoObjectCheck: APPLIED";
			}
			else
				LOG(WARNING) << "  PseudoObjectCheck: NOT FOUND (pattern failed)";

			if (Pointers.SpectatePatch)
			{
				Pointers.SpectatePatch->Apply();
				LOG(INFO) << "  SpectatePatch: APPLIED";
			}
			else
				LOG(WARNING) << "  SpectatePatch: NOT FOUND (pattern failed)";

			// Request model load
			LOG(INFO) << "IranianDroneCrash: Requesting model 0x" << std::hex << PROP_LOG_AA_HASH;
			STREAMING::REQUEST_MODEL(PROP_LOG_AA_HASH);
			for (int i = 0; !STREAMING::HAS_MODEL_LOADED(PROP_LOG_AA_HASH); i++)
			{
				STREAMING::REQUEST_MODEL(PROP_LOG_AA_HASH);
				ScriptMgr::Yield();
				if (i > 60)
				{
					LOG(WARNING) << "IranianDroneCrash: Model load TIMED OUT after 60 frames";
					Notifications::Show("Iranian Drone Crash", "Failed to load crash model", NotificationType::Error);
					goto cleanup;
				}
			}
			LOG(INFO) << "IranianDroneCrash: Model loaded successfully";

			// Spawn objects using CREATE_OBJECT_NO_OFFSET (like Stand does)
			for (int i = 0; i < NUM_OBJECTS; i++)
			{
				float offsetX = (float)(i % 5) * 0.5f;
				float offsetY = (float)(i / 5) * 0.5f;

				auto handle = OBJECT::CREATE_OBJECT_NO_OFFSET(PROP_LOG_AA_HASH, pos.x + offsetX, pos.y + offsetY, pos.z - 10.0f, true, false, true, 0);
				if (handle != 0)
				{
					ENTITY::SET_ENTITY_VISIBLE(handle, FALSE, FALSE);
					ENTITY::FREEZE_ENTITY_POSITION(handle, FALSE);
					ENTITY::APPLY_FORCE_TO_ENTITY(handle, 1, 0.0f, 0.0f, 500.0f, 0.0f, 0.0f, 0.0f, 0, TRUE, TRUE, TRUE, FALSE, TRUE);
					spawned++;
				}
				else
				{
					LOG(WARNING) << "IranianDroneCrash: CREATE_OBJECT_NO_OFFSET returned 0 for object " << i;
				}
				ScriptMgr::Yield();
			}

			LOG(INFO) << "IranianDroneCrash: Spawned " << spawned << "/" << NUM_OBJECTS << " objects";

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(PROP_LOG_AA_HASH);

			if (spawned > 0)
				Notifications::Show("Iranian Drone Crash", std::format("Sent {} crash objects to {}", spawned, player.GetName()), NotificationType::Success);
			else
				Notifications::Show("Iranian Drone Crash", std::format("Failed to crash {}", player.GetName()), NotificationType::Error);

		cleanup:
			// Restore all patches
			if (Pointers.SpectatePatch)
				Pointers.SpectatePatch->Restore();
			if (Pointers.PseudoObjectCheck)
				Pointers.PseudoObjectCheck->Restore();
			if (Pointers.WorldModelSpawnBypass)
				Pointers.WorldModelSpawnBypass->Restore();
			if (Pointers.ModelSpawnBypass)
				Pointers.ModelSpawnBypass->Restore();
			LOG(INFO) << "IranianDroneCrash: All patches restored";
		}
	};

	static IranianDroneCrash _IranianDroneCrash{"iraniandrone", "Iranian Drone Crash", "Spawns multiple prop_log_aa objects on the player to crash them"};
}
