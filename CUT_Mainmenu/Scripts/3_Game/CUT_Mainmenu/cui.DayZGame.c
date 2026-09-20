// Priority Queue ----------------------------------
modded class LoginQueueBase extends LoginScreenBase
{
    protected ImageWidget m_tShader;
    protected ImageWidget m_bShader;
    protected ProgressBarWidget m_ProgressLoading;

    override Widget Init()
    {    
        // Use CUI Layout    
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("CUT_Mainmenu/gui/layouts/cui.dialog_queue_position.layout");
        Widget hintFrame = layoutRoot.FindAnyWidget("hint_frame0");
        if (hintFrame)
            m_HintPanel = new UiHintPanelLoading(hintFrame);
        m_txtPosition = TextWidget.Cast(layoutRoot.FindAnyWidget("LoadingMsg"));
        m_txtNote = TextWidget.Cast(layoutRoot.FindAnyWidget("txtNote"));
        m_btnLeave = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
        
        m_tShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_bShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
        m_ProgressLoading = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));

        Widget notification = layoutRoot.FindAnyWidget("notification_root");
        if (notification)
            notification.Show(false);
        if (m_txtNote)
            m_txtNote.Show(true);
        if (m_btnLeave)
            m_btnLeave.SetColor(colorScheme.TipIcon());
        if (m_tShader)
            m_tShader.SetColor(colorScheme.TopShader());
        if (m_bShader)
            m_bShader.SetColor(colorScheme.BottomShader());
        if (m_ProgressLoading)
            m_ProgressLoading.SetColor(colorScheme.Loadingbar());

        return layoutRoot;
    }

    override void SetPosition(int position)
    {
        if (position != m_iPosition)
        {
            m_iPosition = position;
            if (m_txtPosition)
            {
                m_txtPosition.SetText("Position in Queue " + position.ToString());
                m_txtPosition.SetColor(colorScheme.LoadingMsg());
            }
        }
    }
};

// Logging In Screen ------------------------------
modded class LoginTimeBase extends LoginScreenBase
{
    protected ImageWidget m_LSBackground;
    protected ImageWidget m_tShader;
    protected ImageWidget m_bShader;
    protected ImageWidget m_tipIcon;
    protected TextWidget m_loadingMsg;
    protected ImageWidget m_exitIcon;
    protected TextWidget m_ExitText;
    protected ProgressBarWidget m_ProgressLoading;

    override Widget Init()
    {
        // Use CUI Layout
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("CUT_Mainmenu/gui/layouts/cui.dialog_login_time.layout");
        // CUI 2.0 Elements
        m_LSBackground = ImageWidget.Cast(layoutRoot.FindAnyWidget("Background"));
        m_exitIcon = ImageWidget.Cast(layoutRoot.FindAnyWidget("Exit"));
        m_tShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_bShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
        m_loadingMsg = TextWidget.Cast(layoutRoot.FindAnyWidget("LoadingMsg"));
        m_LSBackground = ImageWidget.Cast(layoutRoot.FindAnyWidget("ImageBackground"));
        m_ProgressLoading = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
        m_ExitText = TextWidget.Cast(layoutRoot.FindAnyWidget("ExitText"));
        // Vanilla Elements
        m_txtDescription = TextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
        m_txtLabel = TextWidget.Cast(layoutRoot.FindAnyWidget("txtLabel"));
        m_btnLeave = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
        if (m_txtDescription)
            m_txtDescription.Show(true);
        Widget notification = layoutRoot.FindAnyWidget("notification_root");
        if (notification)
            notification.Show(false);
        // Theme the elements. 
        if (m_LSBackground)
            m_LSBackground.LoadImageFile(0, GetRandomBackground());
        if (m_ProgressLoading)
            m_ProgressLoading.SetColor(colorScheme.Loadingbar());
        if (m_tShader)
            m_tShader.SetColor(colorScheme.TopShader());
        if (m_bShader)
            m_bShader.SetColor(colorScheme.BottomShader());
        if (m_exitIcon)
            m_exitIcon.SetColor(colorScheme.TipIcon());
        if (m_txtLabel)
            m_txtLabel.SetColor(colorScheme.LoadingMsg());
        return layoutRoot;
    }   
    
    override bool OnMouseEnter(Widget w, int x, int y)
    {
        if (w == m_btnLeave)
        {
            if (m_ExitText)
                m_ExitText.SetColor(colorScheme.TipIcon());
            m_btnLeave.SetColor(UIColor.Transparent());
            return true;
        }
        return false;
    }
    
    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        if (w == m_btnLeave)
        {
            if (m_ExitText)
                m_ExitText.SetColor(colorScheme.PrimaryText());
            return true;
        }
        return false;
    }   
    
    // Change the wording in the countdown timer text
    override void SetTime(int time)
    {
        string text = "";
        TimeConversions.ConvertSecondsToFullTime(time, m_FullTime);
        if (!m_IsRespawn)
            text = "#menu_loading_in_";
        else
            text = "#dayz_game_spawning_in_";

        if (m_FullTime.m_Days > 0)
            text += "dhms";
        else if (m_FullTime.m_Hours > 0)
            text += "hms";
        else if (m_FullTime.m_Minutes > 0)
            text += "ms";
        else
            text += "s";
            
        text = Widget.TranslateString(text);
        text = string.Format(text, m_FullTime.m_Seconds, m_FullTime.m_Minutes, m_FullTime.m_Hours, m_FullTime.m_Days);
        if (m_txtLabel)
        {
            m_txtLabel.SetText(text);
            m_txtLabel.SetText("CONNECTING TO SERVER IN " + time.ToString());
        }

        if (m_IsRespawn && time <= 1)
            GetGame().SetLoginTimerFinished();
    }

    override void SetStatus(string status)
    {
        if (m_txtDescription)
            m_txtDescription.SetText(status);
    }

    override void Show()
    {
        if (layoutRoot)
        {
            layoutRoot.Show(true);
            Widget hintFrame = layoutRoot.FindAnyWidget("hint_frame0");
            if (hintFrame)
                m_HintPanel = new UiHintPanelLoading(hintFrame);
        }
    }
};

// Loading Screen (UH DUH!) --------------------
modded class LoadingScreen
{
    protected ImageWidget m_Background;
    protected ImageWidget m_tShader;
    protected ImageWidget m_bShader;
    protected ImageWidget m_tipIcon;
    protected TextWidget m_loadingMsg;

    void LoadingScreen(DayZGame game)
    {
        m_DayZGame = game;
        // Use the CUI 2.0 custom layout 
        m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("CUT_Mainmenu/gui/layouts/cui.loading.layout");
        
        // CUI 2.0 Elements
        Class.CastTo(m_tShader, m_WidgetRoot.FindAnyWidget("TopShader"));
        Class.CastTo(m_bShader, m_WidgetRoot.FindAnyWidget("BottomShader"));
        Class.CastTo(m_loadingMsg, m_WidgetRoot.FindAnyWidget("LoadingMsg"));
        
        // Load BG and Progress Bar
        m_Background = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));
        m_ProgressLoading = ProgressBarWidget.Cast(m_WidgetRoot.FindAnyWidget("LoadingBar"));
        ProgressAsync.SetProgressData(m_ProgressLoading);
        ProgressAsync.SetUserData(m_Background);
        if (m_Background)
        {
            m_Background.Show(true);
        }

        if (m_ProgressLoading)
            m_ProgressLoading.SetColor(colorScheme.Loadingbar());
        if (m_tShader)
            m_tShader.SetColor(colorScheme.TopShader());
        if (m_bShader)
            m_bShader.SetColor(colorScheme.BottomShader());
        if (m_loadingMsg)
        {
            m_loadingMsg.SetColor(colorScheme.LoadingMsg());
            m_loadingMsg.SetText("GAME IS LOADING!");
        }
    }

    override void SetTitle(string title)
    {
        // This function is intentionally left blank because it is no longer used.
    }

    override void Show()
    {
        // Use Random Background Images
        m_Background = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));
        if (m_Background)
            m_Background.LoadImageFile(0, GetRandomBackground()); 
    }
};
