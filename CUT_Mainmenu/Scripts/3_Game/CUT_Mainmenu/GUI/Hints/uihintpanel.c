modded class UiHintPanel extends ScriptedWidgetEventHandler
{	
	protected const string m_DataPath = "CUT_Mainmenu/Scripts/data/hints.json";
	protected string m_RootPath       = "CUT_Mainmenu/Gui/layouts/new_ui/hints/cui.in_game_hints.layout";	
}


modded class UiHintPanelLoading extends UiHintPanel
{
 	protected ImageWidget m_tipLineL;
	protected ImageWidget m_tipLineR;
	protected ImageWidget m_tipIcon;
	protected ImageWidget m_tShader;
	protected ImageWidget m_bShader;

	override void Init(DayZGame game)
	{
		m_RootPath = "CUT_Mainmenu/gui/layouts/new_ui/hints/cui.in_game_hints_load.layout";

		if (m_Initialized)
			return;
		if (!game)
			return;

		m_Initialized = true;
		m_Game = game;
		LoadContentList();
		if (m_ContentList)
		{
			BuildLayout(m_ParentWidget);
			RandomizePageIndex();
			PopulateLayout();
			if (m_UiHeadlineLabel)
				StartSlideshow();
		}
	}

	override protected void BuildLayout(Widget parent_widget)
	{
		m_RootFrame = m_Game.GetWorkspace().CreateWidgets(m_RootPath, parent_widget);

		if (!m_RootFrame)
			return;

		m_tipLineL          = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LinesImageLeft"));
		m_tipLineR          = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LinesImageRight"));
		m_SpacerFrame       = m_RootFrame.FindAnyWidget("Hints");
		m_tipIcon           = ImageWidget.Cast(m_RootFrame.FindAnyWidget("hintIcon"));
		m_tShader           = ImageWidget.Cast(m_RootFrame.FindAnyWidget("TopShader"));
		m_bShader           = ImageWidget.Cast(m_RootFrame.FindAnyWidget("BottomShader"));
		m_UiLeftButton      = ButtonWidget.Cast(m_RootFrame.FindAnyWidget("LeftButton"));
		m_UiRightButton     = ButtonWidget.Cast(m_RootFrame.FindAnyWidget("RightButton"));
		m_UiHeadlineLabel   = TextWidget.Cast(m_RootFrame.FindAnyWidget("HeadlineLabel"));
		m_UiDescLabel       = RichTextWidget.Cast(m_RootFrame.FindAnyWidget("HintDescLabel"));
		m_UiHintImage       = ImageWidget.Cast(m_RootFrame.FindAnyWidget("HintImage"));
		m_UiPageingLabel    = TextWidget.Cast(m_RootFrame.FindAnyWidget("PageInfoLabel"));

		if (m_tipIcon)
			m_tipIcon.SetColor(colorScheme.TipIcon());
		if (m_tipLineL)
			m_tipLineL.SetColor(colorScheme.TipLine());
		if (m_tipLineR)
			m_tipLineR.SetColor(colorScheme.TipLine());
		if (m_UiHeadlineLabel)
			m_UiHeadlineLabel.SetColor(colorScheme.TipHeader());
		if (m_UiDescLabel)
			m_UiDescLabel.SetColor(colorScheme.TipText());
		if (m_tShader)
			m_tShader.SetColor(colorScheme.TopShader());
		if (m_bShader)
			m_bShader.SetColor(colorScheme.BottomShader());

		m_RootFrame.SetHandler(this);
	}

	override protected void PopulateLayout()
	{
		if (!m_RootFrame || !m_UiHeadlineLabel)
			return;

		super.PopulateLayout();
	}

	override protected void SetHintHeadline()
	{
		if (m_UiHeadlineLabel)
			super.SetHintHeadline();
	}

	override protected void SetHintDescription()
	{
		if (m_UiDescLabel)
			super.SetHintDescription();
	}

	override protected void SetHintImage()
	{
		if (m_UiHintImage)
			super.SetHintImage();
	}
}
