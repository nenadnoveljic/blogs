using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.SqlClient;
using Dapper;

namespace TestingSlapper 
{
    class SlapperTest : TestReturnsObjectList { 
        private SqlConnection connection;
        private string sql;

        public SlapperTest(SqlConnection _connection, string _sql)
        {
            connection = _connection;
            sql = _sql;
        }
        public override void Run()
        {
            var customersDictionary = new Dictionary<int, Customer>();
            rows = Slapper.AutoMapper.MapDynamic<Customer>(
                connection.Query(sql)).ToList();
        }
    }
}
