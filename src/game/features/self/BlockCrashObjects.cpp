#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Pools.hpp"

namespace YimMenu::Features
{
	static constexpr uint32_t PROP_LOG_AA_HASH = 3305783941u;

	class BlockCrashObjects : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			auto myPos = ped.GetPosition();
			int blocked = 0;

			for (auto obj : Pools::GetObjects())
			{
				if (!obj)
					continue;

				if (obj.GetModel() != PROP_LOG_AA_HASH)
					continue;

				auto objPos = obj.GetPosition();
				float dx = objPos.x - myPos.x;
				float dy = objPos.y - myPos.y;
				float dz = objPos.z - myPos.z;
				float dist = dx * dx + dy * dy + dz * dz;

				if (dist < 2500.0f) // within 50m radius
				{
					obj.Delete();
					blocked++;
				}
			}

			if (blocked > 0)
			{
				Notifications::Show("Protection", std::format("Blocked {} crash object(s) (prop_log_aa)", blocked), NotificationType::Warning);
				LOGF(WARNING, "Blocked {} prop_log_aa crash objects near local player", blocked);
			}
		}
	};

	static BlockCrashObjects _BlockCrashObjects{"blockcrashobjects", "Block Crash Objects", "Detects and removes prop_log_aa crash objects near you"};
}
