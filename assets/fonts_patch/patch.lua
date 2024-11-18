helper_print("patching the SYS_runtime font files")

local da = "assets/fonts_patch/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/font/" --game directory

merge("SYS_RunTime", da, dg)
