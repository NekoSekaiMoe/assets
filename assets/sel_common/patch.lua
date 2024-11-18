helper_print("patching all sel_common_* files")

local da = "assets/sel_common/" --asset directory
local dg = "tmp/bin/PSP_GAME/USRDIR/sprite/" --game directory

merge("sel_common_kiridashi", da, dg)
merge("sel_common_load_save_kiridashi", da, dg)
merge("sel_common_system_window", da, dg)
--merge_16col("sel_common_load_tape_kiridashi", da, dg) --16col is a bit buggy, so we use pre-compressed textures

if psp then
    helper_copy(da.."sel_common_load_tape_kiridashi_psp.png.keep", dg.."sel_common_load_tape_kiridashi.png")
else --ps3
    helper_copy(da.."sel_common_load_tape_kiridashi_ps3.png.keep", dg.."sel_common_load_tape_kiridashi.png")
end
png2uvr_palette(dg.."sel_common_load_tape_kiridashi.png", 16, dg.."sel_common_load_tape_kiridashi.uvr")
if cleanup then helper_delete(dg.."sel_common_load_tape_kiridashi.png") end
