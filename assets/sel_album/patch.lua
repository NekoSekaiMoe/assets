helper_print("patching all sel_album_* files")

local da = "assets/sel_album/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_album", da, dg)
merge("sel_album_sou_bgm", da, dg)

--disable since they're blurry and tons of work to grab
--replace("sel_album_eve_cap_01", da, dg)
