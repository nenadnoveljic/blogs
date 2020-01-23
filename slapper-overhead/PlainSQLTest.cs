using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.SqlClient;

namespace TestingSlapper
{
    class PlainSQLTest : ITest
    {
        private SqlConnection connection;
        private string sql;
        public PlainSQLTest(SqlConnection _connection, string _sql)
        {
            connection = _connection;
            sql = _sql;
        }

        private List<NonNormalizedCustomer> rows ;
        public void Run()
        {
            SqlCommand command = new SqlCommand(sql, connection);
            rows = new List<NonNormalizedCustomer>();
            using (var reader = command.ExecuteReader())
            {
                while (reader.Read())
                {
                    var customer = new NonNormalizedCustomer();
                    customer.Id = (int)reader.GetValue(0);
                    customer.Name = (string)reader.GetValue(1);
                    customer.OrderId = (int)reader.GetValue(2);
                    customer.Amount = (int)reader.GetValue(3);
                    rows.Add(customer);
                }
            }
        }
        public void PrintResults()
        {
            for ( int i=0; i < 3; i++ )
            {
                Console.Write("{0} {1} {2} {3} \n", rows[i].Id, rows[i].Name, rows[i].OrderId, rows[i].Amount);
            }
            Console.Write("Fetched rows {0}: \n", rows.Count());
        }
    }

    class NonNormalizedCustomer
    {
        public int Id;
        public String Name;
        public int OrderId;
        public int Amount;
    }
}
