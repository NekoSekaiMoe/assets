helper_print("patching all sel_timer_* files")

local da = "assets/sel_timer/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

--convert yui to UVR and back, to limit the colors (then use the limited color version for the rest of the files)
uvr2png(dg.."sel_timer_widow_kiridashi_yu.uvr")
helper_merge(da.."sel_timer_widow_kiridashi_xx.png", da.."sel_timer_widow_kiridashi_xx_mask.png", dg.."sel_timer_widow_kiridashi_yu.png")
png2uvr(dg.."sel_timer_widow_kiridashi_yu.png")
uvr2png(dg.."sel_timer_widow_kiridashi_yu.uvr")


for i, v in ipairs({"mi", "mu", "ri", "az", "sa", "ui", "no"}) do
    uvr2png(dg.."sel_timer_widow_kiridashi_"..v..".uvr")
    helper_merge(dg.."sel_timer_widow_kiridashi_yu.png", da.."sel_timer_widow_kiridashi_xx_mask.png", dg.."sel_timer_widow_kiridashi_"..v..".png")
    png2uvr(dg.."sel_timer_widow_kiridashi_"..v..".png")
    if cleanup then helper_delete(dg.."sel_timer_widow_kiridashi_"..v..".png") end
end

--same as above, but with moji files
uvr2png(dg.."sel_timer_moji_kiridashi_yu.uvr")
helper_merge(da.."sel_timer_moji_kiridashi_xx.png", da.."sel_timer_moji_kiridashi_xx_mask.png", dg.."sel_timer_moji_kiridashi_yu.png")
png2uvr(dg.."sel_timer_moji_kiridashi_yu.png")
uvr2png(dg.."sel_timer_moji_kiridashi_yu.uvr")

for i, v in ipairs({"mi", "mu", "ri", "az", "sa", "ui", "no"}) do
    uvr2png(dg.."sel_timer_moji_kiridashi_"..v..".uvr")
    helper_merge(dg.."sel_timer_moji_kiridashi_yu.png", da.."sel_timer_moji_kiridashi_xx_mask.png", dg.."sel_timer_moji_kiridashi_"..v..".png")
    png2uvr(dg.."sel_timer_moji_kiridashi_"..v..".png")
    if cleanup then helper_delete(dg.."sel_timer_moji_kiridashi_"..v..".png") end
end
