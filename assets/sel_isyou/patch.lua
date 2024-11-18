helper_print("patching all sel_isyou_* files")

local da = "assets/sel_isyou/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_isyou_edit_kiridashi", da, dg)
