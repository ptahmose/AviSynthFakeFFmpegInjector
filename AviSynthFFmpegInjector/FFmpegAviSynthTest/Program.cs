namespace FFmpegAviSynthTest
{
    using System;
    using System.Diagnostics;
    using System.Threading;

    using FFmpegAviSynthLib;

    using FFmpegAviSynthTest.SharedMem;

    class Program
    {
        static void StartFfmpeg()
        {
            var ffmpegRunner = new FFmpegRunner(
                /*@"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\ffmpeg.exe"*/
                @"L:\temp\ffmpeg-4.0.2-win64-static\bin\ffmpeg.exe");
            ffmpegRunner.Start();
            //var startInfo = new ProcessStartInfo();
            //startInfo.FileName =
            //    @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\ffmpeg.exe";
            //startInfo.Arguments =
            //    //@" -v info -i SCRIPT.avs -aspect 16:9 -c:v h264_nvenc -rc constqp -preset hq OUT.mp4";
            //    @" -v info -i SCRIPT.avs -aspect 16:9 -c:v h264_qsv -rc constqp -preset medium OUT.mp4";
            //// for intel, possible presets: veryfast, faster, fast, medium, slow, slower, veryslow  (https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/cloud-computing-quicksync-video-ffmpeg-white-paper.pdf)
            //startInfo.WorkingDirectory =
            //    @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\";
            //var env = startInfo.EnvironmentVariables;
            //startInfo.CreateNoWindow = false;

            //Process.Start(startInfo);
        }


        static void StartFfmpeg_Old()
        {
            var startInfo = new ProcessStartInfo();
            startInfo.FileName =
                @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\ffmpeg.exe";
            startInfo.Arguments=
                //@" -v info -i SCRIPT.avs -aspect 16:9 -c:v h264_nvenc -rc constqp -preset hq OUT.mp4";
                @" -v info -i SCRIPT.avs -aspect 16:9 -c:v h264_qsv -rc constqp -preset medium OUT.mp4";
            // for intel, possible presets: veryfast, faster, fast, medium, slow, slower, veryslow  (https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/cloud-computing-quicksync-video-ffmpeg-white-paper.pdf)
            startInfo.WorkingDirectory =
                @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\";
            var env = startInfo.EnvironmentVariables;
            startInfo.CreateNoWindow = false;

            Process.Start(startInfo);
        }

        static void Test2()
        {
            var videoInfo = new SmVideoInfo()
            {
                PixelType = (int)SmPixelTypes.Bgr24,
                Width = 1920,
                Height = 1080,
                Stride = 1920 * 3,
                Fps_numerator = 25,
                Fps_denominator = 1
            };
            var smm = new SharedMemManager(videoInfo, 2, "SHAREDMEMTEST");
            smm.Initialize();

            StartFfmpeg();

            var bitmapGenerator = new TestPictureGenerator(videoInfo.Width, videoInfo.Height);

            int i=0;
            for (; ; )
            {
                bitmapGenerator.GenerateNext();

                bitmapGenerator.GetBitmap(
                    (ptr, width, height, stride) =>
                        {
                            for (; ; )
                            {
                                bool b = smm.TryAddFrame(ptr, width, height, stride);
                                if (b == true)
                                {
                                    break;
                                }

                                // I put this here hoping that this plays nicely with HyperTreading
                                // -> https://stackoverflow.com/questions/48959457/c-call-c-sharp-clr-function-spinwaitinternal
                                Thread.SpinWait(10);
                            }
                        });
                Console.WriteLine($"Put frame #{i}");
                ++i;
            }

        }


        static void Test1()
        {
            var videoInfo = new SmVideoInfo()
            {
                PixelType = (int)SmPixelTypes.Bgr24,
                Width = 1920,
                Height = 1080,
                Stride = 1920 * 3,
                Fps_numerator = 25,
                Fps_denominator = 1
            };
            var smm = new SharedMemManager(videoInfo, 2, "SHAREDMEMTEST");
            smm.Initialize();
        }

        static void Main(string[] args)
        {
            // Test1();
            Test2();
        }
    }
}
