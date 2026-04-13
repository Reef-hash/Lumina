#include "core/commands/StringCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	// Reuse StringCommand yang sudah ada
	// static StringCommand _ObjectModelName{"objectmodelname", "Object Model", "The model name of the object you wish to spawn."};

	class ObjectSpawnTest : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			// Guna hash decimal seperti yang kau cadangkan
			Hash modelHash = 3258159972;   // prop_roadcone02a

			LOG(INFO) << "ObjectSpawnTest: Trying to spawn model " << modelHash;

			// Check whitelist macam SpawnObject sedia ada
			if (!STREAMING::IS_MODEL_IN_CDIMAGE(modelHash))
			{
				Notifications::Show("Object Spawn Test", "Model not in CD image (whitelist failed)", NotificationType::Error);
				return;
			}

			// Apply bypass SEKALI (kalau nak test blacklist later)
			if (Pointers.ModelSpawnBypass)      Pointers.ModelSpawnBypass->Apply();
			if (Pointers.WorldModelSpawnBypass) Pointers.WorldModelSpawnBypass->Apply();
			if (Pointers.PseudoObjectCheck)     Pointers.PseudoObjectCheck->Apply();

			// Request model
			STREAMING::REQUEST_MODEL(modelHash);
			for (int i = 0; i < 150; ++i)
			{
				if (STREAMING::HAS_MODEL_LOADED(modelHash))
					break;
				ScriptMgr::Yield(std::chrono::milliseconds(0));
				STREAMING::REQUEST_MODEL(modelHash);
			}

			if (!STREAMING::HAS_MODEL_LOADED(modelHash))
			{
				Notifications::Show("Object Spawn Test", "Model failed to load", NotificationType::Error);
				goto cleanup;
			}

			// Spawn guna Object::Create (sama style dengan SpawnObject sedia ada)
			auto spawnPos = Self::GetPed().GetPosition();
			spawnPos.z += 4.0f;   // spawn di atas sikit

			auto handle = Object::Create(modelHash, spawnPos);

			if (handle != 0 && ENTITY::DOES_ENTITY_EXIST(handle))
			{
				ENTITY::SET_ENTITY_VISIBLE(handle, TRUE, FALSE);
				LOG(INFO) << "ObjectSpawnTest: Successfully spawned object | Handle: " << handle;
				Notifications::Show("Object Spawn Test", "Berjaya spawn cone!", NotificationType::Success);
			}
			else
			{
				LOG(WARNING) << "ObjectSpawnTest: Object::Create returned invalid handle";
				Notifications::Show("Object Spawn Test", "Failed to create object", NotificationType::Error);
			}

		cleanup:
			// Restore bypass
			if (Pointers.ModelSpawnBypass)      Pointers.ModelSpawnBypass->Restore();
			if (Pointers.WorldModelSpawnBypass) Pointers.WorldModelSpawnBypass->Restore();
			if (Pointers.PseudoObjectCheck)     Pointers.PseudoObjectCheck->Restore();

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
		}
	};

	static ObjectSpawnTest _ObjectSpawnTest{"objecttest", "Object Spawn Test", "Test spawn normal cone using Object::Create (whitelist style)"};
}