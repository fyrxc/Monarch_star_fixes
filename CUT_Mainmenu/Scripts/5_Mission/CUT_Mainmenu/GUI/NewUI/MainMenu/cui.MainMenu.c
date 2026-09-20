modded class MainMenu extends UIScriptedMenu
{
// Social Buttons
	private Widget m_Discord;
	private Widget m_Twitter;
	private Widget m_Youtube;
	private Widget m_TikTok;
	private Widget m_Facebook;
	private Widget m_Instagram;

	// Social Icons
	private Widget m_DiscordImg;
	private Widget m_TwitterImg;
	private Widget m_YoutubeImg;
	private Widget m_TikTokImg;
	private Widget m_FacebookImg;
	private Widget m_InstagramImg;
	
	// Top Nav Buttons
	private Widget m_ExitBtn;
	private Widget m_SettingsBtn;
	private Widget m_TutorialBtn;
	private Widget m_MessageBtn;

	// Top Nav Icons
	private Widget m_ExitBtnImg;
	private Widget m_SettingsBtnImg;
	private Widget m_TutorialBtnImg;
	private Widget m_MessageBtnImg;
	
	// Custom Buttons (No Icons)
	private Widget m_Website;
	private Widget m_PriorityQueue;
	private Widget m_CharacterBtn;
	private Widget m_PlayDM;
	private Widget m_SteamIdText;

	// CUI Shaders
	private Widget m_TopShader;
	private Widget m_BottomShader;
	
	// Visual Elements
	private	Widget m_welcomeBack;
	private Widget m_SurvivorBox;
	private Widget m_StatsBox;
	private Widget m_SurvivorDivider;
	private Widget m_MenuDivider;
	private Widget m_ProgressLoading;
	
	override Widget Init()
	{
		// Load the layout file
		layoutRoot					= GetGame().GetWorkspace().CreateWidgets( "CUT_Mainmenu/gui/layouts/new_ui/cui.main_menu.layout" );

		// Social Buttons  
		m_Discord 					= layoutRoot.FindAnyWidget( "DiscordBtn" );
		m_DiscordImg 				= layoutRoot.FindAnyWidget( "DiscordBtn_img" );
		m_Youtube 					= layoutRoot.FindAnyWidget( "YoutubeBtn" );
		m_YoutubeImg 				= layoutRoot.FindAnyWidget( "YoutubeBtn_img" );
		m_Twitter 					= layoutRoot.FindAnyWidget( "TwitterBtn" );
		m_TwitterImg 				= layoutRoot.FindAnyWidget( "TwitterBtn_img" );
		m_TikTok 					= layoutRoot.FindAnyWidget( "TikTokBtn" );
		m_TikTokImg 				= layoutRoot.FindAnyWidget( "TikTokBtn_img" );
		m_Facebook 					= layoutRoot.FindAnyWidget( "FacebookBtn" );
		m_FacebookImg 				= layoutRoot.FindAnyWidget( "FacebookBtn_img" );
		m_Instagram 				= layoutRoot.FindAnyWidget( "InstagramBtn" );
		m_InstagramImg 				= layoutRoot.FindAnyWidget( "InstagramBtn_img" );
		
		// Shaders
		m_TopShader 			    = layoutRoot.FindAnyWidget( "TopShader" );
		m_BottomShader 			    = layoutRoot.FindAnyWidget( "BottomShader" );

		// Call to action Buttons
		m_Play						= layoutRoot.FindAnyWidget( "play" );
		m_PlayDM					= layoutRoot.FindAnyWidget( "play_dm" );
		m_ChooseServer				= layoutRoot.FindAnyWidget( "choose_server" );
		m_CustomizeCharacter		= layoutRoot.FindAnyWidget( "customize_character" );
		m_CharacterBtn 				= layoutRoot.FindAnyWidget( "character_button" );
		m_PrevCharacter				= layoutRoot.FindAnyWidget( "prev_character" );
		m_NextCharacter				= layoutRoot.FindAnyWidget( "next_character" );

		// Custom C2A Buttons
		m_PriorityQueue				= layoutRoot.FindAnyWidget( "QueueBtn" );
		m_Website					= layoutRoot.FindAnyWidget( "WebsiteBtn" );

		// Top Naviagtion Buttons
		m_ExitBtn		 		    = layoutRoot.FindAnyWidget( "ExitBtn" );
		m_ExitBtnImg		 		= layoutRoot.FindAnyWidget( "ExitBtn_img" );
		m_SettingsBtn				= layoutRoot.FindAnyWidget( "SettingsBtn" );
		m_SettingsBtnImg		 	= layoutRoot.FindAnyWidget( "SettingsBtn_img" );
		m_TutorialBtn				= layoutRoot.FindAnyWidget( "TutorialBtn" );
		m_TutorialBtnImg		 	= layoutRoot.FindAnyWidget( "TutorialBtn_img" );
		m_MessageBtn				= layoutRoot.FindAnyWidget( "MessageBtn" );
		m_MessageBtnImg		 		= layoutRoot.FindAnyWidget( "MessageBtn_img" );

		// Misc Elements
		m_welcomeBack				= layoutRoot.FindAnyWidget( "WelcomeBack" );
		m_SteamIdText				= layoutRoot.FindAnyWidget( "SteamIdText" );
		m_SurvivorDivider			= layoutRoot.FindAnyWidget( "SurvivorDivider" );
		m_SurvivorBox				= layoutRoot.FindAnyWidget( "SurvivorBox" );
		m_StatsBox					= layoutRoot.FindAnyWidget( "StatisticsBoxBG" );
		m_MenuDivider				= layoutRoot.FindAnyWidget( "MenuDivider" );
		
		// Vanilla 
		m_Play						= layoutRoot.FindAnyWidget( "play" );
		m_ChooseServer				= layoutRoot.FindAnyWidget( "choose_server" );
		m_Stats						= new MainMenuStats( layoutRoot.FindAnyWidget("StatsBox"));
		m_Mission					= MissionMainMenu.Cast( GetGame().GetMission() );
		m_ScenePC					= m_Mission.GetIntroScenePC();
		m_CharacterRotationFrame	= layoutRoot.FindAnyWidget( "character_rotation_frame" );
		m_PlayerName				= TextWidget.Cast(layoutRoot.FindAnyWidget("character_name_text"));
		string version;
		
		GetGame().GetVersion( version );
		GetGame().GetUIManager().ScreenFadeOut(0);
		SetFocus( null );
		GetDayZGame().GetBacklit().MainMenu_OnShow();
		
		// Colorful UI Theming -----------------------------------------
		if (m_SurvivorBox)
			m_SurvivorBox.SetColor(colorScheme.SurvivorBox());
		if (m_StatsBox)
			m_StatsBox.SetColor(colorScheme.StatsBox());
		if (m_TopShader)
			m_TopShader.SetColor(colorScheme.TopShader());
		if (m_BottomShader)
			m_BottomShader.SetColor(colorScheme.BottomShader());
		if (m_MenuDivider)
			m_MenuDivider.SetColor(colorScheme.Separator());
		if (m_SurvivorDivider)
			m_SurvivorDivider.SetColor(colorScheme.Separator());

		if (m_SettingsBtnImg)
			m_SettingsBtnImg.SetColor(colorScheme.NavIcon());
		if (m_TutorialBtnImg)
			m_TutorialBtnImg.SetColor(colorScheme.NavIcon());
		if (m_MessageBtnImg)
			m_MessageBtnImg.SetColor(colorScheme.NavIcon());
		if (m_ExitBtnImg)
			m_ExitBtnImg.SetColor(colorScheme.NavIcon());
		
		PaintSocialIcons();
		FillWelcome();

		// Bottom trim bar is unused on the main menu.
		m_ProgressLoading = ProgressBarWidget.Cast( layoutRoot.FindAnyWidget("LoadingBar") );
		if (m_ProgressLoading)
			m_ProgressLoading.Show(false);

		g_Game.SetLoadState( DayZLoadState.MAIN_MENU_CONTROLLER_SELECT );
		return layoutRoot;
	}	

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (IsSocialButton(w))
		{
			PaintSocialIcons();
			return true;
		}
		if (IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Play || w == m_PlayDM || w == m_SettingsBtn || w == m_ExitBtn)
			CUT_MenuUISound.PlayHeader();
		else
			CUT_MenuUISound.PlayLight();
		if (w == m_Play)
		{
			if (OpenJoinUrl(MenuURLS.urlCrashoutMain))
				return true;
			return super.OnClick(w, x, y, button);
		}
		if (w == m_PlayDM)
		{
			OpenJoinUrl(MenuURLS.urlCrashoutDM);
			return true;
		}
		if (button == MouseState.LEFT && w == m_Discord)
		{
			GetGame().OpenURL(MenuURLS.urlDiscord);
			return true;
		}
		else if (button == MouseState.LEFT && w == m_Youtube)
		{
			GetGame().OpenURL(MenuURLS.urlYoutube);
			return true;
		}
		else if (button == MouseState.LEFT && w == m_Twitter)
		{
			GetGame().OpenURL(MenuURLS.urlTwitter);
			return true;
		}
		else if (button == MouseState.LEFT && w == m_TikTok)
		{
			GetGame().OpenURL(MenuURLS.urlTikTok);
			return true;
		}
		else if (button == MouseState.LEFT && w == m_Facebook)
		{
			GetGame().OpenURL(MenuURLS.urlFacebook);
			return true;
		}
		else if (button == MouseState.LEFT && w == m_Instagram)
		{
			GetGame().OpenURL(MenuURLS.urlInstagram);
			return true;
		}
		else if (button == MouseState.LEFT && w == m_Website)
		{
			GetGame().OpenURL(MenuURLS.urlWebsite);
			return false;
		}
		else if (button == MouseState.LEFT && w == m_PriorityQueue)
		{
			GetGame().OpenURL(MenuURLS.urlShop);
			return false;
		}
		else if ( w == m_CharacterBtn || w == m_CustomizeCharacter )
		{
			OpenMenuCustomizeCharacter();
			return true;
		}
		else if ( w == m_ChooseServer )
		{
			OpenMenuServerBrowser();
			return true;
		}
		else if (w == m_TutorialBtn)
		{
			OpenTutorials();
			return true;
		}	
		else if ( w == m_SettingsBtn )
		{
			OpenSettings();
			return true;
		}
		else if ( w == m_MessageBtn )
		{
			OpenCredits();
			return true;
		}	
		else if ( w == m_ExitBtn )
		{
			Exit();
			return true;
		}		
		return super.OnClick(w, x, y, button);
	};

	override void ColorHighlight( Widget w )
	{
		if( !w )
			return;	
			
		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.ButtonHover();
		int color_img = colorScheme.ButtonHover();	

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
		ImagenSetColor(w, color_img);	
	}

	override void ColorNormal( Widget w )
	{
		if( !w )
			return;
		
		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.PrimaryText();
		int color_img = colorScheme.PrimaryText();

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
		ImagenSetColor(w, color_img);
	}

	void PaintSocialIcon(Widget img, int color)
	{
		if (img)
			img.SetColor(color);
	}

	void PaintSocialIcons()
	{
		PaintSocialIcon(m_DiscordImg, UIColor.discord());
		PaintSocialIcon(m_YoutubeImg, UIColor.youtube());
		PaintSocialIcon(m_TwitterImg, UIColor.twitter());
		PaintSocialIcon(m_TikTokImg, UIColor.tiktok());
		PaintSocialIcon(m_FacebookImg, UIColor.meta());
		PaintSocialIcon(m_InstagramImg, UIColor.instagram());
	}

	bool IsSocialButton(Widget w)
	{
		if (w == m_Discord || w == m_Youtube || w == m_Twitter)
			return true;
		if (w == m_TikTok || w == m_Facebook || w == m_Instagram)
			return true;
		return false;
	}

	void SocialHighlight(Widget w)
	{
		int color = colorScheme.ButtonHover();
		if (w == m_Discord)
			PaintSocialIcon(m_DiscordImg, color);
		else if (w == m_Youtube)
			PaintSocialIcon(m_YoutubeImg, color);
		else if (w == m_Twitter)
			PaintSocialIcon(m_TwitterImg, color);
		else if (w == m_TikTok)
			PaintSocialIcon(m_TikTokImg, color);
		else if (w == m_Facebook)
			PaintSocialIcon(m_FacebookImg, color);
		else if (w == m_Instagram)
			PaintSocialIcon(m_InstagramImg, color);
	}
	
	override void LoadMods()
	{
		// Kill this annoying shit
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if (IsSocialButton(w))
		{
			SocialHighlight(w);
			return true;
		}
		if (IsFocusable(w))
		{
			ColorHighlight(w);
			return true;
		}
		return false;
	}

	override bool IsFocusable(Widget w)
	{
		if (w == m_PlayDM || w == m_SettingsBtn || w == m_ExitBtn)
			return true;
		if (w == m_PriorityQueue || w == m_Website)
			return true;
		return super.IsFocusable(w);
	}

	protected bool OpenJoinUrl(string url)
	{
		if (url == "" || url == "#")
			return false;
		GetGame().OpenURL(url);
		return true;
	}

	protected void FillWelcome()
	{
		string name = "";
		string uid = "";
		BiosUserManager um = GetGame().GetUserManager();
		if (um)
		{
			BiosUser user = um.GetSelectedUser();
			if (user)
			{
				name = user.GetName();
				uid = user.GetUid();
			}
		}
		if (name == "")
			GetGame().GetPlayerName(name);

		TextWidget welcome = TextWidget.Cast(m_welcomeBack);
		if (welcome)
		{
			if (name != "")
				welcome.SetText("Welcome Back, " + name);
			else
				welcome.SetText("Welcome Back");
			welcome.SetColor(colorScheme.PrimaryText());
		}

		TextWidget steamW = TextWidget.Cast(m_SteamIdText);
		if (steamW)
		{
			steamW.SetText(uid);
			steamW.SetColor(colorScheme.PrimaryText());
		}
	}

	override void Exit()
	{
		EnterScriptedMenu( COLORFUL_EXIT );
	}

	override void OnShow()
	{
		super.OnShow();
		CUT_MainmenuSceneLoader.ApplyPlayerVisibility(true);
		FillWelcome();
	}
}