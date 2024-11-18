helper_print("patching all sel_save_* files")

local da = "assets/sel_save/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_save", da, dg)
