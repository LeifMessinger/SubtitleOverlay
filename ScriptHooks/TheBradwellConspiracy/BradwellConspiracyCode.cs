//Use dnSpy and open TheBradwellConspiracy\TheBradwellConspiracy_Data\Managed\Assembly-CSharp.dll
//Click into it > right click the code > Edit class
//At the top of Assembly-CSharp.dll
using System.IO;

//In GuideUIController::DisplaySubtitle(line) and (line, time)
string tempPath = System.IO.Path.GetTempPath();
File.WriteAllText(System.IO.Path.Combine(tempPath, "SubtitlesCurrent.txt"), dialogueLine.Character + ": " + dialogueLine.Subtitles);
//Only do this in (line, time)
double waitUntilTime = DateTime.UtcNow.Subtract(new DateTime(1970, 1, 1)).TotalSeconds + (double)duration;
File.WriteAllText(System.IO.Path.Combine(tempPath, "SubtitlesCurrentWaitUntil.txt"), "" + ((long)waitUntilTime) * 1000L);