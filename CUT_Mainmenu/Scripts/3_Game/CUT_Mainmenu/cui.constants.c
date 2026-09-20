// Constants.c v2.5.0

// RANDO IMAGES ----------------------------------------------------------------
// If you dont want to use the UiHintPanel you can use these instead.
string GetRandomBackground()
{
	const string images[] = {
		"CUT_Mainmenu/GUI/textures/loading_screens/Loading1.paa",
		"CUT_Mainmenu/GUI/textures/loading_screens/Loading2.paa",
		"CUT_Mainmenu/GUI/textures/loading_screens/Loading3.paa"
	};
	// If you add more images be sure to change the image count.
	const int IMAGES_COUNT = 3;
	int bgIndex = Math.RandomInt(0, IMAGES_COUNT);
	return images[bgIndex];
}

static bool CUI_ShowDeadScreen = false;
static bool CUI_DeadScreenFaded = false;
// Match vanilla DEFAULT_DYING_TIME so the death image eases in instead of popping.
static float CUI_DeadFadeSeconds = 2.5;

static bool CUI_IsDeathScreenLocked()
{
	if (CUI_ShowDeadScreen)
		return true;
	if (!GetGame())
		return false;
	Man player = GetGame().GetPlayer();
	return player && player.GetPlayerState() != EPlayerStates.ALIVE;
}

string GetRandomGameOverScreen()
{
	const string images[] = {
		"CUT_Mainmenu/GUI/textures/globals/DeathScreen.paa", 
		"CUT_Mainmenu/GUI/textures/globals/DeathScreen.paa",
		"CUT_Mainmenu/GUI/textures/globals/DeathScreen.paa"
	};
	// If you add more images be sure to change the image count. 
	const int IMAGES_COUNT = 3;
	int bgIndex = Math.RandomInt(0, IMAGES_COUNT - 1);
	return images[bgIndex];
}

class GameOverScreen
{
    static string GameOverScreenImage() { return "CUT_Mainmenu/GUI/textures/globals/DeathScreen.paa"; };
};

// SOCIALS & LINKS ---------------------------------------------------------------
// Set your links Here
class MenuURLS {
	static string urlDiscord   = "https://discord.gg/crashoutdayz";
	static string urlYoutube   = "https://www.youtube.com/@crashout_dayz";
	static string urlTwitter   = "https://x.com/crashout_dayz";
	static string urlTikTok    = "https://www.tiktok.com/@crashout_dayz";
	static string urlFacebook  = "https://www.facebook.com/profile.php?id=61593243257179";
	static string urlInstagram = "https://www.instagram.com/crashout_dayz/";
	static string urlReddit    = "#";
	static string urlWebsite   = "https://crashoutdayz.com";
	static string urlShop      = "https://monarch-dayz.com/shop";
	static string urlPriorityQ = "https://discord.gg/crashoutdayz";
	static string urlCustom    = "#";
	// DZSA join links — empty until set. Empty MAIN falls back to vanilla Play().
	static string urlCrashoutMain = "";
	static string urlCrashoutDM   = "";
}

// DIALOGS ------------------------------------------------------------------------
// There is No Need to mess with anything below this line. 
const int COLORFUL_EXIT				= 666;
const int COLORFUL_CONFIGURE 		= 667;
const int COLORFUL_DEFAULTS 		= 668;
