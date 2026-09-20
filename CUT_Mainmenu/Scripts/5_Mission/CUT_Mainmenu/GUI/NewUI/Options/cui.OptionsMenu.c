modded class OptionsMenu extends UIScriptedMenu
{
	private Widget m_Separator;
	private	Widget m_shader;
	private Widget m_TopShader;
	private Widget m_BottomShader;
	private Widget m_MenuDivider;

	// Vanilla IsAnyTabChanged() can report phantom changes without any user input
	// (slider quantization writes back into GameOptions on tab switches/reverts,
	// float != compares in tab IsChanged(), etc). Track whether the player actually
	// touched a setting this session and only warn on Back when they did.
	private bool m_CUT_UserDirty;
	private bool m_CUT_SquelchChange;

	override Widget Init()
	{
		m_CUT_SquelchChange = true;
		
		m_Options		= new GameOptions();
		
		layoutRoot		= GetGame().GetWorkspace().CreateWidgets("CUT_Mainmenu/gui/layouts/new_ui/options/pc/cui.options_menu.layout", null);
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);
		
		m_Details		= layoutRoot.FindAnyWidget("settings_details");
		m_Version		= TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		
		m_GameTab		= new OptionsMenuGame(layoutRoot.FindAnyWidget("Tab_0"), m_Details, m_Options, this);
		m_SoundsTab		= new OptionsMenuSounds(layoutRoot.FindAnyWidget("Tab_1"), m_Details, m_Options, this);
		
		m_VideoTab		= new OptionsMenuVideo(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
		m_ControlsTab	= new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_3"), m_Details, m_Options, this);

		CUT_CompactAllOptionTabs();
		CUT_ScheduleOptionRelayout();
		

		m_Apply			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("apply"));
		m_Back			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		m_Reset			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("reset"));
		m_Defaults		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("defaults"));
		
		m_TopShader 			    = layoutRoot.FindAnyWidget( "TopShader" );
		m_BottomShader 			    = layoutRoot.FindAnyWidget( "BottomShader" );
		m_MenuDivider				= layoutRoot.FindAnyWidget( "MenuDivider" );
		

		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());
		m_MenuDivider.SetColor(colorScheme.Separator());

		if (m_Tabber)
		{
			m_Tabber.m_OnTabSwitch.Insert(OnTabSwitch);
			m_Tabber.m_OnAttemptTabSwitch.Insert(OnAttemptTabSwitch);
			m_Tabber.AlignTabbers();
		}

		m_ModalLock = false;
		m_CanApplyOrReset = false;
		
		string version;
		m_Version.SetText( version );
		
		SetFocus( layoutRoot );
		ToggleFocus();
			
		OnChanged();
		KeepApplyEnabled();
		Class.CastTo(m_shader, layoutRoot.FindAnyWidget("Colorful_Shader"));
		// m_shader.SetColor(colorScheme.ShaderColor());

		m_Separator 	= layoutRoot.FindAnyWidget( "colorful_separator" );
		// m_Separator.SetColor(colorScheme.SeparatorColor());

		m_CUT_SquelchChange = false;
		m_CUT_UserDirty = false;

		return layoutRoot;
	}

	override void OnShow()
	{
		super.OnShow();
		CUT_CompactAllOptionTabs();
		CUT_ScheduleOptionRelayout();
	}

	override void OnHide()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(CUT_CompactAllOptionTabs);
		super.OnHide();
	}

	protected void CUT_ScheduleOptionRelayout()
	{
		ScriptCallQueue queue = GetGame().GetCallQueue(CALL_CATEGORY_GUI);
		if (!queue)
			return;

		queue.Remove(CUT_CompactAllOptionTabs);
		// VPPAdminTools and similar mods inject extra option rows after Init.
		queue.CallLater(CUT_CompactAllOptionTabs, 50, false);
		queue.CallLater(CUT_CompactAllOptionTabs, 250, false);
	}

	// Vanilla option-group WrapSpacers are authored at 473px, which stretches
	// setting rows across the whole group. Collapse those shells so they hug
	// their rows, but keep a full 650px row width so labels do not paint over
	// dropdowns and sliders. Extra tabs (Hitmarker / Crashout / other mods)
	// get the same treatment without renaming any widget IDs.
	protected void CUT_CompactAllOptionTabs()
	{
		if (!layoutRoot)
			return;

		CUT_CompactOptionsTree(layoutRoot.FindAnyWidget("Tab_0"));
		CUT_CompactOptionsTree(layoutRoot.FindAnyWidget("Tab_1"));
		CUT_CompactOptionsTree(layoutRoot.FindAnyWidget("Tab_2"));
		CUT_CompactOptionsTree(layoutRoot.FindAnyWidget("Tab_3"));
		CUT_CompactOptionsTree(layoutRoot.FindAnyWidget("Tab_4"));
		CUT_CompactOptionsTree(layoutRoot.FindAnyWidget("Tab_5"));

		Widget holder = layoutRoot.FindAnyWidget("TabContentsHolder");
		if (holder)
			CUT_CompactOptionsTree(holder);
	}

	protected void CUT_CompactOptionsTree(Widget w)
	{
		if (!w)
			return;

		CUT_FixOptionWidget(w);

		Widget child = w.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			CUT_CompactOptionsTree(child);
			child = next;
		}
	}

	protected void CUT_FixOptionWidget(Widget w)
	{
		string name = w.GetName();

		if (name.Contains("_settings_root") || name.Contains("_settings_content"))
		{
			float sx;
			float sy;
			w.GetSize(sx, sy);
			if (sy > 80)
			{
				w.SetSize(sx, 1);
				w.Update();
			}

			CUT_ForceSettingsWidth(w);
		}

		if (name.Contains("_setting_panel") || name.Contains("_mode_panel"))
		{
			w.SetFlags(WidgetFlags.CLIPCHILDREN);
			w.ClearFlags(WidgetFlags.HEXACTSIZE);
			w.ClearFlags(WidgetFlags.VEXACTSIZE);
			w.SetSize(0.58, 1);
		}
		else if ((name.Contains("_setting_option") || name.Contains("_mode_option")) && (w.GetFlags() & WidgetFlags.HEXACTSIZE) != WidgetFlags.HEXACTSIZE)
		{
			w.ClearFlags(WidgetFlags.HEXACTSIZE);
			w.ClearFlags(WidgetFlags.VEXACTSIZE);
			w.SetSize(0.42, 1);
		}

		CUT_FixInjectedSettingPair(w);
	}

	protected void CUT_ForceSettingsWidth(Widget w)
	{
		float screenX;
		float screenY;
		w.GetScreenSize(screenX, screenY);
		if (screenX <= 10 || screenX >= 620)
			return;

		float sx;
		float sy;
		w.GetSize(sx, sy);
		w.SetFlags(WidgetFlags.HEXACTSIZE);
		w.SetSize(650, sy);
		w.Update();
	}

	// Two-column rows (vanilla or injected) whose children are a label panel
	// plus an option frame: give the label 58% and clip overflow.
	protected void CUT_FixInjectedSettingPair(Widget w)
	{
		Widget first = w.GetChildren();
		if (!first)
			return;

		Widget second = first.GetSibling();
		if (!second || second.GetSibling())
			return;

		string firstName = first.GetName();
		string secondName = second.GetName();

		Widget panel;
		Widget option;
		if (firstName.Contains("_panel") && secondName.Contains("_option"))
		{
			panel = first;
			option = second;
		}
		else if (firstName.Contains("_option") && secondName.Contains("_panel"))
		{
			option = first;
			panel = second;
		}
		else
			return;

		float sx;
		float sy;
		w.GetScreenSize(sx, sy);
		if (sx > 10 && sx < 620 && sy > 16 && sy < 80)
		{
			w.SetFlags(WidgetFlags.HEXACTSIZE);
			w.SetFlags(WidgetFlags.VEXACTSIZE);
			w.SetSize(650, sy);
			w.Update();
		}

		panel.SetFlags(WidgetFlags.CLIPCHILDREN);
		panel.ClearFlags(WidgetFlags.HEXACTSIZE);
		panel.ClearFlags(WidgetFlags.VEXACTSIZE);
		panel.SetSize(0.58, 1);

		if ((option.GetFlags() & WidgetFlags.HEXACTSIZE) != WidgetFlags.HEXACTSIZE)
		{
			option.ClearFlags(WidgetFlags.HEXACTSIZE);
			option.ClearFlags(WidgetFlags.VEXACTSIZE);
			option.SetSize(0.42, 1);
		}

		panel.Update();
		option.Update();
	}
		
	//Coloring functions (Until WidgetStyles are useful)
	override void ColorHighlight( Widget w )
	{
		if ( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( colorScheme.ButtonHover() );
		}
		
		w.SetColor( UIColor.Black() );
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindAnyWidget( w.GetName() + "_image" ) );
		Widget option		= Widget.Cast( w.FindAnyWidget( w.GetName() + "_option_wrapper" ) );
		Widget option_label = w.FindAnyWidget( "option_label" );
		
		if ( text1 )
		{
			text1.SetColor(colorScheme.ButtonHover() );
		}
		
		if ( text2 )
		{
			text2.SetColor(colorScheme.ButtonHover() );
		}
		
		if ( text3 )
		{
			text3.SetColor(colorScheme.ButtonHover() );
			w.SetAlpha(1);
		}
		
		if ( image )
		{
			image.SetColor( colorScheme.ButtonHover() );
		}
		
		if ( option )
		{
			option.SetColor(colorScheme.ButtonHover() );
		}
		
		if ( option_label )
		{
			option_label.SetColor(colorScheme.ButtonHover() );
		}
	}
	
	override void ColorNormal( Widget w )
	{
		if ( (w.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER )
		{
			return;
		}
		
		if ( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( colorScheme.PrimaryText() );
		}
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindAnyWidget( w.GetName() + "_image" ) );
		Widget option		= w.FindAnyWidget( w.GetName() + "_option_wrapper" );
		Widget option_label = w.FindAnyWidget( "option_label" );
		
		if ( text1 )
		{
			text1.SetColor( colorScheme.PrimaryText() );
		}
		
		if ( text2 )
		{
			text2.SetColor( colorScheme.PrimaryText() );
		}
		
		if ( text3 )
		{
			text3.SetColor( colorScheme.PrimaryText() );
			w.SetAlpha(0);
		}
		
		if ( image )
		{
			image.SetColor( colorScheme.PrimaryText() );
		}
		
		if ( option )
		{
			w.SetAlpha(0.5);
			option.SetColor( colorScheme.PrimaryText() );
		}
		
		if ( option_label )
		{
			option_label.SetColor( colorScheme.PrimaryText() );
		}
	}

	void KeepApplyEnabled()
	{
		if (!m_Apply)
			return;
		m_Apply.ClearFlags(WidgetFlags.IGNOREPOINTER);
		ColorNormal(m_Apply);
	}

	override void OnChanged()
	{
		if (!m_CUT_SquelchChange)
			m_CUT_UserDirty = true;
		super.OnChanged();
		KeepApplyEnabled();
	}

	override void Apply()
	{
		m_CUT_SquelchChange = true;
		super.Apply();
		m_CUT_SquelchChange = false;
		m_CUT_UserDirty = false;
		KeepApplyEnabled();
	}

	override void Reset()
	{
		m_CUT_SquelchChange = true;
		super.Reset();
		m_CUT_SquelchChange = false;
		m_CUT_UserDirty = false;
		KeepApplyEnabled();
	}

	override void ResetCurrentTab()
	{
		m_CUT_SquelchChange = true;
		super.ResetCurrentTab();
		m_CUT_SquelchChange = false;
		m_CUT_UserDirty = false;
		KeepApplyEnabled();
	}

	override void OnAttemptTabSwitch(int source, int target)
	{
		// Phantom "changed" state with no user input: treat as a clean switch
		// instead of popping the vanilla apply dialog.
		if (!m_CUT_UserDirty)
		{
			ResetCurrentTab();
			m_Tabber.SetCanSwitch(true);
			return;
		}
		super.OnAttemptTabSwitch(source, target);
	}

	override void ColorDisable(Widget w)
	{
		if (w == m_Apply)
		{
			KeepApplyEnabled();
			return;
		}
		super.ColorDisable(w);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Apply || w == m_Back || w == m_Defaults || w == m_Reset)
			CUT_MenuUISound.PlayHeader();
		else
			CUT_MenuUISound.PlayLight();
		return super.OnClick(w, x, y, button);
	}

	override void Back()
	{
		if (!g_Game.GetUIManager().IsDialogVisible() && !g_Game.GetUIManager().IsModalVisible())
		{
			if (m_CUT_UserDirty && IsAnyTabChanged())
			{
				EnterScriptedMenu( COLORFUL_CONFIGURE );
				//g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#main_menu_configure_desc", 1337, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
			}
			else
			{
				m_Options.Revert();
				GetGame().EndOptionsVideo();
				GetGame().GetUIManager().Back();
			}
		}
	}
	
}
