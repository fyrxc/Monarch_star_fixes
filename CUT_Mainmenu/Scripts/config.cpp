
class CfgPatches
{
	class CUT_Mainmenu
	{
        requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Sounds_Effects"};
	};
};

class CfgAddons
{
	class PreloadBanks{};
	class PreloadAddons
	{
		class DayZ
		{
			list[] = {"DZ_Data","DZ_Scripts","DZ_UI","DZ_UIFonts","DZ_Sounds_Effects","COLORFUL_HUD"};
		};
	};
};

class CfgMods 
{
	class CUT_Mainmenu
	{
		name = "CUT Mainmenu";
		dir = "CUT_Mainmenu";
		creditsJson = "CUT_Mainmenu/Scripts/Credits.json";
		inputs = "CUT_Mainmenu/Scripts/Inputs.xml";
		type = "mod";

		dependencies[] =
		{
			"Game", "World", "Mission"
		};
		class defs
		{
			class imageSets
			{
				files[]= {"CUT_Mainmenu/GUI/imagesets/CUI.imageset"};
			};
			class gameScriptModule
			{
				files[] = 
				{
					"CUT_Mainmenu/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				files[] = 
				{
					"CUT_Mainmenu/scripts/4_World"
				};
			};
			class missionScriptModule 
			{
				files[] = 
				{
					"CUT_Mainmenu/scripts/5_Mission"
				};
			};
		};
	};	
};

class CfgSoundSets
{
	class Music_Menu_SoundSet
	{
		soundShaders[]=
		{
			"DZNC_Music_Menu_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=0;
	};
	class CUT_Header_Click_UI_SoundSet
	{
		soundShaders[] = {"CUT_Header_Click_UI_SoundShader"};
		volumeFactor = 1;
		frequencyFactor = 1;
		spatial = 0;
	};
	class CUT_Light_Click_UI_SoundSet
	{
		soundShaders[] = {"CUT_Light_Click_UI_SoundShader"};
		volumeFactor = 1;
		frequencyFactor = 1;
		spatial = 0;
	};
};

class CfgSoundShaders
{
	class DZNC_Music_Menu_SoundShader
	{
		samples[] = 
		{
			// {"\CUT_Mainmenu\GUI\sounds\MainMenu.ogg", 1}
			// If you want to use more than one song to override the new vanilla main menu music that is based on time of day, 
			// you can add more lines like the one above. But using just one song will override all of them
			// {"\CUT_Mainmenu\GUI\sounds\MainMenu_1.ogg", 1},
			// {"\CUT_Mainmenu\GUI\sounds\MainMenu_2.ogg", 1},
			// {"\CUT_Mainmenu\GUI\sounds\MainMenu_3.ogg", 1},
			// {"\CUT_Mainmenu\GUI\sounds\MainMenu_4.ogg", 1},
			// {"\CUT_Mainmenu\GUI\sounds\MainMenu_5.ogg", 1},
			// {"\CUT_Mainmenu\GUI\sounds\MainMenu_6.ogg", 1}
		};
		volume = 0.5;
	};
	class CUT_Header_Click_UI_SoundShader
	{
		samples[] = {{"CUT_Mainmenu\GUI\sounds\Header_Click_UI",1}};
		volume = 1.2;
	};
	class CUT_Light_Click_UI_SoundShader
	{
		samples[] = {{"CUT_Mainmenu\GUI\sounds\Light_Click_UI",1}};
		volume = 1.2;
	};
};
