namespace FFmpegAviSynthLib
{
    using System;
    using System.Diagnostics;
    using System.IO;
    using System.Runtime.InteropServices;

    public class FFmpegRunner
    {
        private string ffmpegExecutableFilename;

        public FFmpegRunner(string ffmpegExecutableFilename)
        {
            this.FfmpegExecutableFilename = ffmpegExecutableFilename;
        }

        public string FfmpegExecutableFilename
        {
            get { return this.ffmpegExecutableFilename; }
            set { this.ffmpegExecutableFilename = value; }
        }

        public void Start()
        {
            if (string.IsNullOrWhiteSpace(this.FfmpegExecutableFilename))
            {
                throw new InvalidOperationException("Property \"FfmpegExecutableFilename\" must be valid.");
            }

            var startInfo = new ProcessStartInfo();
            startInfo.FileName = this.FfmpegExecutableFilename;
            //@"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\ffmpeg.exe";
            startInfo.Arguments =
                //@" -v info -i SCRIPT.avs -aspect 16:9 -c:v h264_nvenc -rc constqp -preset hq OUT.mp4";
                @" -v info -i D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\SCRIPT.avs -aspect 16:9 -c:v h264_qsv -rc constqp -preset medium OUT.mp4";
            // for intel, possible presets: veryfast, faster, fast, medium, slow, slower, veryslow  (https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/cloud-computing-quicksync-video-ffmpeg-white-paper.pdf)
            /* startInfo.WorkingDirectory =
                 @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\";*/
            this.AddPathForAviSynthInjector(startInfo);
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.WorkingDirectory =
                @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\";


            //this.SymLinkAviSynthTest();
            this.CopyAviSynthDll();

            Process.Start(startInfo);
        }

        private void CopyAviSynthDll()
        {
            // ffmpeg is employing a "safe DLL-load mechanism" - it only loads a DLL from the same directory the exe
            // is residing in and from the system-directory (cf. w32dlfcn.h funtion win_dlopen). So, the easy ways to
            // ensure that ffmpeg loads our "avisynth.dll" (like modifying the %PATH% or changing current directory) will
            // not work. Other options which crossed my mind:
            // (1) inject a thread which loads the DLL ( something like this: https://stackoverflow.com/questions/22750112/dll-injection-with-createremotethread )
            // (2) create a symbolic link or a hard link
            // Well, both are quite involved - so the easiest thing to do is to simply copy the DLL.
            string curExeLocation = System.Reflection.Assembly.GetEntryAssembly().Location;
            string srcAviSynthDll = Path.Combine(Path.GetDirectoryName(curExeLocation), "avisynth.dll");
            string dstAviSynthDll = Path.Combine(Path.GetDirectoryName(this.FfmpegExecutableFilename), "avisynth.dll");
            File.Copy(srcAviSynthDll, dstAviSynthDll, true);
        }

        private void AddPathForAviSynthInjector(ProcessStartInfo startInfo)
        {
            var env = startInfo.EnvironmentVariables;

            string path = env["Path"];
            path = @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug;" + path;
            env["Path"] = path;
        }
    }
}
