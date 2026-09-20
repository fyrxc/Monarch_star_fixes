modded class PlayerBase
{
	// Menu dummies only. Stops emote input from ending poses / committing suicide.
	bool CUT_MenuPoseLock;

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (CUT_MenuPoseLock)
			DisableSimulation(false);
	}
};
