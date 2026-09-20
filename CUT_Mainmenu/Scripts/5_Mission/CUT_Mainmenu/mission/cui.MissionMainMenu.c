modded class MissionMainMenu
{
	override void OnMissionFinish()
	{
		CUT_MainmenuSceneLoader.Stop();
		super.OnMissionFinish();
	}
};
