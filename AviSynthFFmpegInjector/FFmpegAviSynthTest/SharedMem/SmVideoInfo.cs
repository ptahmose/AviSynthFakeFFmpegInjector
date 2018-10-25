namespace FFmpegAviSynthTest.SharedMem
{
    public class SmVideoInfo
    {
        public SmPixelTypes PixelType { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public int Stride { get; set; }

        public int Fps_numerator { get; set; }
        public int Fps_denominator { get; set; }
    }
}
