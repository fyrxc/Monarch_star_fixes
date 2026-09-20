# CUT Mainmenu

Crashout / CUT DayZ main-menu pack. This repo contains the unpacked `CUT_Mainmenu` mod with a compact Options / Settings menu.

## Options spacing fix

The Options menu looked stretched because **setting rows were 30px tall inside 473px group spacers**. Vanilla Game / Video / Sounds / Controls tabs (`*_settings_root` WrapSpacers) keep that 473px height, so GridSpacer spreads a handful of rows across the whole block.

CUT also used a **90px Back button** in a **400px** action column, so Apply / Undo / Defaults sat far apart.

Those parent containers now hug their content. Widget names, IDs, and settings are unchanged. Scrollbars still apply when a tab is taller than the 800px panel.

## Options category bar

The GAME / AUDIO / VIDEO / CONTROLS / HITMARKER / CRASHOUT tabs sat in a **1400px GridSpacer with 10 columns**, so they were stretched evenly across the screen. That container is now a **WrapSpacer** that packs each tab to a compact width (about 150–200px) with a 4px gap. The **OPTIONS** title stays right-aligned and is unchanged.

## Label / control overlap fix

After the first compact pass, **labels painted over dropdowns and sliders** (Game tab, VPPAdminTools camera rows, etc.). Group WrapSpacers still had **Size To Content H**, so rows shrank to the option widget instead of staying **650px** wide. A 50/50 split of that narrow row made long captions collide with the control.

Rows now keep the full scroll width. Labels use **58%** with clipping; option widgets use **42%**. Injected rows from other mods are resized the same way after the menu opens.

## Install

Copy `CUT_Mainmenu` onto your DayZ Workbench P: drive as `P:\CUT_Mainmenu` (replace the existing folder). Repack / file-patch as you usually do.

## Run locally

1. Open `CUT_Mainmenu/Workbench/dayz.gproj` in DayZ Tools Workbench.
2. Launch with this mod loaded.
3. Main Menu → Settings, then Game / Sounds / Video / Controls.

## Files changed for spacing

| File | What changed |
| --- | --- |
| `GUI/layouts/new_ui/options/pc/cui.options_menu.layout` | Action column `play_panel_root` 400→170, sizes to content, Padding 4. Back 90→36. Apply / Reset / Defaults 38→36. Details header 44→32, details padding 8→4. |
| `GUI/layouts/new_ui/options/pc/cui.keybinding_menu.layout` | Same button compacting. `group_header` 128→40. |
| `GUI/layouts/new_ui/options/pc/cui.game_tab.layout` | Compact PC Game tab. Groups hug vertically and stay 650px wide. Rows 34px. Label/option **0.58 / 0.42** with clipped labels. |
| `GUI/layouts/new_ui/options/pc/cui.video_tab.layout` | Same group compacting and column split. |
| `GUI/layouts/new_ui/options/pc/cui.sound_tab.layout` | Same group compacting and column split. |
| `GUI/layouts/new_ui/options/pc/cui.controls_tab.layout` | Same group compacting and column split. Dummy spacers stay 6px. |
| `Scripts/.../Options/cui.OptionsMenuTabs.c` | `GetLayoutName()` points Game / Sounds / Video / Controls at the compact tabs (PC only). |
| `Scripts/.../Options/cui.OptionsMenu.c` | After init / on show, collapse leftover `*_settings_root` shells taller than 80px, force 650px row width, and fix injected 2-column rows (Hitmarker / Crashout / VPPAdminTools). |
