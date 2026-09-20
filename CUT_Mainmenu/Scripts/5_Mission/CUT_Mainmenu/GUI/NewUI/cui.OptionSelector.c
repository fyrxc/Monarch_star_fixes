modded class OptionSelector
{
	override bool OnClick(Widget w, int x, int y, int button)
	{
		bool handled = super.OnClick(w, x, y, button);
		if (button == MouseState.LEFT)
			CUT_MenuUISound.PlayLight();
		return handled;
	}
};
