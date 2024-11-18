if not cleanup then cleanup = true end

--DEBUG
--psp = true
if PSP then psp=true end

if not did_prepare then
    execute_lua("patch_prepare.lua")
end

--helper functions:
function uvr2png(input)
    local output = string.gsub(input, ".uvr", ".png")
    uvr_unpack(input, output)
end

function png2uvr(input)
    pal_size = 256
    local output = string.gsub(input, ".png", ".uvr")
    uvr_pack(input, pal_size, output)
end

function png2uvr_palette(input, palette_size)
    pal_size = palette_size or 256
    local output = string.gsub(input, ".png", ".uvr")
    uvr_pack(input, pal_size, output)
end

function merge(input, dasset, dgame)
    uvr2png(dgame..input..".uvr")
    helper_merge(dasset..input..".png", dasset..input.."_mask.png", dgame..input..".png")
    png2uvr(dgame..input..".png")
end

function merge_16col(input, dasset, dgame)
    uvr2png(dgame..input..".uvr")
    helper_merge(dasset..input..".png", dasset..input.."_mask.png", dgame..input..".png")
    png2uvr_palette(dgame..input..".png", 16)
end

function replace(input, dasset, dgame)
    helper_copy(dasset..input..".png", dgame..input..".png")
    if psp then helper_halve(dasset..input..".png", dgame..input..".png") end
    png2uvr(dgame..input..".png")
    if cleanup then helper_delete(dgame..input..".png") end
end

function replace_16col(input, dasset, dgame)
    helper_copy(dasset..input..".png", dgame..input..".png")
    if psp then helper_halve(dasset..input..".png", dgame..input..".png") end
    png2uvr_palette(dgame..input..".png", 16)
    if cleanup then helper_delete(dgame..input..".png") end
end


--copy files for XMB menu
helper_copy("assets/ICON0.PNG.keep", "tmp/game.iso_folder/PSP_GAME/INSDIR/ICON0.PNG")
helper_copy("assets/ICON0.PNG.keep", "tmp/game.iso_folder/PSP_GAME/ICON0.PNG")
if psp then
	helper_copy("assets/PARAM.SFO.PSP", "tmp/game.iso_folder/PSP_GAME/PARAM.SFO")
else
	helper_copy("assets/PARAM.SFO.PS3", "tmp/game.iso_folder/PSP_GAME/PARAM.SFO")
end

--delete install files (since they're useless when playing on memory stick)
if psp then
    helper_delete("tmp/game.iso_folder/PSP_GAME/INSDIR/STD.DNS")
end


--enable "patch_override" to manually select which patches to enable, then put the patches to enable
--into the variable as patch_one,patch_two etc, making sure to separate them with a ,
local override_table = {}
if patch_override then 
	for token in string.gmatch(patch_override, "[^,]+") do
		table.insert(override_table, token)
		print(token)
		override_table[token] = true
		print(override_table[token])
	end
end

function patcher(name, patch)
    if patch_override then
        if override_table[name] == nil then
            helper_print("skipping "..name.." because it wasn't specified in the patch_override list")
            return
        end
    end
    
    if patch:sub(-#".lua") == ".lua" then --if extension is .lua
        execute_lua(patch)
    else
        execute_list(patch)
    end
end

--actual patches
patcher("sel_tushin", "assets/sel_tushin/patch.lua")
patcher("sel_tea", "assets/sel_tea/patch.lua")
patcher("sel_utaou", "assets/sel_utaou/patch.lua")
patcher("atr", "assets/atr/patch.lua")
patcher("sel_common", "assets/sel_common/patch.lua")
patcher("sel_option", "assets/sel_option/patch.lua")
patcher("sel_chara", "assets/sel_chara/patch.lua")
patcher("sel_item", "assets/sel_item/patch.lua")
patcher("sel_save", "assets/sel_save/patch.lua")
patcher("sel_music", "assets/sel_music/patch.lua")
patcher("sel_album", "assets/sel_album/patch.lua")
patcher("sel_timer", "assets/sel_timer/patch.lua")
patcher("sel_isyou", "assets/sel_isyou/patch.lua")
patcher("teatime_event", "assets/teatime_event/patch.lua")
patcher("fonts_patch", "assets/fonts_patch/patch.lua")
patcher("cp_result", "assets/cp_result/patch.txt")
patcher("text", "assets/text/patch.txt")
patcher("cp_eff", "assets/cp_eff/patch.txt")

if not did_prepare then
    execute_lua("patch_finish.lua")
end
