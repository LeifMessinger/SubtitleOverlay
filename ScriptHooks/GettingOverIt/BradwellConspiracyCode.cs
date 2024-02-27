//Use dnSpy and open Getting\\ Over\\ It\GettingOverIt_Data\Managed\Assembly-CSharp.dll
//Click into it > right click the code > Edit class
//At the top of Assembly-CSharp.dll
using System.IO;

//After the line: if (this.currentSubtitleBuffer.Count > 0 && this.subtitleTimer <= 0f)
string tempPath = System.IO.Path.GetTempPath();
File.WriteAllText(System.IO.Path.Combine(tempPath, "SubtitlesCurrent.txt"), this.currentSubtitleBuffer[0]);
double waitUntilTime = DateTime.UtcNow.Subtract(new DateTime(1970, 1, 1)).TotalSeconds + (double)this.currentSubtitleTimeBuffer[0];
File.WriteAllText(System.IO.Path.Combine(tempPath, "SubtitlesCurrentWaitUntil.txt"), "" + ((long)waitUntilTime) * 1000L);

//It might have some compiler errors. Just add (byte) before those variables.