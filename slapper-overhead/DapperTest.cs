using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dapper;
using System.Data.SqlClient;

namespace TestingSlapper
{
     class DapperTest : TestReturnsObjectList
    {
        private SqlConnection connection;
        private string sql;

        public DapperTest ( SqlConnection _connection, string _sql )
        {
            connection = _connection;
            sql = _sql;
        }
        public override void Run()
        {
            var customersDictionary = new Dictionary<int, Customer>();
            rows = connection.Query<Customer, Order, Customer>(sql,
                (c, o) =>
                {
                    Customer customer;
                    if (!customersDictionary.TryGetValue(c.Id, out customer))
                    {
                        customersDictionary.Add(c.Id, customer = c);
                    }
                    if (customer.Orders == null)
                    {
                        customer.Orders = new List<Order>();
                    }
                    customer.Orders.Add(o);
                    return customer;
                }, splitOn: "Id").Distinct().ToList();
        }

    }
}
