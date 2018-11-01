
namespace FFmpegAviSynthLib
{
    using System;
    using System.Collections.Generic;
    using System.IO;

    public partial class IniFileWriter : IDisposable
    {
        private TextWriter tw;

        private bool hasOwnershipOfTextWriter;

        public IniFileWriter(TextWriter tw, bool takeOwnership)
        {
            this.tw = tw;
            this.hasOwnershipOfTextWriter = takeOwnership;
        }

        public void Write(IReadOnlyDictionary<string, IEnumerable<Tuple<string, string>>> entries)
        {
            foreach (var kv in entries)
            {
                this.tw.WriteLine($"[{kv.Key}]");
                foreach (var items in kv.Value)
                {
                    this.tw.WriteLine($"{items.Item1}={items.Item2}");
                }
            }
        }
    }

    public partial class IniFileWriter
    {
        #region IDisposable Support
        private bool disposedValue; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposedValue)
            {
                if (disposing)
                {
                    if (this.hasOwnershipOfTextWriter)
                    {
                        this.tw?.Dispose();
                        this.tw = null;
                    }
                }

                this.disposedValue = true;
            }
        }

        // This code added to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            this.Dispose(true);
        }
        #endregion
    }
}
