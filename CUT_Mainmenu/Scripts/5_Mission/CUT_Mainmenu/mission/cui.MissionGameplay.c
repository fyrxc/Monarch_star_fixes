modded class MissionGameplay
{
	override void Continue()
	{
		if (CUI_IsDeathScreenLocked())
			return;
		super.Continue();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (!CUI_IsDeathScreenLocked() || !GetGame() || !GetGame().GetUIManager())
			return;
		if (GetGame().GetUIManager().IsMenuOpen(MENU_INGAME))
			return;
		if (GetGame().GetUIManager().IsMenuOpen(MENU_RESPAWN_DIALOGUE))
			return;
		if (GetGame().GetUIManager().IsMenuOpen(MENU_LOGOUT))
			return;
		GetGame().GetUIManager().EnterScriptedMenu(MENU_INGAME, null);
	}
}
