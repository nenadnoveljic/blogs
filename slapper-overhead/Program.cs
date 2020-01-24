/*
 * Author: Nenad Noveljic
 * 
 * Testing Slapper.AutoMapper overhead
 * see https://nenadnoveljic.com/blog/orm-overhead
 *
 * Version: 2.0
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
using System.Diagnostics;

namespace TestingSlapper
{
    class Program
    {
        private static SqlConnection connection;
        private static System.Collections.Specialized.NameValueCollection appSettings;

        static void Main(string[] args)
        {
            appSettings = ConfigurationManager.AppSettings;
            int iterations = Int32.Parse(appSettings["Iterations"]);
            int rows_n = Int32.Parse(appSettings["Rows"]);
            int threads = Int32.Parse(appSettings["Threads"]);

            string connectionString 
                = ConfigurationManager.ConnectionStrings["TestDB"].ConnectionString;

            connection = new SqlConnection(connectionString);
            connection.Open();

            Console.Write("\nRows: {0}\n", rows_n);
            Console.Write("Iterations: {0}\n", iterations);

            if ( threads == 1)
            {
                ExecuteTests();
            } else
            {
                for ( int i = 0; i < threads ; i++)
                {
                    ThreadStart childref = new ThreadStart(ExecuteTests);
                    Thread childThread = new Thread(childref);
                    childThread.Start();
                }
            }
        }

        static void ExecuteTests()
        {
            int iterations = Int32.Parse(appSettings["Iterations"]);
            int rows_n = Int32.Parse(appSettings["Rows"]);
            int verbosity = Int32.Parse(appSettings["Verbosity"]);
            string[] tests = appSettings["tests"].Split(',');

            Dictionary<string, long[]> times = new Dictionary<string, long[]>();           

            for (int i = 0; i < iterations; i++)
            {
                foreach (var test in tests)
                {
                    string sql;
                    if (test.Equals("DapperTest"))
                    {
                        /* 
                         * I couldn't get Dapper working with the aliases in the 
                         * query defined according to Slapper.AutoMapper 
                         * convention). In particular, Order 
                         * wasn't properly mapped. 
                         * This is just a quick'n'dirty fix.
                        */
                        sql
                            = "select c.id, c.name,o.id ,o.amount "
                            + "from customers c inner join orders o "
                            + "on c.id = o.customer_id where c.id <= "
                            + rows_n;

                    }
                    else
                    {
                        sql
                            = "select c.Id as Id, "
                                + "name as Name, o.Id as Orders_Id, "
                                + "amount as Orders_Amount "
                            + " from customers c inner join orders o on "
                                + " c.id = o.customer_id where c.id <= "
                                    + rows_n;
                    }
                    var obj =
                        (ITest)Activator.CreateInstance(
                            Type.GetType("TestingSlapper." + test),
                            connection, sql
                        );


                    var watch = System.Diagnostics.Stopwatch.StartNew();
                    obj.Run();
                    watch.Stop();
                    var elapsedMs = watch.ElapsedMilliseconds;

                    if (i == 0)
                    {
                        times[test] = new long[iterations];
                    } 
                    times[test][i] = elapsedMs;

                    if ( verbosity >= 1)
                    {
                        Console.WriteLine(test + " elapsed [ms]: " + elapsedMs);
                    }

                    if (verbosity >=2 )
                    {
                        obj.PrintResults();
                    }
                }
            }
            Console.WriteLine("\nMedians [ms]:");
            foreach (KeyValuePair<string, long[]> time in times)
            {
                Console.WriteLine("{0}: {1}", time.Key, GetMedian(time.Value));
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
    }
    public class Customer
    {
        public int Id;
        public String Name;
        public List<Order> Orders;
    }

    public class Order
    {
        public int Id;
        public int Amount ;
        public int CustomerId;
    }    
}
