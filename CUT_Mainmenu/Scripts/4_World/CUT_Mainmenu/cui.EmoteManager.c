// Looping "search inventory" modifier used by menu dummies (Seth rummaging).
class CUT_MenuInventoryCB : HumanCommandActionCallback
{
	void CUT_MenuInventoryCB()
	{
		EnableCancelCondition(false);
	}
};

class CUT_MenuLoopCB : HumanCommandActionCallback
{
	void CUT_MenuLoopCB()
	{
		EnableCancelCondition(false);
	}
};

modded class EmoteCB
{
	override void OnAnimationEvent(int pEventID)
	{
		PlayerBase player = PlayerBase.Cast(m_player);
		if (player && player.CUT_MenuPoseLock)
		{
			if (pEventID == EmoteConstants.EMOTE_SUICIDE_DEATH || pEventID == EmoteConstants.EMOTE_SUICIDE_BLEED || pEventID == EmoteConstants.EMOTE_SUICIDE_SIMULATION_END)
				return;
		}

		super.OnAnimationEvent(pEventID);
	}
};

modded class EmoteManager
{
	protected bool CUT_MenuLocked()
	{
		PlayerBase player = PlayerBase.Cast(m_Player);
		return player && player.CUT_MenuPoseLock;
	}

	override void KillPlayer()
	{
		if (CUT_MenuLocked())
			return;

		super.KillPlayer();
	}

	override protected void CommitSuicide()
	{
		if (CUT_MenuLocked())
			return;

		super.CommitSuicide();
	}

	override void EndCallbackCommand()
	{
		if (CUT_MenuLocked())
			return;

		super.EndCallbackCommand();
	}

	override void InterruptCallbackCommand()
	{
		if (CUT_MenuLocked())
			return;

		super.InterruptCallbackCommand();
	}
};
