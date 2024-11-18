helper_print("patching all sel_music_* files")

local da = "assets/sel_music/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_music_kiridashi", da, dg)
merge("sel_music_stage", da, dg)
merge("sel_music_bar_title", da, dg)
