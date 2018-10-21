using System;
using System.Drawing;

namespace FFmpegAviSynthTest
{
    using System.Drawing.Imaging;

    class TestPictureGenerator
    {
        private int width, height;

        private Bitmap bitmap;

        private int count;

        public TestPictureGenerator(int width, int height)
        {
            this.width = width;
            this.height = height;

            this.bitmap = new Bitmap(width, height, PixelFormat.Format24bppRgb);
        }

        public void GenerateNext()
        {
            using (Graphics g = Graphics.FromImage(this.bitmap))
            {
                g.Clear(Color.Chartreuse);

                Font f = new Font("Arial", 10.0f, FontStyle.Bold, GraphicsUnit.Point);
                g.DrawString($"#{this.count}", f, Brushes.Black, 100, 10);
            }
        }

        public void GetBitmap(Action<IntPtr, int, int, int> action)
        {
            Rectangle rect = new Rectangle(0, 0, this.bitmap.Width, this.bitmap.Height);
            var bmData = this.bitmap.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

            action(bmData.Scan0, bmData.Width, bmData.Height, bmData.Stride);

            this.bitmap.UnlockBits(bmData);
        }

    }
}
