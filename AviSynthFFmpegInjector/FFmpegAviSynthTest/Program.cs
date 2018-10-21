namespace FFmpegAviSynthTest
{
    using System;
    using System.Diagnostics;

    using FFmpegAviSynthTest.SharedMem;

    class Program
    {
        static void StartFfmpeg()
        {
            var startInfo = new ProcessStartInfo();
            startInfo.FileName =
                @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\ffmpeg.exe";
            startInfo.Arguments= @" -v info -i SCRIPT.avs -aspect 16:9 -c:v h264_nvenc -rc constqp -preset hq OUT.mp4";
            startInfo.WorkingDirectory =
                @"D:\Dev\GitHub\AviSynthFakeFFmpegInjector\AviSynthFFmpegInjector\Build\VS\x64\Debug\";
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
