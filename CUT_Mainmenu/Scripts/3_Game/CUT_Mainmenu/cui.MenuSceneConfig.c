class CUT_MainmenuCameraDef
{
	ref array<float> position;
	ref array<float> orientation;
};

class CUT_MainmenuPlayerDef
{
	ref array<float> position;
	ref array<float> orientation;
	string emote;
	string hands;
};

class CUT_MainmenuActorDef
{
	string type;
	ref array<float> position;
	ref array<float> orientation;
	ref array<string> attachments;
	string emote;
};

class CUT_MainmenuSceneConfig
{
	string note;
	bool enabled;
	string world;
	string dze;
	string actors;
	bool useDzeCamera;
	bool hidePlayerCharacter;
	ref CUT_MainmenuCameraDef camera;
	ref CUT_MainmenuPlayerDef player;
	ref array<ref CUT_MainmenuActorDef> npcs;
	ref array<int> date;
	ref array<float> cameraPosition;
	float overcast;
	float fog;
	float rain;
};

class CUT_MainmenuDzeObject
{
	string Type;
	ref array<float> Position;
	ref array<float> Orientation;
	float Scale;
	bool EditorOnly;
};

class CUT_MainmenuDzeFile
{
	ref array<ref CUT_MainmenuDzeObject> EditorObjects;
	ref array<float> CameraPosition;
	ref array<float> CameraOrientation;
};

class CUT_MainmenuSpawnerObject
{
	string name;
	ref array<float> pos;
	ref array<float> ypr;
	float scale;
};

class CUT_MainmenuSpawnerFile
{
	ref array<ref CUT_MainmenuSpawnerObject> Objects;
};

class CUT_MainmenuActorsFile
{
	ref array<ref CUT_MainmenuActorDef> actors;
};

class CUT_MainmenuSceneStore
{
	protected static const string CONFIG_PATH = "CUT_Mainmenu/Scenes/scene.json";
	protected static ref CUT_MainmenuSceneConfig s_Config;
	protected static bool s_Tried;

	static void EnsureLoaded()
	{
		if (s_Tried)
			return;

		s_Tried = true;
		s_Config = new CUT_MainmenuSceneConfig();

		if (!FileExist(CONFIG_PATH))
		{
			Print("[CUT_Mainmenu] scene: missing " + CONFIG_PATH + " - vanilla intro");
			return;
		}

		string errorMessage;
		if (!JsonFileLoader<CUT_MainmenuSceneConfig>.LoadFile(CONFIG_PATH, s_Config, errorMessage))
		{
			Print("[CUT_Mainmenu] scene: failed to read scene.json - " + errorMessage);
			s_Config = new CUT_MainmenuSceneConfig();
			return;
		}

		if (s_Config.dze == "")
			s_Config.dze = "CUT_Mainmenu/Scenes/menu.json";
		if (s_Config.actors == "")
			s_Config.actors = "CUT_Mainmenu/Scenes/actors.json";
	}

	static CUT_MainmenuSceneConfig Get()
	{
		EnsureLoaded();
		return s_Config;
	}

	static bool IsEnabled()
	{
		CUT_MainmenuSceneConfig cfg = Get();
		return cfg && cfg.enabled;
	}
};

modded class DayZGame
{
	override void MainMenuLaunch()
	{
		CUT_MainmenuSceneStore.EnsureLoaded();
		CUT_MainmenuSceneConfig cfg = CUT_MainmenuSceneStore.Get();
		if (cfg && cfg.enabled && cfg.world != "")
		{
			SetMainMenuWorld(cfg.world);
			Print("[CUT_Mainmenu] scene: intro world pinned to " + cfg.world);
		}

		super.MainMenuLaunch();
	}
};
