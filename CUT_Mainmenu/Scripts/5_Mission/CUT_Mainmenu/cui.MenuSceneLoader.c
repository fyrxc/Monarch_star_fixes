class CUT_MainmenuSceneLoader
{
	protected static ref CUT_MainmenuSceneLoader s_Instance;

	protected ref array<Object> m_SpawnedObjects;
	protected ref array<PlayerBase> m_Actors;
	protected ref array<int> m_ActorEmotes;
	protected bool m_HidePlayer;
	protected bool m_Applied;
	protected bool m_OnMainMenu;
	protected bool m_MovedCamera;
	protected vector m_CamPos;
	protected vector m_CamOri;
	protected bool m_HasCamOri;
	protected vector m_PlayerPos;
	protected vector m_PlayerOri;
	protected bool m_HasPlayerPos;
	protected int m_PlayerEmote;
	protected string m_PlayerHands;
	protected ref CUT_MainmenuDzeFile m_MappingDze;

	static const int SPAWN_FLAGS = ECE_SETUP | ECE_CREATEPHYSICS | ECE_AIRBORNE | ECE_KEEPHEIGHT | ECE_NOSURFACEALIGN | ECE_LOCAL | ECE_NOPERSISTENCY_WORLD | ECE_NOLIFETIME;
	// Physics + no airborne: airborne dummies never get a move command, so they T-pose
	// and PlayEmote/StartCommand_Action silently does nothing.
	static const int ACTOR_FLAGS = ECE_SETUP | ECE_CREATEPHYSICS | ECE_KEEPHEIGHT | ECE_NOSURFACEALIGN | ECE_LOCAL | ECE_NOPERSISTENCY_WORLD | ECE_NOLIFETIME;
	static const float CAM_MIN_DISTANCE = 3.2;
	static const int EMOTE_USE_CAMPFIRE = -1001;
	static const int EMOTE_INVENTORY = -1002;

	void CUT_MainmenuSceneLoader()
	{
		m_SpawnedObjects = new array<Object>;
		m_Actors = new array<PlayerBase>;
		m_ActorEmotes = new array<int>;
	}

	static CUT_MainmenuSceneLoader Get()
	{
		if (!s_Instance)
			s_Instance = new CUT_MainmenuSceneLoader();
		return s_Instance;
	}

	static void Apply(DayZIntroScenePC intro)
	{
		Get().ApplyInternal(intro);
	}

	static void ApplyPlayerVisibility(bool onMainMenu)
	{
		Get().ApplyPlayerVisibilityInternal(onMainMenu);
	}

	static void ShowPlayerForCharacterMenu()
	{
		Get().ApplyPlayerVisibilityInternal(false);
	}

	static void Stop()
	{
		if (s_Instance)
			s_Instance.StopInternal();
	}

	protected void ApplyInternal(DayZIntroScenePC intro)
	{
		if (m_Applied)
			return;

		CUT_MainmenuSceneConfig cfg = CUT_MainmenuSceneStore.Get();
		if (!cfg || !cfg.enabled)
			return;

		if (!intro)
		{
			Print("[CUT_Mainmenu] scene: intro scene missing");
			return;
		}

		m_Applied = true;
		m_OnMainMenu = true;
		m_HidePlayer = cfg.hidePlayerCharacter;
		ReadPlayerDef(cfg);

		ApplyDateAndWeather(cfg);

		int spawned = LoadAndSpawnMapping(cfg.dze);

		Print("[CUT_Mainmenu] scene: loaded " + spawned.ToString() + " objects");

		if (!ApplyCamera(intro, cfg, m_MappingDze))
			Print("[CUT_Mainmenu] scene: camera skipped (set camera.position in scene.json)");

		ApplyPlayerPlacement(intro);

		if (cfg.npcs && cfg.npcs.Count() > 0)
			SpawnActorList(cfg.npcs);
		else
			LoadActorsFile(cfg.actors);

		// 800ms — fresh menu characters need time before emote commands take.
		// Starting earlier silently fails and leaves everyone in idle/broken pose.
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StartActorEmotes, 1000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StartActorEmotes, 2200, false);
		// GUI queue: the gameplay queue never ticks in the main-menu mission,
		// so the old repeater there never fired and one-shot fullbody anims
		// (USING/campfire) froze the dummy the moment they ended.
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TickActorEmotes, 3500, true);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(FocusCameraOnPlayer, 200, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(FocusCameraOnPlayer, 1100, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(FocusCameraOnPlayer, 2300, false);
	}

	protected void ReadPlayerDef(CUT_MainmenuSceneConfig cfg)
	{
		m_HasPlayerPos = false;
		m_PlayerEmote = 0;
		m_PlayerHands = "";
		if (!cfg.player)
			return;

		m_PlayerPos = FloatsToVec(cfg.player.position);
		m_PlayerOri = FloatsToVec(cfg.player.orientation);
		m_HasPlayerPos = m_PlayerPos != vector.Zero;
		m_PlayerEmote = ResolveEmote(cfg.player.emote);
		m_PlayerHands = cfg.player.hands;
	}

	protected void ApplyDateAndWeather(CUT_MainmenuSceneConfig cfg)
	{
		World world = GetGame().GetWorld();
		if (world && cfg.date && cfg.date.Count() >= 5)
		{
			world.SetDate(cfg.date.Get(0), cfg.date.Get(1), cfg.date.Get(2), cfg.date.Get(3), cfg.date.Get(4));
		}

		Weather weather = GetGame().GetWeather();
		if (!weather)
			return;

		weather.GetOvercast().SetLimits(cfg.overcast, cfg.overcast);
		weather.GetRain().SetLimits(cfg.rain, cfg.rain);
		weather.GetFog().SetLimits(cfg.fog, cfg.fog);
		weather.GetOvercast().Set(cfg.overcast, 0, 0);
		weather.GetRain().Set(cfg.rain, 0, 0);
		weather.GetFog().Set(cfg.fog, 0, 0);
	}

	protected int LoadAndSpawnMapping(string path)
	{
		m_MappingDze = null;

		if (path != "" && FileExist(path) && IsBinnedDze(path))
		{
			Print("[CUT_Mainmenu] scene: binarized DZE - " + path);
			return SpawnBinnedDze(path);
		}

		string trimmed = ReadMappingText(path);
		if (trimmed == "")
			return 0;

		if (IsObjectSpawnerJson(trimmed))
		{
			Print("[CUT_Mainmenu] scene: object-spawner JSON - " + path);
			return SpawnSpawnerJson(trimmed);
		}

		m_MappingDze = ParseDzeText(trimmed, path);
		if (m_MappingDze && m_MappingDze.EditorObjects)
			return SpawnDzeObjects(m_MappingDze.EditorObjects);

		return 0;
	}

	protected string ReadMappingText(string path)
	{
		if (path == "")
			return "";

		if (!FileExist(path))
		{
			Print("[CUT_Mainmenu] scene: mapping missing - " + path);
			return "";
		}

		FileHandle handle = OpenFile(path, FileMode.READ);
		if (handle == 0)
		{
			Print("[CUT_Mainmenu] scene: cannot open mapping - " + path);
			return "";
		}

		string fileContent;
		ReadFile(handle, fileContent, 100000000);
		CloseFile(handle);

		string trimmed = fileContent.Trim();
		if (trimmed.Length() == 0)
		{
			Print("[CUT_Mainmenu] scene: mapping is empty - " + path);
			return "";
		}

		string firstChar = trimmed.Substring(0, 1);
		if (firstChar != "{")
		{
			Print("[CUT_Mainmenu] scene: mapping is binarized. Export Object Spawner JSON or unbinarized DZE - " + path);
			return "";
		}

		return trimmed;
	}

	protected bool IsBinnedDze(string path)
	{
		FileSerializer serializer = new FileSerializer();
		if (!serializer.Open(path, FileMode.READ))
			return false;

		string check;
		bool ok = serializer.Read(check);
		serializer.Close();
		return ok && check == "EditorBinned";
	}

	protected int SpawnBinnedDze(string path)
	{
		FileSerializer serializer = new FileSerializer();
		if (!serializer.Open(path, FileMode.READ))
		{
			Print("[CUT_Mainmenu] scene: cannot open binarized DZE - " + path);
			return 0;
		}

		string check;
		serializer.Read(check);

		int version;
		serializer.Read(version);

		string mapName;
		serializer.Read(mapName);

		vector cam;
		serializer.Read(cam);

		string author;
		serializer.Read(author);

		int credits;
		serializer.Read(credits);
		for (int c = 0; c < credits; c++)
		{
			string credit;
			serializer.Read(credit);
		}

		if (version >= 7)
		{
			int lastModified;
			int created;
			serializer.Read(lastModified);
			serializer.Read(created);
		}

		int count;
		serializer.Read(count);

		m_MappingDze = new CUT_MainmenuDzeFile();
		m_MappingDze.CameraPosition = new array<float>;
		m_MappingDze.CameraPosition.Insert(cam[0]);
		m_MappingDze.CameraPosition.Insert(cam[1]);
		m_MappingDze.CameraPosition.Insert(cam[2]);
		m_MappingDze.EditorObjects = new array<ref CUT_MainmenuDzeObject>;

		int failed;
		for (int i = 0; i < count; i++)
		{
			CUT_MainmenuDzeObject obj = ReadBinnedDzeObject(serializer, version);
			if (obj)
				m_MappingDze.EditorObjects.Insert(obj);
			else
				failed++;
		}

		serializer.Close();

		if (failed > 0)
			Print("[CUT_Mainmenu] scene: " + failed.ToString() + " binarized objects failed to read");

		Print("[CUT_Mainmenu] scene: DZE camera " + cam.ToString() + " map " + mapName);
		return SpawnDzeObjects(m_MappingDze.EditorObjects);
	}

	protected CUT_MainmenuDzeObject ReadBinnedDzeObject(FileSerializer serializer, int version)
	{
		CUT_MainmenuDzeObject obj = new CUT_MainmenuDzeObject();

		string displayName;
		serializer.Read(obj.Type);
		serializer.Read(displayName);

		vector pos;
		vector ori;
		serializer.Read(pos);
		serializer.Read(ori);
		obj.Position = new array<float>;
		obj.Position.Insert(pos[0]);
		obj.Position.Insert(pos[1]);
		obj.Position.Insert(pos[2]);
		obj.Orientation = new array<float>;
		obj.Orientation.Insert(ori[0]);
		obj.Orientation.Insert(ori[1]);
		obj.Orientation.Insert(ori[2]);

		serializer.Read(obj.Scale);

		int flags;
		serializer.Read(flags);

		if (version < 8)
		{
			int attachments;
			serializer.Read(attachments);
			for (int a = 0; a < attachments; a++)
			{
				string attachment;
				serializer.Read(attachment);
			}
		}

		int paramsCount;
		serializer.Read(paramsCount);
		if (paramsCount > 0)
		{
			Print("[CUT_Mainmenu] scene: skipped DZE object with extra params - " + obj.Type);
			return null;
		}

		if (version >= 3)
		{
			bool locked;
			bool allowDamage;
			bool simulate;
			serializer.Read(obj.EditorOnly);
			serializer.Read(locked);
			serializer.Read(allowDamage);
			serializer.Read(simulate);
		}

		if (version >= 5)
		{
			string model;
			serializer.Read(model);
		}

		if (version >= 8)
		{
			int attachmentMap;
			serializer.Read(attachmentMap);
			for (int m = 0; m < attachmentMap; m++)
			{
				int slotId;
				serializer.Read(slotId);
				ReadBinnedDzeObject(serializer, version);
			}
		}

		return obj;
	}

	protected bool IsObjectSpawnerJson(string trimmed)
	{
		return trimmed.IndexOf("\"Objects\"") != -1 && trimmed.IndexOf("\"EditorObjects\"") == -1;
	}

	protected int SpawnSpawnerJson(string trimmed)
	{
		CUT_MainmenuSpawnerFile spawner = new CUT_MainmenuSpawnerFile();
		string errorMessage;
		if (!JsonFileLoader<CUT_MainmenuSpawnerFile>.LoadData(trimmed, spawner, errorMessage))
		{
			Print("[CUT_Mainmenu] scene: object-spawner JSON parse failed - " + errorMessage);
			return 0;
		}

		if (!spawner.Objects)
			return 0;

		int spawned;
		int failed;
		foreach (CUT_MainmenuSpawnerObject obj : spawner.Objects)
		{
			if (!obj || obj.name == "")
				continue;

			if (IsSurvivorType(obj.name))
			{
				Print("[CUT_Mainmenu] scene: skipped mapping survivor " + obj.name);
				continue;
			}

			if (IsCameraMarkerType(obj.name))
				continue;

			vector pos = FloatsToVec(obj.pos);
			vector ori = FloatsToVec(obj.ypr);
			float scale = NormalizeScale(obj.scale);
			Object created = SpawnTyped(obj.name, pos, ori, scale);
			if (created && Math.AbsFloat(scale - 1) >= 0.001)
				Print("[CUT_Mainmenu] scene: " + obj.name + " scale " + scale.ToString());
			if (created)
			{
				m_SpawnedObjects.Insert(created);
				spawned++;
			}
			else
			{
				failed++;
				if (failed <= 12)
					Print("[CUT_Mainmenu] scene: object Type not found - " + obj.name);
			}
		}

		if (failed > 12)
			Print("[CUT_Mainmenu] scene: " + failed.ToString() + " objects failed to spawn");

		return spawned;
	}

	protected CUT_MainmenuDzeFile ParseDzeText(string trimmed, string path)
	{
		CUT_MainmenuDzeFile dze = new CUT_MainmenuDzeFile();
		string errorMessage;
		if (!JsonFileLoader<CUT_MainmenuDzeFile>.LoadData(trimmed, dze, errorMessage))
		{
			Print("[CUT_Mainmenu] scene: DZE JSON parse failed - " + path + " - " + errorMessage);
			return null;
		}

		return dze;
	}

	protected int SpawnDzeObjects(array<ref CUT_MainmenuDzeObject> objects)
	{
		int spawned;
		int failed;
		foreach (CUT_MainmenuDzeObject obj : objects)
		{
			if (!obj || obj.EditorOnly || obj.Type == "")
				continue;

			if (IsSurvivorType(obj.Type))
			{
				Print("[CUT_Mainmenu] scene: skipped DZE survivor " + obj.Type);
				continue;
			}

			if (IsCameraMarkerType(obj.Type))
				continue;

			float scale = NormalizeScale(obj.Scale);
			Object created = SpawnTyped(obj.Type, FloatsToVec(obj.Position), FloatsToVec(obj.Orientation), scale);
			if (created)
			{
				m_SpawnedObjects.Insert(created);
				spawned++;
				if (Math.AbsFloat(scale - 1) >= 0.001)
					Print("[CUT_Mainmenu] scene: " + obj.Type + " scale " + scale.ToString());
			}
			else
			{
				failed++;
				if (failed <= 12)
					Print("[CUT_Mainmenu] scene: object Type not found - " + obj.Type);
			}
		}

		if (failed > 12)
			Print("[CUT_Mainmenu] scene: " + failed.ToString() + " objects failed to spawn");

		return spawned;
	}

	protected vector FloatsToVec(array<float> values)
	{
		if (!values || values.Count() < 3)
			return vector.Zero;
		return Vector(values.Get(0), values.Get(1), values.Get(2));
	}

	protected float NormalizeScale(float scale)
	{
		if (scale <= 0)
			return 1;
		return scale;
	}

	protected string FormatP3DPath(string typeName)
	{
		string path = typeName;
		path.Replace("\\", "/");
		if (path.Length() > 0)
		{
			string first = path.Substring(0, 1);
			if (first == "/")
				path = path.Substring(1, path.Length() - 1);
		}
		return path;
	}

	// Same method DayZ Editor uses: scale the yaw/pitch/roll basis, then SetTransform.
	protected void ApplyEditorScale(Object obj, vector pos, vector ori, float scale)
	{
		if (!obj)
			return;

		obj.SetPosition(pos);
		obj.SetOrientation(ori);
		obj.SetOrientation(ori);
		obj.SetScale(scale);

		if (Math.AbsFloat(scale - 1) >= 0.001)
		{
			vector matrix[4];
			Math3D.YawPitchRollMatrix(ori, matrix);
			matrix[0] = matrix[0] * scale;
			matrix[1] = matrix[1] * scale;
			matrix[2] = matrix[2] * scale;
			matrix[3] = pos;
			obj.SetTransform(matrix);
		}

		obj.Update();
	}

	protected Object SpawnTyped(string typeName, vector pos, vector ori, float scale)
	{
		scale = NormalizeScale(scale);

		Object created;
		if (typeName.Contains(".p3d") || typeName.Contains("\\") || typeName.Contains("/"))
		{
			created = GetGame().CreateStaticObjectUsingP3D(FormatP3DPath(typeName), pos, ori, scale, true);
			return created;
		}

		created = GetGame().CreateObjectEx(typeName, pos, SPAWN_FLAGS);
		if (!created)
			return null;

		ApplyEditorScale(created, pos, ori, scale);
		return created;
	}

	protected bool ApplyCamera(DayZIntroScenePC intro, CUT_MainmenuSceneConfig cfg, CUT_MainmenuDzeFile dze)
	{
		vector camPos = vector.Zero;
		if (cfg.camera)
			camPos = FloatsToVec(cfg.camera.position);
		if (camPos == vector.Zero && cfg.cameraPosition)
			camPos = FloatsToVec(cfg.cameraPosition);
		if (camPos == vector.Zero && dze)
			camPos = FloatsToVec(dze.CameraPosition);

		if (camPos == vector.Zero)
			return false;

		m_CamPos = camPos;
		m_CamOri = vector.Zero;
		m_HasCamOri = false;
		if (cfg.camera)
			m_CamOri = FloatsToVec(cfg.camera.orientation);
		m_HasCamOri = m_CamOri != vector.Zero;
		m_MovedCamera = true;
		Print("[CUT_Mainmenu] scene: camera " + m_CamPos.ToString());
		intro.CUT_ApplySceneCamera(m_CamPos, m_CamOri);
		return true;
	}

	protected void ApplyPlayerPlacement(DayZIntroScenePC intro)
	{
		if (m_HidePlayer)
		{
			intro.CUT_SetPlayerInvisible(true);
			return;
		}

		if (m_HasPlayerPos)
		{
			intro.CUT_PlacePlayerAt(m_PlayerPos, m_PlayerOri);
			Print("[CUT_Mainmenu] scene: player " + m_PlayerPos.ToString());
		}
		else if (m_MovedCamera)
		{
			intro.CUT_PlacePlayerAuto();
		}

		intro.CUT_RefreshOrbitRadius();
		PlayerBase player = intro.CUT_GetPlayer();
		if (player)
		{
			player.SetAllowDamage(false);
			player.DisableSimulation(false);
		}

		intro.CUT_SetPlayerInvisible(m_HidePlayer);
		EquipPlayerFirearm(player);
		if (player && m_PlayerEmote != 0 && GetFirearm(player.GetItemInHands()))
			LockMenuPose(player);
		intro.CUT_ApplySceneCamera(GetPulledCamPos(), vector.Zero);
		intro.CUT_LookAtPlayer();
		intro.CUT_RefreshOrbitRadius();
	}

	protected vector GetPulledCamPos()
	{
		if (!m_HasPlayerPos)
			return m_CamPos;

		vector delta = m_CamPos - m_PlayerPos;
		delta[1] = 0;
		float dist = delta.Length();
		if (dist < 0.05)
			return m_CamPos;
		if (dist < CAM_MIN_DISTANCE)
			delta = delta * (CAM_MIN_DISTANCE / dist);
		return Vector(m_PlayerPos[0] + delta[0], m_CamPos[1], m_PlayerPos[2] + delta[2]);
	}

	protected void EquipPlayerFirearm(PlayerBase player)
	{
		if (!player)
			return;

		if (GetFirearm(player.GetItemInHands()))
			return;

		Weapon_Base found = FindPlayerFirearm(player);
		if (found)
			player.LocalTakeEntityToHands(found);
	}

	protected Weapon_Base GetFirearm(EntityAI item)
	{
		return Weapon_Base.Cast(item);
	}

	protected Weapon_Base FindPlayerFirearm(PlayerBase player)
	{
		if (!player || !player.GetInventory())
			return null;

		array<EntityAI> items = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI item : items)
		{
			Weapon_Base gun = Weapon_Base.Cast(item);
			if (gun)
				return gun;
		}
		return null;
	}

	protected bool IsSurvivorType(string typeName)
	{
		if (typeName == "")
			return false;

		if (GetGame().IsKindOf(typeName, "SurvivorMale_Base") || GetGame().IsKindOf(typeName, "SurvivorFemale_Base") || GetGame().IsKindOf(typeName, "SurvivorBase"))
			return true;

		string lower = typeName;
		lower.ToLower();
		return lower.IndexOf("survivorm_") == 0 || lower.IndexOf("survivorf_") == 0;
	}

	protected bool IsCameraMarkerType(string typeName)
	{
		if (typeName == "")
			return false;

		string lower = typeName;
		lower.ToLower();
		return lower == "apple" || lower == "apple_green" || lower == "apple_dried";
	}

	protected void ApplyPlayerVisibilityInternal(bool onMainMenu)
	{
		if (!m_Applied)
			return;

		m_OnMainMenu = onMainMenu;

		MissionMainMenu mission = MissionMainMenu.Cast(GetGame().GetMission());
		if (!mission)
			return;

		DayZIntroScenePC intro = mission.GetIntroScenePC();
		if (!intro)
			return;

		if (onMainMenu)
		{
			ApplyPlayerPlacement(intro);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(StartActorEmotes);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StartActorEmotes, 1000, false);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StartActorEmotes, 2200, false);
			FocusCameraOnPlayer();
			return;
		}

		PlayerBase player = intro.CUT_GetPlayer();
		UnlockMenuPose(player);
		StopPlayerEmote(player);
		intro.CUT_SetPlayerInvisible(false);
		intro.ResetIntroCamera();
	}

	protected void StopInternal()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(StartActorEmotes);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(TickActorEmotes);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(FocusCameraOnPlayer);
	}

	protected void LoadActorsFile(string path)
	{
		if (path == "" || !FileExist(path))
			return;

		CUT_MainmenuActorsFile file = new CUT_MainmenuActorsFile();
		string errorMessage;
		if (!JsonFileLoader<CUT_MainmenuActorsFile>.LoadFile(path, file, errorMessage))
		{
			Print("[CUT_Mainmenu] scene: actors.json failed - " + errorMessage);
			return;
		}

		if (file.actors)
			SpawnActorList(file.actors);
	}

	protected void SpawnActorList(array<ref CUT_MainmenuActorDef> list)
	{
		if (!list)
			return;

		int spawned;
		foreach (CUT_MainmenuActorDef def : list)
		{
			if (!def || def.type == "")
				continue;

			if (!GetGame().IsKindOf(def.type, "SurvivorMale_Base") && !GetGame().IsKindOf(def.type, "SurvivorFemale_Base"))
			{
				Print("[CUT_Mainmenu] scene: npc is not a survivor (will T-pose) - " + def.type);
			}

			vector pos = FloatsToVec(def.position);
			vector ori = FloatsToVec(def.orientation);
			PlayerBase actor = PlayerBase.Cast(GetGame().CreateObjectEx(def.type, pos, ACTOR_FLAGS));
			if (!actor)
			{
				Print("[CUT_Mainmenu] scene: npc Type not found - " + def.type);
				continue;
			}

			actor.SetPosition(pos);
			actor.SetOrientation(ori);
			actor.SetAllowDamage(false);
			KeepMenuDummySimulated(actor);
			LockMenuPose(actor);
			EquipActor(actor, def.attachments);

			int emoteId = ResolveEmote(def.emote);

			m_Actors.Insert(actor);
			m_ActorEmotes.Insert(emoteId);
			spawned++;
		}

		Print("[CUT_Mainmenu] scene: spawned " + spawned.ToString() + " npcs");
	}

	protected void EquipActor(PlayerBase actor, array<string> attachments)
	{
		if (!actor || !attachments)
			return;

		foreach (string attachment : attachments)
		{
			if (attachment == "")
				continue;

			EntityAI item = actor.GetInventory().CreateAttachment(attachment);
			if (!item)
				item = actor.GetInventory().CreateInInventory(attachment);
			if (!item)
				Print("[CUT_Mainmenu] scene: attachment not found - " + attachment);
		}
	}

	protected int ResolveEmote(string name)
	{
		if (name == "")
			return 0;

		string key = name;
		key.ToUpper();
		key.Replace("ID_EMOTE_", "");
		key.Replace(" ", "_");

		switch (key)
		{
			case "IDLE":
				return 0;
			case "SIT":
			case "SITA":
				return EmoteConstants.ID_EMOTE_SITA;
			case "SIT_CAMP":
			case "SITB":
			case "LEAN":
				return EmoteConstants.ID_EMOTE_SITB;
			case "THROAT":
				return EmoteConstants.ID_EMOTE_THROAT;
			case "SALUTE":
				return EmoteConstants.ID_EMOTE_SALUTE;
			case "SURRENDER":
				return EmoteConstants.ID_EMOTE_SURRENDER;
			case "WATCHING":
				return EmoteConstants.ID_EMOTE_WATCHING;
			case "CAMPFIRE":
				return EmoteConstants.ID_EMOTE_CAMPFIRE;
			case "USING":
			case "USE":
			case "STARTFIRE":
			case "CAMPFIRE_USE":
				return EMOTE_USE_CAMPFIRE;
			case "INVENTORY":
			case "SEARCH":
			case "SEARCHINVENTORY":
			case "RUMMAGE":
				return EMOTE_INVENTORY;
			case "FACEPALM":
				return EmoteConstants.ID_EMOTE_FACEPALM;
			case "LYING":
			case "LYINGDOWN":
				return EmoteConstants.ID_EMOTE_LYINGDOWN;
			case "DANCE":
				return EmoteConstants.ID_EMOTE_DANCE;
			case "GREETING":
				return EmoteConstants.ID_EMOTE_GREETING;
			case "TAUNT":
				return EmoteConstants.ID_EMOTE_TAUNT;
			case "CLAP":
				return EmoteConstants.ID_EMOTE_CLAP;
			case "TIMEOUT":
				return EmoteConstants.ID_EMOTE_TIMEOUT;
			case "SUICIDE":
				return EmoteConstants.ID_EMOTE_SUICIDE;
		}

		Print("[CUT_Mainmenu] scene: unknown emote '" + name + "' - idle");
		return 0;
	}

	void StartActorEmotes()
	{
		PlayActorEmotes(true);
		PlayPlayerEmote(true);
		FocusCameraOnPlayer();
	}

	// Repeating restart for one-shot fullbody anims (USING ends after a
	// while). No camera work here so the timer never snaps the view, and the
	// player pose is skipped while the character menu is showing the real
	// character.
	void TickActorEmotes()
	{
		PlayActorEmotes(true);
		if (m_OnMainMenu)
			PlayPlayerEmote(true);
	}

	void FocusCameraOnPlayer()
	{
		if (m_HidePlayer)
			return;
		MissionMainMenu mission = MissionMainMenu.Cast(GetGame().GetMission());
		if (!mission)
			return;
		DayZIntroScenePC intro = mission.GetIntroScenePC();
		if (!intro)
			return;
		if (m_MovedCamera)
			intro.CUT_ApplySceneCamera(GetPulledCamPos(), vector.Zero);
		intro.CUT_LookAtPlayer();
		intro.CUT_RefreshOrbitRadius();
	}

	protected void PlayActorEmotes(bool force)
	{
		int count = m_Actors.Count();
		for (int i = 0; i < count; i++)
		{
			PlayEmoteOn(m_Actors.Get(i), m_ActorEmotes.Get(i), force);
		}
	}

	protected void PlayPlayerEmote(bool force)
	{
		MissionMainMenu mission = MissionMainMenu.Cast(GetGame().GetMission());
		if (!mission)
			return;

		DayZIntroScenePC intro = mission.GetIntroScenePC();
		if (!intro || m_HidePlayer)
			return;

		PlayerBase player = intro.CUT_GetPlayer();
		if (!player || !GetFirearm(player.GetItemInHands()))
			return;

		PlayEmoteOn(player, m_PlayerEmote, force);
	}

	protected void PlayEmoteOn(PlayerBase actor, int emote, bool force)
	{
		if (!actor || emote == 0)
			return;

		EmoteManager manager = actor.GetEmoteManager();
		if (!manager)
			return;

		LockMenuPose(actor);

		if (emote == EMOTE_USE_CAMPFIRE || emote == EMOTE_INVENTORY)
		{
			ForceEmoteCommand(actor, emote);
			return;
		}

		if (manager.IsEmotePlaying() || IsInEmoteHold(actor))
			return;

		manager.PlayEmote(emote);
		if (manager.IsEmotePlaying() || IsInEmoteHold(actor))
			return;

		Print("[CUT_Mainmenu] scene: PlayEmote did not start, forcing command id " + emote.ToString());
		ForceEmoteCommand(actor, emote);
	}

	protected void ForceEmoteCommand(PlayerBase actor, int emote)
	{
		if (!actor)
			return;

		if (emote == EMOTE_INVENTORY)
		{
			if (CUT_MenuInventoryCB.Cast(actor.GetCommandModifier_Action()))
				return;
			actor.AddCommandModifier_Action(DayZPlayerConstants.CMD_ACTIONMOD_SEARCHINVENTORY, CUT_MenuInventoryCB);
			return;
		}

		if (emote == EMOTE_USE_CAMPFIRE)
		{
			if (IsInEmoteHold(actor) && actor.GetCommand_Action())
				return;
			int fireCmd = DayZPlayerConstants.CMD_ACTIONFB_STARTFIRE;
			int fireMask = DayZPlayerConstants.STANCEMASK_CROUCH;
			actor.StartCommand_Action(fireCmd, CUT_MenuLoopCB, fireMask);
			return;
		}

		if (IsInEmoteHold(actor) || actor.GetCommand_Action() || actor.GetCommandModifier_Action())
			return;

		int cmd = 0;
		int mask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		switch (emote)
		{
			case EmoteConstants.ID_EMOTE_CAMPFIRE:
				cmd = DayZPlayerConstants.CMD_GESTUREFB_CAMPFIRE;
				mask = DayZPlayerConstants.STANCEMASK_CROUCH;
				break;
			case EMOTE_USE_CAMPFIRE:
				cmd = DayZPlayerConstants.CMD_ACTIONFB_STARTFIRE;
				mask = DayZPlayerConstants.STANCEMASK_CROUCH;
				break;
			case EmoteConstants.ID_EMOTE_SITA:
				cmd = DayZPlayerConstants.CMD_GESTUREFB_SITA;
				mask = DayZPlayerConstants.STANCEMASK_CROUCH;
				break;
			case EmoteConstants.ID_EMOTE_SITB:
				cmd = DayZPlayerConstants.CMD_GESTUREFB_SITB;
				mask = DayZPlayerConstants.STANCEMASK_CROUCH;
				break;
			case EmoteConstants.ID_EMOTE_SUICIDE:
				if (GetFirearm(actor.GetItemInHands()) && GetFirearm(actor.GetItemInHands()).IsKindOf("Pistol_Base"))
					cmd = DayZPlayerConstants.CMD_SUICIDEFB_PISTOL;
				else
					cmd = DayZPlayerConstants.CMD_SUICIDEFB_RIFLE;
				mask = DayZPlayerConstants.STANCEMASK_CROUCH;
				break;
			case EmoteConstants.ID_EMOTE_WATCHING:
				cmd = DayZPlayerConstants.CMD_GESTUREFB_WATCHING;
				break;
			default:
				return;
		}

		if (cmd == 0)
			return;

		actor.StartCommand_Action(cmd, EmoteCB, mask);
	}

	protected bool IsInEmoteHold(PlayerBase actor)
	{
		HumanCommandActionCallback cb = GetEmoteCallback(actor);
		if (!cb)
			return false;

		int state = cb.GetState();
		return state == HumanCommandActionCallback.STATE_LOOP_LOOP || state == HumanCommandActionCallback.STATE_LOOP_LOOP2 || state == HumanCommandActionCallback.STATE_LOOP_ACTION;
	}

	protected HumanCommandActionCallback GetEmoteCallback(PlayerBase actor)
	{
		if (!actor)
			return null;

		HumanCommandActionCallback cb = actor.GetCommand_Action();
		if (!cb)
			cb = actor.GetCommandModifier_Action();
		return cb;
	}

	protected void LockMenuPose(PlayerBase actor)
	{
		if (!actor)
			return;

		actor.CUT_MenuPoseLock = true;
		actor.SetAllowDamage(false);
	}

	protected void KeepMenuDummySimulated(PlayerBase actor)
	{
		if (!actor)
			return;

		actor.DisableSimulation(false);
		if (dBodyIsSet(actor))
			dBodyActive(actor, ActiveState.ALWAYS_ACTIVE);
		HumanInputController hic = actor.GetInputController();
		if (hic)
			hic.SetDisabled(true);
	}

	protected void UnlockMenuPose(PlayerBase actor)
	{
		if (!actor)
			return;

		actor.CUT_MenuPoseLock = false;
	}

	protected void StopPlayerEmote(PlayerBase player)
	{
		if (!player)
			return;

		EmoteManager manager = player.GetEmoteManager();
		if (manager && manager.IsEmotePlaying())
			manager.InterruptCallbackCommand();
	}
};
