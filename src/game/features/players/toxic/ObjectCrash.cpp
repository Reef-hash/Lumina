#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	static constexpr Hash PROP_CONE_HASH = 0x0FB81A5E; // prop_roadcone02a

	class ObjectSpawnTest : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto ped = player.GetPed();
						if (!ped || !ped.IsValid())
			{
				LOG(WARNING) << "ObjectSpawnTest: Invalid target ped";
				Notifications::Show("Object Spawn Test", "Invalid target player", NotificationType::Error);
				return;
			}

			auto pos = ped.GetPosition();
			if (pos.x == 0.0f && pos.y == 0.0f)
			{
				LOG(WARNING) << "ObjectSpawnTest: Invalid position";
				return;
			}

			int spawned = 0;

			LOG(INFO) << "ObjectSpawnTest: Starting normal object spawn test...";

			// Apply bypass patches SEKALI sahaja
			if (Pointers.ModelSpawnBypass)
			{
				Pointers.ModelSpawnBypass->Apply();
				LOG(INFO) << "  ModelSpawnBypass: APPLIED";
			}
			if (Pointers.WorldModelSpawnBypass)
			{
				Pointers.WorldModelSpawnBypass->Apply();
				LOG(INFO) << "  WorldModelSpawnBypass: APPLIED";
			}
			if (Pointers.PseudoObjectCheck)
			{
				Pointers.PseudoObjectCheck->Apply();
				LOG(INFO) << "  PseudoObjectCheck: APPLIED";
			}

			// Load model
			LOG(INFO) << "ObjectSpawnTest: Requesting model 0x" << std::hex << PROP_CONE_HASH;
			STREAMING::REQUEST_MODEL(PROP_CONE_HASH);

			bool modelLoaded = false;
			for (int i = 0; i < 120; ++i)   // max ~2.4 saat
			{
				if (STREAMING::HAS_MODEL_LOADED(PROP_CONE_HASH))
				{
					modelLoaded = true;
					break;
				}
								ScriptMgr::Yield(std::chrono::milliseconds(0));
				STREAMING::REQUEST_MODEL(PROP_CONE_HASH);
			}

			if (!modelLoaded)
			{
				LOG(WARNING) << "ObjectSpawnTest: Model load TIMED OUT";
				Notifications::Show("Object Spawn Test", "Failed to load cone model", NotificationType::Error);
				goto cleanup;
			}

			LOG(INFO) << "ObjectSpawnTest: Model loaded successfully";

			// Spawn 8 object secara perlahan
			for (int i = 0; i < 8; ++i)
			{
				float offsetX = (i % 4) * 1.5f;
				float offsetY = (i / 4) * 1.5f;

				// Spawn di atas player supaya nampak jelas
				Vector3 spawnPos = { pos.x + offsetX, pos.y + offsetY, pos.z + 2.0f };

				auto handle = OBJECT::CREATE_OBJECT_NO_OFFSET(
					PROP_CONE_HASH, 
					spawnPos.x, spawnPos.y, spawnPos.z, 
					true, false, false, 0
				);

				if (handle != 0 && ENTITY::DOES_ENTITY_EXIST(handle))
				{
					ENTITY::SET_ENTITY_VISIBLE(handle, TRUE, FALSE);   // buat visible supaya kita boleh tengok
					ENTITY::FREEZE_ENTITY_POSITION(handle, FALSE);

					// Force kecil supaya tak terbang teruk
					ENTITY::APPLY_FORCE_TO_ENTITY(handle, 1, 0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0, 
						TRUE, TRUE, TRUE, FALSE, TRUE);

					spawned++;
					LOG(INFO) << "ObjectSpawnTest: Spawned cone " << i << " | Handle: " << handle;
				}
				else
				{
					LOG(WARNING) << "ObjectSpawnTest: Failed to create cone " << i;
				}

								ScriptMgr::Yield(std::chrono::milliseconds(20));   // beri masa game bernafas
			}

			LOG(INFO) << "ObjectSpawnTest: Successfully spawned " << spawned << " normal objects";

			if (spawned > 0)
				Notifications::Show("Object Spawn Test", std::format("Berjaya spawn {} traffic cones", spawned), NotificationType::Success);
			else
				Notifications::Show("Object Spawn Test", "Tiada object berjaya di-spawn", NotificationType::Error);

		cleanup:
			// Restore patches
			if (Pointers.ModelSpawnBypass)      Pointers.ModelSpawnBypass->Restore();
			if (Pointers.WorldModelSpawnBypass) Pointers.WorldModelSpawnBypass->Restore();
			if (Pointers.PseudoObjectCheck)     Pointers.PseudoObjectCheck->Restore();

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(PROP_CONE_HASH);
			LOG(INFO) << "ObjectSpawnTest: All patches restored";
		}
	};

	static ObjectSpawnTest _ObjectSpawnTest{"objecttest", "Object Spawn Test", "Test spawn normal object (prop_roadcone02a) - Check if basic spawn works"};
}