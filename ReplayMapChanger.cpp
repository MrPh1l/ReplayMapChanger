#include "pch.h"
#include "ReplayMapChanger.h"
#include "bakkesmod/wrappers/GameObject/ReplayManagerWrapper.h"

BAKKESMOD_PLUGIN(ReplayMapChanger, "Allows to change current map while in a replay", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void ReplayMapChanger::onLoad()
{
	_globalCvarManager = cvarManager;
	isInReplay = !gameWrapper->GetGameEventAsReplay().IsNull();

	gameWrapper->HookEventPost("Function TAGame.GameInfo_Replay_TA.InitGame", [this](std::string eventName) {
		isInReplay = true;
	});
	gameWrapper->HookEvent("Function TAGame.GameInfo_Replay_TA.Destroyed", [this](std::string eventName) {
		isInReplay = false;
	});

	CurlRequest req;
	req.url = "https://raw.githubusercontent.com/MrPh1l/ReplayMapChanger/master/data/ReplayMapChanger/MapNames.json";
	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
	{
		if (code == 200 && json::accept(result))
		{
			json jsonResult = json::parse(result);
			mapNames.clear();

			for (auto& [key, value] : jsonResult.items())
			{
				mapNames.emplace(key, value);
			}
		}
		else // Invalid json. Use local file
		{
			if (std::filesystem::exists({ gameWrapper->GetDataFolder() / "ReplayMapChanger" / "MapNames.json" }))
			{
				std::ifstream stream(gameWrapper->GetDataFolder() / "ReplayMapChanger" / "MapNames.json");

				try
				{
					json jsonResult = json::parse(stream);
					mapNames.clear();

					for (auto& [key, value] : jsonResult.items())
					{
						mapNames.emplace(key, value);
					}
				}
				catch (json::parse_error& ex) {}
			}
		}
	});
}

void ReplayMapChanger::onUnload()
{
	gameWrapper->UnhookEventPost("Function TAGame.GameInfo_Replay_TA.InitGame");
	gameWrapper->UnhookEvent("Function TAGame.GameInfo_Replay_TA.Destroyed");
}

void ReplayMapChanger::LoadReplayWithMap(std::string mapFilename)
{
	if (!isInReplay) return;

	ReplayServerWrapper serverReplay = gameWrapper->GetGameEventAsReplay();
	if (serverReplay.IsNull()) return;

	ReplayWrapper replay = serverReplay.GetReplay();
	if (replay.IsNull()) return;

	ReplayManagerWrapper const replayManager = gameWrapper->GetReplayManagerWrapper();
	replayManager.PlayReplay(replay, mapFilename);
}
