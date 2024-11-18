helper_print("patching all sel_option_* files")

local da = "assets/sel_option/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_option", da, dg)
