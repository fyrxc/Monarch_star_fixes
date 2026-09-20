class CUT_MenuUISoundFile
{
	int enabled;
	int volume;
}

class CUT_MenuUISound
{
	const static string ProfileFolder = "$profile:\\CRASHOUT\\CUT_Mainmenu\\";
	const static string SettingsPath = "$profile:\\CRASHOUT\\CUT_Mainmenu\\ui_sounds.json";
	const static string CrashoutRoot = "$profile:\\CRASHOUT\\";
	const static int DETAIL_ENABLE = 88344101;
	const static int DETAIL_VOLUME = 88344102;
	const static string HeaderSet = "CUT_Header_Click_UI_SoundSet";
	const static string LightSet = "CUT_Light_Click_UI_SoundSet";

	protected static ref CUT_MenuUISound s_Instance;
	protected int m_Enabled;
	protected int m_Volume;

	static CUT_MenuUISound Get()
	{
		if (!s_Instance)
			s_Instance = new CUT_MenuUISound();
		return s_Instance;
	}

	void CUT_MenuUISound()
	{
		m_Enabled = 1;
		m_Volume = 70;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		Load();
	}

	static array<string> EnableLabels()
	{
		array<string> labels = new array<string>;
		labels.Insert("Off");
		labels.Insert("On");
		return labels;
	}

	int GetEnabled()
	{
		return m_Enabled;
	}

	int GetVolume()
	{
		return m_Volume;
	}

	void SetEnabled(int enabled, bool persist)
	{
		m_Enabled = enabled;
		if (m_Enabled < 0)
			m_Enabled = 0;
		if (m_Enabled > 1)
			m_Enabled = 1;
		if (persist)
			Save();
	}

	void SetVolume(int volume, bool persist)
	{
		m_Volume = volume;
		if (m_Volume < 0)
			m_Volume = 0;
		if (m_Volume > 100)
			m_Volume = 100;
		if (persist)
			Save();
	}

	void EnsureFolder()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		if (!FileExist(CrashoutRoot))
			MakeDirectory(CrashoutRoot);
		if (!FileExist(ProfileFolder))
			MakeDirectory(ProfileFolder);
	}

	void Load()
	{
		m_Enabled = 1;
		m_Volume = 70;
		if (!FileExist(SettingsPath))
			return;
		CUT_MenuUISoundFile file = new CUT_MenuUISoundFile;
		JsonFileLoader<CUT_MenuUISoundFile>.JsonLoadFile(SettingsPath, file);
		if (!file)
			return;
		m_Enabled = file.enabled;
		m_Volume = file.volume;
		if (m_Enabled < 0)
			m_Enabled = 0;
		if (m_Enabled > 1)
			m_Enabled = 1;
		if (m_Volume < 0)
			m_Volume = 70;
		if (m_Volume > 100)
			m_Volume = 100;
	}

	void Save()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		EnsureFolder();
		CUT_MenuUISoundFile file = new CUT_MenuUISoundFile;
		file.enabled = m_Enabled;
		file.volume = m_Volume;
		JsonFileLoader<CUT_MenuUISoundFile>.JsonSaveFile(SettingsPath, file);
	}

	static void PlayHeader()
	{
		Get().Play(HeaderSet);
	}

	static void PlayLight()
	{
		Get().Play(LightSet);
	}

	void Play(string soundSet)
	{
		if (soundSet == "")
			return;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		if (m_Enabled != 1)
			return;
		float vol01 = m_Volume / 100.0;
		if (vol01 <= 0.001)
			return;
		float vol = vol01 * vol01;
		vector pos = Vector(0, 0, 0);
		Man man = GetGame().GetPlayer();
		if (man)
			pos = man.GetPosition();
		EffectSound sound = SEffectManager.CreateSound(soundSet, pos);
		if (!sound)
			return;
		sound.SetSoundMaxVolume(vol);
		sound.SetSoundVolume(vol);
		sound.SetAutodestroy(true);
		sound.SoundPlay();
	}
}
