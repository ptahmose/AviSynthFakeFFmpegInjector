
namespace FFmpegAviSynthLibTests
{
    using FFmpegAviSynthLib;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using System;
    using System.Collections.Generic;
    using System.IO;

    [TestClass]
    public class IniFileWriterTests
    {
        [TestMethod]
        public void IniFileWriterSimpleTest1()
        {
            var dict = new Dictionary<string, IEnumerable<Tuple<string, string>>>();
            dict.Add(
                "Test1",
                new List<Tuple<string, string>>()
                    {
                        Tuple.Create("Item1", "value1"),
                        Tuple.Create("Item2", "value2")
                    });

            string str;
            var tw = new StringWriter();
            using (var iniFw = new IniFileWriter(tw, true))
            {
                iniFw.Write(dict);
                str = tw.ToString();
            }

            const string ExpectedResult = @"[Test1]
Item1=value1
Item2=value2
";

            Assert.IsTrue(ExpectedResult == str, "Not the expected result.");
        }

        [TestMethod]
        public void IniFileWriterSimpleTest2()
        {
            var dict = new Dictionary<string, IEnumerable<Tuple<string, string>>>();
            dict.Add(
                "Test1",
                new List<Tuple<string, string>>()
                    {
                        Tuple.Create("ItemA1", "value1"),
                        Tuple.Create("ItemA2", "value2")
                    });
            dict.Add(
                "Test2",
                new List<Tuple<string, string>>()
                    {
                        Tuple.Create("ItemB1", "value1"),
                        Tuple.Create("ItemB2", "value2")
                    });

            string str;
            var tw = new StringWriter();
            using (var iniFw = new IniFileWriter(tw, true))
            {
                iniFw.Write(dict);
                str = tw.ToString();
            }

            const string ExpectedResult = @"[Test1]
ItemA1=value1
ItemA2=value2
[Test2]
ItemB1=value1
ItemB2=value2
";

            Assert.IsTrue(ExpectedResult == str, "Not the expected result.");
        }
    }
}
