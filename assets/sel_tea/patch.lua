helper_print("patching all sel_tea_* files")

local da = "assets/sel_tea/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory


merge("sel_tea_kiridashi", da, dg)
merge("sel_tea_item_kiridashi", da, dg)
for i = 1, 4 do merge("sel_tea_kisekae_tutorial_"..i.."_kiridashi", da, dg) end
for i = 1, 6 do replace("sel_tea_ensou_tutorial_"..i.."_kiridashi", da, dg) end
for i = 1, 3 do replace("sel_tea_item_tutorial_"..i.."_kiridashi", da, dg) end
for i = 1, 4 do replace("sel_tea_tokei_tutorial_"..i.."_kiridashi", da, dg) end
for i = 1, 5 do replace("sel_tea_tushin_tutorial_"..i.."_kiridashi", da, dg) end
for i = 1, 6 do replace("sel_tea_utaou_tutorial_"..i.."_kiridashi", da, dg) end
for i = 1, 4 do replace("sel_tea_album_tutorial_"..i.."_kiridashi", da, dg) end


if psp then
    replace("sel_tea_system_tutorial_1_kiridashi", da, dg)
    replace("sel_tea_system_tutorial_2_kiridashi", da, dg)
else --on ps3:
    replace_16col("sel_tea_system_tutorial_1_kiridashi", da, dg)
    replace_16col("sel_tea_system_tutorial_2_kiridashi", da, dg)
end
