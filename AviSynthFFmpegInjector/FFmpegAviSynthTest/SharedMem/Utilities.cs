using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FFmpegAviSynthTest.SharedMem
{
    public static class Utilities
    {
        public static int BytesPerPixel(SmPixelTypes pt)
        {
            switch (pt)
            {
                case SmPixelTypes.Bgr24: return 3;
                case SmPixelTypes.Bgr32: return 4;
            }

            throw new ArgumentException("Unknown pixeltype.");
        }

        public static int CalcLineSize(SmPixelTypes pt, int width)
        {
            switch (pt)
            {
                case SmPixelTypes.Bgr24: return 3*width;
                case SmPixelTypes.Bgr32: return 4 * width;
            }

            throw new ArgumentException("Unknown pixeltype.");
        }

        public static int CalcLineSize(SmVideoInfo vi)
        {
            return CalcLineSize((SmPixelTypes)vi.PixelType, vi.Width);
        }
    }
}
