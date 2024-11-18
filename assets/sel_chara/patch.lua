helper_print("patching all sel_chara_* files")

local da = "assets/sel_chara/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_chara_mugi_etc_kiridashi", da, dg)
