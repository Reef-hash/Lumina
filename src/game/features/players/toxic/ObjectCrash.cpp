#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/backend/ScriptMgr.hpp"     // <--- penting untuk Yield

namespace YimMenu::Features
{
	static constexpr Hash PROP_CONE_HASH = 3258159972;   // prop_roadcone02a

	class ObjectSpawnTest : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();                    // guna Self supaya lebih selamat
			if (!ped || !ped.IsValid())
			{
				LOG(WARNING) << "ObjectSpawnTest: Invalid local ped";
				Notifications::Show("Object Spawn Test", "Invalid local ped", NotificationType::Error);
				return;
			}

			auto pos = ped.GetPosition();
			if (pos.x == 0.0f && pos.y == 0.0f)
			{
				LOG(WARNING) << "ObjectSpawnTest: Invalid position";
				return;
			}

			int spawned = 0;

			LOG(INFO) << "ObjectSpawnTest: Starting normal object spawn test (hash: " << PROP_CONE_HASH << ")...";

			// Apply bypass SEKALI sahaja
			if (Pointers.ModelSpawnBypass)      Pointers.ModelSpawnBypass->Apply();
			if (Pointers.WorldModelSpawnBypass) Pointers.WorldModelSpawnBypass->Apply();
			if (Pointers.PseudoObjectCheck)     Pointers.PseudoObjectCheck->Apply();

			// Request model
			STREAMING::REQUEST_MODEL(PROP_CONE_HASH);
			for (int i = 0; i < 180; ++i)
			{
				if (STREAMING::HAS_MODEL_LOADED(PROP_CONE_HASH))
					break;

				ScriptMgr::Yield(std::chrono::milliseconds(0));
				STREAMING::REQUEST_MODEL(PROP_CONE_HASH);
			}

			if (!STREAMING::HAS_MODEL_LOADED(PROP_CONE_HASH))
			{
				LOG(WARNING) << "ObjectSpawnTest: Model load TIMED OUT";
				Notifications::Show("Object Spawn Test", "Failed to load model", NotificationType::Error);
				goto cleanup;
			}

			LOG(INFO) << "ObjectSpawnTest: Model loaded successfully";

			// Spawn loop
			for (int i = 0; i < 5; ++i)        // hanya 5 object dulu
			{
				Vector3 spawnPos = pos;
				spawnPos.x += (i % 3) * 3.0f;
				spawnPos.y += (i / 3) * 3.0f;
				spawnPos.z += 5.0f;

				auto obj = Object::Create(PROP_CONE_HASH, spawnPos);

				if (obj.IsValid())                 // guna .IsValid() dari class Object
				{
					ENTITY::SET_ENTITY_VISIBLE(obj.GetHandle(), TRUE, FALSE);
					ENTITY::FREEZE_ENTITY_POSITION(obj.GetHandle(), FALSE);

					spawned++;
					LOG(INFO) << "ObjectSpawnTest: Spawned cone " << i << " | Handle: " << obj.GetHandle();
				}
				else
				{
					LOG(WARNING) << "ObjectSpawnTest: Failed to create cone " << i;
				}

				ScriptMgr::Yield(std::chrono::milliseconds(30));
			}

			LOG(INFO) << "ObjectSpawnTest: Successfully spawned " << spawned << " normal objects";

			if (spawned > 0)
				Notifications::Show("Object Spawn Test", std::format("Berjaya spawn {} cones", spawned), NotificationType::Success);
			else
				Notifications::Show("Object Spawn Test", "Tiada object berjaya di-spawn", NotificationType::Error);

		cleanup:
			// Restore bypass
			if (Pointers.ModelSpawnBypass)      Pointers.ModelSpawnBypass->Restore();
			if (Pointers.WorldModelSpawnBypass) Pointers.WorldModelSpawnBypass->Restore();
			if (Pointers.PseudoObjectCheck)     Pointers.PseudoObjectCheck->Restore();

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(PROP_CONE_HASH);
			LOG(INFO) << "ObjectSpawnTest: All patches restored";
		}
	};

	static ObjectSpawnTest _ObjectSpawnTest{"objecttest", "Object Spawn Test", "Test spawn normal cone using Object::Create"};
}