helper_print("patching all sel_utaou_* files")

local da = "assets/sel_utaou/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_utaou_kiridashi", da, dg)
merge("sel_utaou_song_kiridashi", da, dg)
