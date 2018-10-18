using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FFmpegAviSynthTest
{
    using FFmpegAviSynthTest.SharedMem;

    class Program
    {
        static void Test1()
        {
            var videoInfo = new SmVideoInfo()
                                {
                                    PixelType = (int)SmPixelTypes.Bgr24,
                                    Width = 1920,
                                    Height = 1080,
                                    Stride = 1920*3,
                                    Fps_numerator = 25,
                                    Fps_denominator = 1
                                };
            var smm = new SharedMemManager(videoInfo,2,"SHAREDMEMTEST");
            smm.Initialize();
        }

        static void Main(string[] args)
        {
            Test1();
        }
    }
}
