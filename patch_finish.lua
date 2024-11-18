if not output_iso then output_iso = "./game_patched.iso" end


if not ends_with then
    function ends_with(str, ending)
        return ending == "" or str:sub(-#ending) == ending
    end
end


for i, e in pairs(helper_list("tmp/bin", true)) do
    if ends_with(e, ".bin.package.txt") then
        local in_path = e
        local out_path = string.gsub(string.gsub(e, "tmp/bin", "tmp/std.cpk_folder"), ".package.txt", "")
        fmdx_pack(in_path, out_path)
    end
end

cpk_pack("tmp/std.cpk_folder/", "tmp/game.iso_folder/PSP_GAME/USRDIR/std.cpk")

if windows then 
	helper_execute("assets/tools/mkisofs.exe -iso-level 4 -xa -A \"PSP GAME\" -V \"A\" -sysid \"PSP GAME\" -volset \"A\" -p \"A\" -publisher \"A\" -o \""..output_iso.."\" tmp/game.iso_folder/")
elseif linux then
	helper_execute("assets/tools/mkisofs -iso-level 4 -xa -A \"PSP GAME\" -V \"A\" -sysid \"PSP GAME\" -volset \"A\" -p \"A\" -publisher \"A\" -o \""..output_iso.."\" tmp/game.iso_folder/")
else
	helper_print("error: not a supported platform")
end
