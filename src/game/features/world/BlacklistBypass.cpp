#include "core/commands/LoopedCommand.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class BlacklistBypass : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		bool m_patchesApplied = false;

		virtual void OnTick() override
		{
			// Hanya apply sekali, bukan setiap frame
			if (!m_patchesApplied)
			{
				if (Pointers.ModelSpawnBypass)
					Pointers.ModelSpawnBypass->Apply();

				if (Pointers.WorldModelSpawnBypass)
					Pointers.WorldModelSpawnBypass->Apply();

				if (Pointers.PseudoObjectCheck)
					Pointers.PseudoObjectCheck->Apply();

				m_patchesApplied = true;
				LOG(INFO) << "Blacklist Bypass patches applied successfully.";
			}
		}

		virtual void OnDisable() override
		{
			if (m_patchesApplied)
			{
				if (Pointers.ModelSpawnBypass)
					Pointers.ModelSpawnBypass->Restore();

				if (Pointers.WorldModelSpawnBypass)
					Pointers.WorldModelSpawnBypass->Restore();

				if (Pointers.PseudoObjectCheck)
					Pointers.PseudoObjectCheck->Restore();

				m_patchesApplied = false;
				LOG(INFO) << "Blacklist Bypass patches restored.";
			}
		}
	};

	static BlacklistBypass _BlacklistBypass{"blacklistbypass", "Blacklist Bypass", "Bypass blacklisted models and objects..."};
}