modded class OptionsMenuGame
{
	override string GetLayoutName()
	{
#ifdef PLATFORM_CONSOLE
		return super.GetLayoutName();
#else
		return "CUT_Mainmenu/gui/layouts/new_ui/options/pc/cui.game_tab.layout";
#endif
	}
}

modded class OptionsMenuSounds
{
	override string GetLayoutName()
	{
#ifdef PLATFORM_CONSOLE
		return super.GetLayoutName();
#else
		return "CUT_Mainmenu/gui/layouts/new_ui/options/pc/cui.sound_tab.layout";
#endif
	}
}

modded class OptionsMenuVideo
{
	override string GetLayoutName()
	{
#ifdef PLATFORM_CONSOLE
		return super.GetLayoutName();
#else
		return "CUT_Mainmenu/gui/layouts/new_ui/options/pc/cui.video_tab.layout";
#endif
	}
}

modded class OptionsMenuControls
{
	override string GetLayoutName()
	{
#ifdef PLATFORM_CONSOLE
		return super.GetLayoutName();
#else
		return "CUT_Mainmenu/gui/layouts/new_ui/options/pc/cui.controls_tab.layout";
#endif
	}
}
