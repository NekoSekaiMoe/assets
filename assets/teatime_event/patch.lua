helper_print("patching all teatime events")

local da = "assets/teatime_event/" --asset directory
local dg = "tmp/std.cpk_folder/PSP_GAME/USRDIR/teatime_event/" --game directory

events = {1, 2, 3, 4, 5, 6, 7, 55, 56, 57, 71, 202, 303, 304, 402, 404, 405, 502, 505, 601, 602, 604, 702, 802, 901, 902, 903, 904, 1001, 1003, 1005, 1305, 9901, 9902, 9905, 9906, 9907, 9913, 9915, 9917, 9918, 9920, 9921, 9922, 9923, 9925, 9926, 9927, 9928, 9929}
event_scripts = {}

local function has_value (tab, val)
    for index, value in ipairs(tab) do
        if value == val then
            return true
        end
    end

    return false
end


for i, v in ipairs(events) do
    tts_name = "TTS_Nm_"..string.format("%04d", v)..".tts"
    tts_path = dg.."Event/"..tts_name
    tts_unpack(tts_path, tts_path.."_folder")
    helper_copy(da..tts_name.."_folder", tts_path.."_folder")
    
    if(has_value(event_scripts, v)) then
        helper_copy(da..string.format("%04d_script.txt", v), tts_path.."_folder/script.txt")
    end
    
    if psp then
        helper_halve(tts_path.."_folder/1.png", tts_path.."_folder/1.png")
    end
    
    uvr_pack(tts_path.."_folder/1.png", 16, tts_path.."_folder/1.uvr")
    helper_delete(tts_path.."_folder/1.png")
    tts_pack(tts_path.."_folder", tts_path)
    
    if cleanup then
        helper_delete(tts_path.."_folder")
    end
end
