modded class InGameMenu extends UIScriptedMenu
{
	private Widget m_Separator;
	private	Widget m_shader;
	private Widget m_Discord;
	private Widget m_Twitter;
	private Widget m_Youtube;
	private Widget m_Reddit;
	private Widget m_Facebook;
	
	private Widget m_DiscordImg;
	private Widget m_TwitterImg;
	private Widget m_YoutubeImg;
	private Widget m_RedditImg;
	private Widget m_FacebookImg;
	private Widget m_MetaImg;
	private Widget m_MenuDivider;
	private Widget m_Website;
	private Widget m_PriorityQueue;
	
	private Widget m_TopShader;
	private Widget m_BottomShader;

	private Widget m_GameOverScreen;
	private ImageWidget m_GameOverScreenImage;
	private Widget m_Logo;
	private Widget m_ShadersRoot;
	private float m_DeadFade;
	private bool m_DeadFadeStarted;

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("CUT_Mainmenu/gui/layouts/cui.day_z_ingamemenu.layout");
			
		m_ContinueButton			= layoutRoot.FindAnyWidget( "ContinueBtn" );
		m_ExitButton				= layoutRoot.FindAnyWidget( "ExitBtn" );
		m_MenuDivider				= layoutRoot.FindAnyWidget( "MenuDivider" );
		m_RestartButton				= layoutRoot.FindAnyWidget( "RestartBtn" );
		m_RespawnButton 			= layoutRoot.FindAnyWidget( "respawn_button" );
		m_RestartDeadRandomButton	= layoutRoot.FindAnyWidget( "respawn_button_random" );
		m_RestartDeadCustomButton	= layoutRoot.FindAnyWidget( "respawn_button_custom" );
		m_OptionsButton				= layoutRoot.FindAnyWidget( "OptionsBtn" );
		m_ModdedWarning				= TextWidget.Cast( layoutRoot.FindAnyWidget( "ModdedWarning" ) );
		m_HintPanel					= new UiHintPanel(layoutRoot.FindAnyWidget( "hint_frame" ));
		m_ServerInfoPanel 			= layoutRoot.FindAnyWidget( "server_info" );
		m_ServerIP 					= TextWidget.Cast( layoutRoot.FindAnyWidget( "server_ip" ) );
		m_ServerPort 				= TextWidget.Cast( layoutRoot.FindAnyWidget( "server_port" ) );
		m_ServerName 				= TextWidget.Cast( layoutRoot.FindAnyWidget( "server_name" ) );
		m_FavoriteImage 			= layoutRoot.FindAnyWidget( "favorite_image" );
		m_UnfavoriteImage 			= layoutRoot.FindAnyWidget( "unfavorite_image" );
		m_CopyInfoButton 			= layoutRoot.FindAnyWidget( "copy_button" );
		m_Separator 				= layoutRoot.FindAnyWidget( "actionItems_separator" );
		m_Discord 					= layoutRoot.FindAnyWidget( "DiscordBtn" );
		m_Twitter 					= layoutRoot.FindAnyWidget( "TwitterBtn" );
		m_Youtube 					= layoutRoot.FindAnyWidget( "YoutubeBtn" );
		m_Reddit 					= layoutRoot.FindAnyWidget( "RedditBtn" );
		m_Facebook 					= layoutRoot.FindAnyWidget( "FacebookBtn" );
		m_DiscordImg 				= layoutRoot.FindAnyWidget( "DiscordBtn_img" );
	    m_TwitterImg 				= layoutRoot.FindAnyWidget( "TwitterBtn_img" );
		m_YoutubeImg 				= layoutRoot.FindAnyWidget( "YoutubeBtn_img" );
		m_RedditImg 				= layoutRoot.FindAnyWidget( "RedditBtn_img" );
		m_FacebookImg 				= layoutRoot.FindAnyWidget( "FacebookBtn_img" );
		m_MetaImg 					= layoutRoot.FindAnyWidget( "MetaBtn_img" );
		m_Website					= layoutRoot.FindAnyWidget( "WebsiteBtn" );
		m_PriorityQueue				= layoutRoot.FindAnyWidget( "QueueBtn" );
		m_TopShader 			    = layoutRoot.FindAnyWidget( "TopShader" );
		m_BottomShader 			    = layoutRoot.FindAnyWidget( "BottomShader" );
		
		// Social Icons
		m_DiscordImg.SetColor(UIColor.discord());
		m_TwitterImg.SetColor(UIColor.twitter());
		m_YoutubeImg.SetColor(UIColor.youtube());
		m_RedditImg.SetColor(UIColor.reddit());
		m_FacebookImg.SetColor(UIColor.meta());
		m_MetaImg.SetColor(UIColor.meta());

   		m_MenuDivider.SetColor(colorScheme.Separator());
		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());

		HudShow( false );
		Class.CastTo(m_shader, layoutRoot.FindAnyWidget("Colorful_Shader"));

		m_Logo = layoutRoot.FindAnyWidget("Logo");
		m_ShadersRoot = layoutRoot.FindAnyWidget("Shaders");
		m_GameOverScreen = Widget.Cast(layoutRoot.FindAnyWidget("GameOverScreen"));
		if (m_GameOverScreen)
		{
			m_GameOverScreen.SetAlpha(0);
			m_GameOverScreen.Show(false);
			m_GameOverScreenImage = ImageWidget.Cast(m_GameOverScreen.FindAnyWidget("GameOverScreenImage"));
			if (m_GameOverScreenImage)
			{
				m_GameOverScreenImage.LoadImageFile(0, GameOverScreen.GameOverScreenImage());
				m_GameOverScreenImage.SetAlpha(0);
			}
		}

		return layoutRoot;
	}

	protected bool IsDeathMenu()
	{
		if (CUI_ShowDeadScreen)
			return true;

		Man player = GetGame().GetPlayer();
		return player && player.GetPlayerState() != EPlayerStates.ALIVE;
	}

	protected void ApplyDeadScreenAlpha(float alpha)
	{
		if (alpha < 0)
			alpha = 0;
		if (alpha > 1)
			alpha = 1;
		if (m_GameOverScreen)
		{
			m_GameOverScreen.Show(true);
			m_GameOverScreen.SetAlpha(alpha);
		}
		if (m_GameOverScreenImage)
			m_GameOverScreenImage.SetAlpha(alpha);
		if (m_Logo)
			m_Logo.Show(false);
		if (m_ShadersRoot)
			m_ShadersRoot.Show(false);
	}

	protected void TickDeadScreen(float timeslice)
	{
		if (!m_DeadFadeStarted)
		{
			m_DeadFadeStarted = true;
			if (CUI_DeadScreenFaded)
				m_DeadFade = 1;
			else
				m_DeadFade = 0;
		}
		if (m_DeadFade < 1)
		{
			float dur = CUI_DeadFadeSeconds;
			if (dur <= 0.05)
				m_DeadFade = 1;
			else
				m_DeadFade = m_DeadFade + (timeslice / dur);
			if (m_DeadFade >= 1)
			{
				m_DeadFade = 1;
				CUI_DeadScreenFaded = true;
			}
		}
		ApplyDeadScreenAlpha(m_DeadFade);
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (IsDeathMenu())
			TickDeadScreen(timeslice);
	}

	override void OnClick_Continue()
	{
		if (IsDeathMenu())
			return;
		super.OnClick_Continue();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_ContinueButton || w == m_ExitButton || w == m_OptionsButton || w == m_RestartButton || w == m_RespawnButton)
			CUT_MenuUISound.PlayHeader();
		else
			CUT_MenuUISound.PlayLight();
		if ( w == m_ContinueButton )
		{
			OnClick_Continue();
			return true;
		}
		else if ( w == m_RestartButton )
		{
			OnClick_Restart();
			return true;
		}
		else if ( w == m_RespawnButton )
		{
			OnClick_Respawn();
			return true;
		}
		else if ( w == m_OptionsButton )
		{
			OnClick_Options();
			return true;
		}
		else if ( w == m_ExitButton )
		{
			OnClick_Exit();
			return true;
		}
		else if (button == MouseState.LEFT && w == m_Discord)
		{
			GetGame().OpenURL(MenuURLS.urlDiscord);
			return true;
		}
		else if (button == MouseState.LEFT && w == m_Twitter)
		{
			GetGame().OpenURL(MenuURLS.urlTwitter);
			return false;
		}
		else if (button == MouseState.LEFT && w == m_Reddit)
		{
			GetGame().OpenURL(MenuURLS.urlReddit);
			return false;
		}
		else if (button == MouseState.LEFT && w == m_Youtube)
		{
			GetGame().OpenURL(MenuURLS.urlYoutube);
			return false;
		}	
		else if (button == MouseState.LEFT && w == m_Facebook)
		{
			GetGame().OpenURL(MenuURLS.urlFacebook);
			return false;
		}
		else if (button == MouseState.LEFT && w == m_Website)
		{
			GetGame().OpenURL(MenuURLS.urlWebsite);
			return true;
		}
		else if (button == MouseState.LEFT && w == m_PriorityQueue)
		{
			GetGame().OpenURL(MenuURLS.urlPriorityQ);
			return true;
		}

		return false;
	}

	// If you want to change the way the buttons work.  
	// Visit the Colorful.MainMenu.c file. It has notes.
	override void ColorHighlight( Widget w )
	{
		if( !w )
			return;
		
		ButtonSetColor(w, UIColor.Transparent());
		ButtonSetTextColor(w, colorScheme.ButtonHover());
		
	};

	override void ColorNormal( Widget w )
	{
		if( !w )
			return;
		
		ButtonSetColor(w, UIColor.Transparent());
		ButtonSetTextColor(w, UIColor.White());
	}

	protected void TwitterHighlight( Widget w )
	{
		if( !w )
			return;	
			
		int color_pnl = UIColor.Transparent();
		int color_lbl = UIColor.twitter();
		int color_img = UIColor.twitter();	

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}

	protected void DiscordHighlight( Widget w )
	{
		if( !w )
			return;	
			
		int color_pnl = UIColor.Transparent();
		int color_lbl = UIColor.discord();
		int color_img = UIColor.discord();	

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}

	protected void YoutubeHighlight( Widget w )
	{
		if( !w )
			return;	
			
		int color_pnl = UIColor.Transparent();
		int color_lbl = UIColor.youtube();
		int color_img = UIColor.youtube();	

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}

	protected void RedditHighlight( Widget w )
	{
		if( !w )
			return;	
			
		int color_pnl = UIColor.Transparent();
		int color_lbl = UIColor.reddit();
		int color_img = UIColor.reddit();	

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}

	protected void MetaHighlight( Widget w )
	{
		if( !w )
			return;	
			
		int color_pnl = UIColor.Transparent();
		int color_lbl = UIColor.meta();
		int color_img = UIColor.meta();	

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}

	override bool OnMouseEnter( Widget w, int x, int y )
	{
		
		if( w == m_Twitter )
		{
			TwitterHighlight( w );
			return true;
		}

		if( w == m_Facebook )
		{
			MetaHighlight( w );
			return true;
		}

		if( w == m_Discord )
		{
			DiscordHighlight( w );
			return true;
		}

		if( w == m_Youtube )
		{
			YoutubeHighlight( w );
			return true;
		}

		if( w == m_Reddit )
		{
			RedditHighlight( w );
			return true;
		}

		if (IsFocusable(w))
		{
			ColorHighlight(w);
			return true;
		}

		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_Twitter || w == m_Facebook || w == m_Discord || w == m_Youtube || w == m_Reddit)
		{
			ColorNormal(w);
			if (m_DiscordImg)
				m_DiscordImg.SetColor(UIColor.discord());
			if (m_TwitterImg)
				m_TwitterImg.SetColor(UIColor.twitter());
			if (m_YoutubeImg)
				m_YoutubeImg.SetColor(UIColor.youtube());
			if (m_RedditImg)
				m_RedditImg.SetColor(UIColor.reddit());
			if (m_FacebookImg)
				m_FacebookImg.SetColor(UIColor.meta());
			if (m_MetaImg)
				m_MetaImg.SetColor(UIColor.meta());
			return true;
		}
		if (IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}
		return false;
	}

	bool IsFocusable(Widget w)
	{
		if (w == m_ContinueButton || w == m_ExitButton || w == m_OptionsButton)
			return true;
		if (w == m_RestartButton || w == m_RespawnButton)
			return true;
		if (w == m_RestartDeadRandomButton || w == m_RestartDeadCustomButton)
			return true;
		if (w == m_Website || w == m_PriorityQueue)
			return true;
		return false;
	}
}
