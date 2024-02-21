print("Added script hook for subtitles")

if CLIENT then
	hook.Add("OnCloseCaptionEmit", "LibreSubtitles", function(soundScriptString, duration, fromPlayer, fullText)
		local outfile_path = "C:\\Users\\leifm\\Downloads\\SubtitleOverlay\\GModSubtitles.lua"
		local outfile = assert(io.open(outfile_path, "w"))	--Truncates and writes
		
		outfile.write(fullText);
	end)
end