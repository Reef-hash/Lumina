#include "core/commands/LoopedCommand.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class BlacklistBypass : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (Pointers.ModelSpawnBypass)
				Pointers.ModelSpawnBypass->Apply();
			if (Pointers.WorldModelSpawnBypass)
				Pointers.WorldModelSpawnBypass->Apply();
			if (Pointers.PseudoObjectCheck)
				Pointers.PseudoObjectCheck->Apply();
		}

		virtual void OnDisable() override
		{
			if (Pointers.ModelSpawnBypass)
				Pointers.ModelSpawnBypass->Restore();
			if (Pointers.WorldModelSpawnBypass)
				Pointers.WorldModelSpawnBypass->Restore();
			if (Pointers.PseudoObjectCheck)
				Pointers.PseudoObjectCheck->Restore();
		}
	};

	static BlacklistBypass _BlacklistBypass{"blacklistbypass", "Blacklist Bypass", "Bypass blacklisted models and objects, allowing you to spawn any model. Also benefits co-loaded menus."};
}
