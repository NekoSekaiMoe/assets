helper_print("patching all sel_item_* files")

local da = "assets/sel_item/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_item_etc_kiridashi", da, dg)
merge("sel_item_mongon_kiridashi", da, dg)
for i, v in ipairs({"03", "04", "05", "06", "07", 10, 14, 15, 16, 18, 19}) do 
    replace("sel_item_song_"..v.."_kiridashi", da, dg)
end
