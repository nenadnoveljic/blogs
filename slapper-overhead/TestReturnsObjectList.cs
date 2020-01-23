using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestingSlapper 
{
    abstract class TestReturnsObjectList : ITest
    {
        public List<Customer> rows;
        public abstract void Run();
        public void PrintResults()
        {
            for (int i = 0; i < 3; i++)
            {
                Console.Write("{0} {1}\n", rows[i].Id, rows[i].Name);
                for (int j = 0; j < 3; j++)
                {
                    Console.Write(" {0} {1}\n", 
                        rows[i].Orders[j].Id, rows[i].Orders[j].Amount);
                }
            }
            Console.Write("Fetched customer rows {0}: \n", rows.Count());
        }
    }
}
