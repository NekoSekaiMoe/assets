if not input_iso then input_iso = "./game.iso" end

local function ends_with(str, ending)
   return ending == "" or str:sub(-#ending) == ending
end

iso_unpack(input_iso, "tmp/game.iso_folder")
cpk_unpack("tmp/game.iso_folder/PSP_GAME/USRDIR/std.cpk", "tmp/std.cpk_folder")

for i, e in pairs(helper_list("tmp/std.cpk_folder/PSP_GAME/USRDIR/bind", true)) do
    --filter out mgr_pv_effect_dummy because it contains dummy files which will otherwise overwrite the "real" files we want
    if ends_with(e, ".bin") and not ends_with(e, "dummy.bin") then
        local in_path = e
        local out_path = string.gsub(e, "tmp/std.cpk_folder/PSP_GAME/USRDIR/bind", "tmp/bin/PSP_GAME/USRDIR/bind")
        fmdx_unpack(in_path, out_path)
    end
end
