using System;
using System.Runtime.InteropServices;

namespace FFmpegAviSynthTest.SharedMem
{
    using System.Collections.Generic;
    using System.IO.MemoryMappedFiles;
    using System.Linq;
    using System.Threading;

    public partial class SharedMemManager
    {
        private SmVideoInfo videoInfo;

        private string mapName;

        private List<uint> offsetBitmaps;

        private int sizeOfBitmap;

        private MemoryMappedFile mmf;

        private MemoryMappedViewAccessor viewAccessor;

        public SharedMemManager(SmVideoInfo videoInfo, int bufferCnt, string mapName)
        {
            this.videoInfo = videoInfo;
            this.mapName = mapName;

            int sizeOfHdr = Marshal.SizeOf<SharedMemHdr>();
            this.sizeOfBitmap = videoInfo.Height * videoInfo.Stride;

            this.offsetBitmaps = new List<uint>();
            uint offset = Align((uint)sizeOfHdr, 256);
            for (int i = 0; i < bufferCnt; ++i)
            {
                this.offsetBitmaps.Add(offset);
                offset = Align((uint)offset + (uint)this.sizeOfBitmap, 256);
            }

            //byte* ptr = (byte*)0;
            //view.SafeMemoryMappedViewHandle.AcquirePointer(ref ptr);
            //view.SafeMemoryMappedViewHandle.ReleasePointer();
        }

        public void Initialize()
        {
            long totalSize = this.offsetBitmaps.Last() + this.sizeOfBitmap;
            var mmf = MemoryMappedFile.CreateNew(this.mapName, totalSize);
            var viewAccessor = mmf.CreateViewAccessor();

            var hdr = this.CreateInitialSharedMemHdr();
            viewAccessor.Write(0, ref hdr);

            this.mmf = mmf;
            this.viewAccessor = viewAccessor;
        }

        public unsafe bool TryAddFrame(IntPtr ptr, int width, int height, int stride)
        {
            // try to lock next free frame
            byte* sharedMemHdr = null;
            this.viewAccessor.SafeMemoryMappedViewHandle.AcquirePointer(ref sharedMemHdr);

            try
            {
                ref int state = ref this.GetBufferStateAtWritePointer(sharedMemHdr);

                if (Interlocked.CompareExchange(ref state, BufferState_InUse, BufferState_Free) == BufferState_Free)
                {
                    this.CopyToBitmapAtWritePointer(sharedMemHdr, ptr, width, height, stride);
                    state = BufferState_Filled;
                    this.IncrementWritePtr(sharedMemHdr);
                    return true;
                }

                return false;
            }
            finally
            {
                this.viewAccessor.SafeMemoryMappedViewHandle.ReleasePointer();
            }
        }

        private unsafe ref int GetBufferStateAtWritePointer(byte* ptr)
        {
            SharedMemHdr* hdr = (SharedMemHdr*)ptr;
            return ref hdr->videoBufferState.bufferStates[hdr->videoBufferState.writePtr];
        }

        private unsafe  void IncrementWritePtr(byte* ptr)
        {
            SharedMemHdr* hdr = (SharedMemHdr*)ptr;
            int wp = hdr->videoBufferState.writePtr + 1;
            if (wp >= hdr->videoBufferInfo.videoBufferCount)
            {
                wp = 0;
            }

            hdr->videoBufferState.writePtr = wp;
        }

        private unsafe void CopyToBitmapAtWritePointer(byte* sharedMemHdr, IntPtr srcPtr, int width, int height, int stride)
        {
            SharedMemHdr* hdr = (SharedMemHdr*)sharedMemHdr;
            byte* ptrBitmap = sharedMemHdr + hdr->videoBufferInfo.offset[hdr->videoBufferState.writePtr];
            StridedCopy(srcPtr, stride, new IntPtr(ptrBitmap), (int)hdr->videoInfo.stride, height, (int)Math.Min(stride, hdr->videoInfo.stride));
        }

        private unsafe SharedMemHdr CreateInitialSharedMemHdr()
        {
            var hdr = new SharedMemHdr();
            hdr.magic = SharedMemMagic;
            hdr.videoInfo.pixelType = this.videoInfo.PixelType;
            hdr.videoInfo.width = (uint)this.videoInfo.Width;
            hdr.videoInfo.height = (uint)this.videoInfo.Height;
            hdr.videoInfo.stride = (uint)this.videoInfo.Stride;
            hdr.videoInfo.fps_denominator = (uint)this.videoInfo.Fps_denominator;
            hdr.videoInfo.fps_numerator = (uint)this.videoInfo.Fps_numerator;

            hdr.videoBufferInfo.videoBufferCount = this.offsetBitmaps.Count;
            for (int i = 0; i < this.offsetBitmaps.Count; ++i)
            {
                hdr.videoBufferInfo.offset[i] = this.offsetBitmaps[i];
            }

            hdr.videoBufferState.readPtr = 0;
            hdr.videoBufferState.writePtr = 0;
            for (int i = 0; i < this.offsetBitmaps.Count; ++i)
            {
                hdr.videoBufferState.bufferStates[i] = BufferState_Free;
            }

            return hdr;
        }

        private void PerformOperationOnSharedMemory(Action<IntPtr> action)
        {
            unsafe
            {
                byte* ptr = (byte*)0;
                this.viewAccessor.SafeMemoryMappedViewHandle.AcquirePointer(ref ptr);
                try
                {
                    action(new IntPtr(ptr));
                }
                finally
                {
                    this.viewAccessor.SafeMemoryMappedViewHandle.ReleasePointer();
                }
            }
        }

        private static uint Align(uint offset, uint alignment)
        {
            return ((offset + alignment - 1) / alignment) * alignment;
        }

        private static void StridedCopy(IntPtr src, int strideSrc, IntPtr dst, int strideDst, int height, int lineLength)
        {
            unsafe
            {
                if (strideSrc == strideDst)
                {
                    System.Buffer.MemoryCopy(src.ToPointer(), dst.ToPointer(), height * strideSrc, height * strideSrc);
                }
                else
                {
                    for (int i = 0; i < height; ++i)
                    {
                        System.Buffer.MemoryCopy((src + i * strideSrc).ToPointer(), (dst + i * strideDst).ToPointer(), lineLength, lineLength);
                    }
                }
            }
        }
    }

    public partial class SharedMemManager
    {
        private const int MAX_VIDEO_BUFFERS = 8;

        private const int BufferState_InUse = 1;
        private const int BufferState_Filled = 2;
        private const int BufferState_Free = 3;

        private static Guid SharedMemMagic = new Guid(0x1eb32e5e, 0x56b9, 0x466e, new byte[8] { 0x9c, 0x98, 0x27, 0xe3, 0x93, 0x3d, 0xbd, 0x23 });

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        struct SharedMemVideoInfo
        {
            public int pixelType;
            public UInt32 width;
            public UInt32 height;
            public UInt32 stride;

            public UInt32 fps_numerator;
            public UInt32 fps_denominator;
        };

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        unsafe struct VideoBufferInfo
        {
            public int videoBufferCount;
            public fixed UInt32 offset[MAX_VIDEO_BUFFERS];
        };

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        unsafe struct BufferState
        {
            public int readPtr;
            public int writePtr;

            public fixed int bufferStates[MAX_VIDEO_BUFFERS];
        };

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        struct SharedMemHdr
        {
            public Guid magic;

            public SharedMemVideoInfo videoInfo;

            public VideoBufferInfo videoBufferInfo;
            public BufferState videoBufferState;
        };
    }
}
