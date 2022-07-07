#include "pch.h"
#include "ReplayMapChanger.h"

std::string ReplayMapChanger::GetPluginName()
{
	return "Replay Map Changer";
}

void ReplayMapChanger::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void ReplayMapChanger::RenderSettings()
{
	if (isInReplay)
	{
		if (!mapNames.empty())
		{
			static std::string currentMapName = mapNames.begin()->first;
			const char* combo_preview_value = currentMapName.c_str();
			if (ImGui::BeginCombo("##MapsCombo", combo_preview_value))
			{
				for (auto& mapName : mapNames)
				{
					const bool isSelected = currentMapName == mapName.first;
					if (ImGui::Selectable(mapName.first.c_str(), isSelected))
						currentMapName = mapName.first;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Change Map"))
			{
				std::string mapName = mapNames.at(currentMapName);

				gameWrapper->Execute([this, mapName](GameWrapper* gw) {
					LoadReplayWithMap(mapName);
				});
			}
		}
		else
		{
			ImGui::TextUnformatted("Unable to get map list at the moment. Try again later.");
		}
	}
	else
	{
		ImGui::TextUnformatted("Enter a replay first.");
	}
}
