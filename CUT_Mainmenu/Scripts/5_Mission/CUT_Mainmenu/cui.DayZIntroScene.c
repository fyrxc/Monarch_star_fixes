modded class DayZIntroScene
{
	void CUT_ApplySceneCamera(vector camPos, vector camOri)
	{
		if (!m_Camera)
			return;

		m_Camera.SetPosition(camPos);
		if (camOri != vector.Zero)
			m_Camera.SetOrientation(camOri);
		m_Camera.SetFOV(0.72);
		m_Camera.SetFocus(4.0, 0.0);
		m_Camera.SetActive(true);
	}

	void CUT_LookAtPlayer()
	{
		if (!m_Camera || !m_Character || !m_Character.GetCharacterObj())
			return;

		PlayerBase player = m_Character.GetCharacterObj();
		m_Target = player.GetPosition() + Vector(0, 1.05, 0);
		m_Camera.LookAt(m_Target);
		m_Camera.SetFOV(0.72);
		m_Camera.SetFocus(4.0, 0.0);
		m_Camera.SetActive(true);
	}

	void CUT_PlacePlayerAuto()
	{
		if (!m_Camera || !m_Character || !m_Character.GetCharacterObj())
			return;

		Math3D.MatrixIdentity4(m_CameraTrans);
		Math3D.DirectionAndUpMatrix(m_Camera.GetDirection(), "0 1 0", m_CameraTrans);
		m_CameraTrans[3] = m_Camera.GetPosition();
		m_CharacterPos = Vector(0.685547, -0.988281, 3.68823).Multiply4(m_CameraTrans);

		float pos_x = m_CharacterPos[0];
		float pos_z = m_CharacterPos[2];
		float pos_y = g_Game.SurfaceY(pos_x, pos_z);
		m_CharacterPos = Vector(pos_x, pos_y, pos_z);

		vector to_cam_dir = m_Camera.GetPosition() - m_CharacterPos;
		m_CharacterRot[0] = Math.Atan2(to_cam_dir[0], to_cam_dir[2]) * Math.RAD2DEG;

		CUT_ApplyPlayerTransform(m_CharacterPos, m_CharacterRot);
	}

	void CUT_PlacePlayerAt(vector pos, vector ori)
	{
		m_CharacterPos = pos;
		m_CharacterRot = ori;
		CUT_ApplyPlayerTransform(pos, ori);
	}

	protected void CUT_ApplyPlayerTransform(vector pos, vector ori)
	{
		if (!m_Character || !m_Character.GetCharacterObj())
			return;

		PlayerBase player = m_Character.GetCharacterObj();
		player.SetPosition(pos);
		player.SetOrientation(ori);
	}

	void CUT_SetPlayerInvisible(bool hide)
	{
		if (m_Character && m_Character.GetCharacterObj())
			m_Character.GetCharacterObj().SetInvisible(hide);
	}

	PlayerBase CUT_GetPlayer()
	{
		if (m_Character)
			return m_Character.GetCharacterObj();
		return null;
	}
};

modded class DayZIntroScenePC
{
	void DayZIntroScenePC()
	{
		CUT_MainmenuSceneLoader.Apply(this);
	}

	void CUT_RefreshOrbitRadius()
	{
		if (!m_Camera)
			return;

		m_DeltaX = Math.AbsFloat(m_CharacterPos[0] - m_Camera.GetPosition()[0]);
		m_DeltaZ = Math.AbsFloat(m_CharacterPos[2] - m_Camera.GetPosition()[2]);
		m_Radius = Math.Sqrt(Math.Pow(m_DeltaX, 2) + Math.Pow(m_DeltaZ, 2));
		m_Radius_original = m_Radius;
	}
};
