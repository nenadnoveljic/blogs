/*
 * Author: Nenad Noveljic
 * 
 * Testing Slapper overhead
 * 
 * Version: 1.0
 * 
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.SqlClient;
using System.Configuration;
using System.Threading;
using Dapper;
using System.Reflection;

namespace TestingSlapper
{
    class Program
    {
        private static SqlConnection connection;
        private static string sql ;
        private static System.Collections.Specialized.NameValueCollection appSettings;
        private static int verbosity ;

        private delegate List<AppObj> TestFunction();

        static void Main(string[] args)
        {
            appSettings = ConfigurationManager.AppSettings;
            int iterations = Int32.Parse(appSettings["Iterations"]);
            int rows = Int32.Parse(appSettings["Rows"]);
            int threads = Int32.Parse(appSettings["Threads"]);
            verbosity = Int32.Parse(appSettings["Verbosity"]);

            /*
            sql
                = "select top " + rows
                + " biSeqNo as n from [AMSDB_ES1].[dbo].[testtab]" 
            ;
            */
            sql = "select top " + rows + " n from t" ;

            string connectionString 
                = ConfigurationManager.ConnectionStrings["TestDB"].ConnectionString;

            connection = new SqlConnection(connectionString);
            connection.Open();

            for ( int i = 0; i < threads ; i++)
            {
                ThreadStart childref = new ThreadStart(ExecuteTests);
                Thread childThread = new Thread(childref);
                childThread.Start();
            }

            Console.Write("\nRows: {0}\n", rows);
            Console.Write("Iterations: {0}\n", iterations);
        }
        static List<AppObj> ExecuteDapper()
        {
            var rows = new List<AppObj>();
            using ( var reader = connection.ExecuteReader(sql))
            {
                var appObj = reader.GetRowParser<AppObj>();
                while (reader.Read())
                {
                    rows.Add(appObj(reader));
                }
            }
            return rows;
        }
        static List<AppObj> ExecutePlainSQL ()
        {
            SqlCommand command = new SqlCommand(sql, connection);
            var rows = new List<AppObj>();
            using (var reader = command.ExecuteReader())
            {
                while (reader.Read())
                {
                    var appObj = new AppObj();
                    appObj.n = (int)reader.GetValue(0);
                    rows.Add(appObj);
                }
            }
            return rows;
        }
        static List<AppObj> ExecuteSlapper()
        {
            var rows 
                = Slapper.AutoMapper.MapDynamic<AppObj>(connection.Query(sql)).ToList();
            return rows;
        }
        static void ExecuteTests()
        {
            int iterations = Int32.Parse(appSettings["Iterations"]);

            Dictionary<string, TestFunction> functions = new Dictionary<string, TestFunction>();
            functions.Add("ExecutePlainSQL", ExecutePlainSQL);
            functions.Add("ExecuteDapper", ExecuteDapper);
            functions.Add("ExecuteSlapper", ExecuteSlapper);

            Dictionary<string, long[]> times = new Dictionary<string, long[]>();

            for (int i = 0; i < iterations; i++)
            {
                //                Dictionary<string, Delegate> dict = new Dictionary<string, Delegate>();

                foreach (KeyValuePair<string, TestFunction> function in functions)
                {
                    string name = function.Key;
                    if ( ! Convert.ToBoolean(ConfigurationManager.AppSettings[name]) )
                    {
                        continue;
                    }
                    
                    var watch = System.Diagnostics.Stopwatch.StartNew();
                    var rows = function.Value();
                    watch.Stop();
                    var elapsedMs = watch.ElapsedMilliseconds;

                    if (i == 0)
                    {
                        times[function.Key] = new long[iterations];
                    } 
                    times[name][i] = elapsedMs;

                    if ( verbosity >= 1)
                    {
                        Console.WriteLine(function.Key + " Elapsed [ms]: " + elapsedMs);
                    }
                    if (verbosity >=2 )
                    {
                        PrintFirstRows(rows);
                    }
                }
            }
            Console.WriteLine("\nMedians [ms]:");
            foreach (KeyValuePair<string, long[]> function in times)
            {
                Console.WriteLine("{0}: {1}", function.Key, GetMedian(function.Value));
            }

        }

        // GetMedian from https://stackoverflow.com/questions/4140719/calculate-median-in-c-sharp/22702269
        public static long GetMedian(long[] sourceNumbers)
        {
            //Framework 2.0 version of this method. there is an easier way in F4        
            if (sourceNumbers == null || sourceNumbers.Length == 0)
                throw new System.Exception("Median of empty array not defined.");

            //make sure the list is sorted, but use a new array
            long[] sortedPNumbers = (long[])sourceNumbers.Clone();
            Array.Sort(sortedPNumbers);

            //get the median
            int size = sortedPNumbers.Length;
            int mid = size / 2;
            long median = (size % 2 != 0) ? (long)sortedPNumbers[mid] : ((long)sortedPNumbers[mid] + (long)sortedPNumbers[mid - 1]) / 2;
            return median;
        }
        static void PrintFirstRows (List<AppObj> rows)
        {
            for (int j = 0; j < 3; j++)
            {
                Console.Write("{0}\n", rows[j].n);
            }
        }

    }
    public class AppObj
    {
        public int n { get; set; }
    }
}
