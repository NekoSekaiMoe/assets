helper_print("patching all sel_tushin_* files")

local da = "assets/sel_tushin/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_tushin_kiridashi", da, dg)
