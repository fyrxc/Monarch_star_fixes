modded class DayZPlayerImplement extends DayZPlayer
{
    override void ShowDeadScreen(bool show, float duration)
    {
        #ifndef NO_GUI
        CUI_ShowDeadScreen = show;
        CUI_DeadScreenFaded = false;

        if (show && IsPlayerSelected())
        {
            if (duration > 0)
                CUI_DeadFadeSeconds = duration;
            else
                CUI_DeadFadeSeconds = 2.5;
            GetGame().GetUIManager().ScreenFadeOut(0);
            GetGame().GetUIManager().EnterScriptedMenu(MENU_INGAME, null);
        }
        else
        {
            GetGame().GetUIManager().ScreenFadeOut(0);
        }

        if (duration > 0)
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StopDeathDarkeningEffect, duration * 1000, false);
        else
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(StopDeathDarkeningEffect);
        #endif
    };
};
