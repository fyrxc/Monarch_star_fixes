modded class TabberUI extends ScriptedWidgetEventHandler
{
	protected int m_CUT_LastSoundTab;
	protected bool m_CUT_SoundReady;

	override void SelectTabControl( int index )
	{
		if (m_CUT_SoundReady && index == m_CUT_LastSoundTab)
		{
		}
		else
		{
			m_CUT_LastSoundTab = index;
			m_CUT_SoundReady = true;
			CUT_MenuUISound.PlayLight();
		}
		Widget tab_control = m_TabControls.Get( index );
		if( tab_control )
		{
			Widget tab_title = TextWidget.Cast(tab_control.FindAnyWidget( tab_control.GetName() + "_Title" ));
			
			int color_title = colorScheme.TabHover();
			int color_backg = UIColor.Black();
			
			if (tab_title)
				tab_title.SetColor( color_title );
			tab_control.SetColor( color_backg );
		}
	}

	override bool OnMouseEnter( Widget w, int x, int y )
	{
		int index = m_TabControls.GetKeyByValue( w );
		if( m_SelectedIndex == index )
		{
			return false;
		}
		
		Widget tab_control = m_TabControls.Get( index );
		if( tab_control )
		{			
			Widget tab_title = TextWidget.Cast(tab_control.FindAnyWidget( tab_control.GetName() + "_Title" ));
			if (tab_title)
				tab_title.SetColor(colorScheme.TabHover());
			tab_control.SetColor(UIColor.Black());
		}
		
		return false;
	}

	override int AddTab( string name )
	{
		int new_index = m_Tabs.Count();
		Widget container = m_Root.FindAnyWidget("Tab_Control_Container");
		Widget tab = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/tabber_prefab/tab.layout", m_Root);
		Widget control = GetGame().GetWorkspace().CreateWidgets("CUT_Mainmenu/GUI/layouts/new_ui/tabber_prefab/cui.tab_control.layout", container);
		if (!tab || !control)
			return super.AddTab(name);

		TextWidget control_text = TextWidget.Cast(control.FindAnyWidget("Tab_Control_x_Title"));
		Widget control_bg = control.FindAnyWidget("Tab_Control_x_Background");

		tab.SetName("Tab_" + new_index);
		control.SetName("Tab_Control_" + new_index);
		if (control_text)
		{
			control_text.SetName("Tab_Control_" + new_index + "_Title");
			control_text.SetText(name);
			control_text.SetColor(colorScheme.PrimaryText());
		}
		if (control_bg)
			control_bg.SetName("Tab_Control_" + new_index + "_Background");

		control.SetColor(ARGB(140, 0, 0, 0));
		control.SetHandler(this);
		m_TabControls.Insert(new_index, control);
		m_Tabs.Insert(new_index, tab);

		Widget existing = m_Root.FindAnyWidget("Tab_0");
		if (existing)
		{
			float x, y, sx, sy;
			existing.GetPos(x, y);
			existing.GetSize(sx, sy);
			tab.SetPos(x, y);
			tab.SetSize(sx, sy);
		}
		tab.Show(false);

		AlignTabbers();
		return new_index;
	}

	override void AlignTabbers()
	{
		if (!m_TabControlsRoot)
			return;

		if (CUT_IsOptionsTabber())
		{
			CUT_AlignOptionsCategoryBar();
			return;
		}

		float total_size;
		float x, y;

		Widget tab_controls_container = m_TabControlsRoot.FindAnyWidget("Tab_Control_Container");
		Widget tab_controls_scroller = m_TabControlsRoot.FindAnyWidget("Tab_Control_Scroller");
		if (!tab_controls_container)
			return;

		m_TabControlsRoot.Update();
		tab_controls_container.Update();

		float pad = 10 * m_ResolutionMultiplier;
		if (pad < 10)
			pad = 10;

		Widget tab_child = tab_controls_container.GetChildren();
		while (tab_child)
		{
			if (tab_child.IsVisible())
			{
				TextWidget tab_text = TextWidget.Cast(tab_child.FindAnyWidget(tab_child.GetName() + "_Title"));
				float width = 160;
				if (tab_text)
				{
					int t_x, t_y;
					tab_text.Update();
					tab_text.GetTextSize(t_x, t_y);
					width = t_x + pad;
				}
				if (width < 140)
					width = 160;
				tab_child.SetSize(width, 1);
				tab_controls_container.Update();
				total_size += width;
			}
			tab_child = tab_child.GetSibling();
		}

		tab_child = tab_controls_container.GetChildren();
		float x_f_c, y_f_c;
		tab_controls_container.GetScreenPos(x_f_c, y_f_c);
		while (tab_child)
		{
			Widget tab_bg = tab_child.FindAnyWidget(tab_child.GetName() + "_Background");
			if (tab_bg)
			{
				tab_child.GetScreenPos(x, y);
				tab_bg.SetPos((x_f_c - x), 0);
				tab_bg.SetSize(total_size, 1);
			}
			tab_child = tab_child.GetSibling();
		}

		m_TabControlsRoot.GetSize(x, y);
		if (total_size > 0)
			m_TabControlsRoot.SetSize(total_size, y);
		tab_controls_container.Update();
		if (tab_controls_scroller)
			tab_controls_scroller.Update();
		m_TabControlsRoot.Update();
	}

	protected bool CUT_IsOptionsTabber()
	{
		if (!m_Root)
			return false;

		Widget parent = m_Root.GetParent();
		if (!parent)
			return false;

		return parent.FindAnyWidget("SettingsTextWidget") != null;
	}

	protected void CUT_AlignOptionsCategoryBar()
	{
		Widget tab_controls_container = m_TabControlsRoot.FindAnyWidget("Tab_Control_Container");
		if (!tab_controls_container)
			return;

		m_TabControlsRoot.Update();
		tab_controls_container.Update();

		float min_w = 148;
		float max_w = 220;
		float text_pad = 56;
		float gap = 4;
		float total_size = 0;
		int visible = 0;

		Widget tab_child = tab_controls_container.GetChildren();
		while (tab_child)
		{
			if (tab_child.IsVisible())
			{
				TextWidget tab_text = TextWidget.Cast(tab_child.FindAnyWidget(tab_child.GetName() + "_Title"));
				float width = min_w;
				if (tab_text)
				{
					int t_x;
					int t_y;
					tab_text.Update();
					tab_text.GetTextSize(t_x, t_y);
					width = t_x + text_pad;
				}
				if (width < min_w)
					width = min_w;
				if (width > max_w)
					width = max_w;

				tab_child.SetFlags(WidgetFlags.HEXACTSIZE);
				tab_child.SetSize(width, 1);

				Widget tab_bg = tab_child.FindAnyWidget(tab_child.GetName() + "_Background");
				if (tab_bg)
				{
					tab_bg.ClearFlags(WidgetFlags.HEXACTSIZE);
					tab_bg.SetPos(0, 0);
					tab_bg.SetSize(1, 1);
				}

				total_size += width;
				visible++;
			}
			tab_child = tab_child.GetSibling();
		}

		if (visible > 1)
			total_size += gap * (visible - 1);

		if (total_size > 0)
		{
			tab_controls_container.SetFlags(WidgetFlags.HEXACTSIZE);
			tab_controls_container.SetSize(total_size, 1);
			tab_controls_container.Update();

			float x;
			float y;
			m_TabControlsRoot.GetSize(x, y);
			m_TabControlsRoot.SetFlags(WidgetFlags.HEXACTSIZE);
			m_TabControlsRoot.SetSize(total_size, y);
			m_TabControlsRoot.Update();
		}
	}
}
