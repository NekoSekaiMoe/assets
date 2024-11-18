helper_print("patching all atr_* files")

local da = "assets/atr/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("atr_title_2_kiridashi", da, dg)
merge("atr_logo_sega", da, dg)
