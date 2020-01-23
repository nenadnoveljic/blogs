create table customers (
	id integer, name varchar(10), 
	constraint pk_customers primary key clustered (id asc)
);

insert into customers
SELECT TOP (100000)
  id = CONVERT(INT, ROW_NUMBER() OVER (ORDER BY s1.[object_id])),
  name = 'A' + CONVERT(varchar(20),ROW_NUMBER() OVER (ORDER BY s1.[object_id]))
  FROM sys.all_objects AS s1 CROSS JOIN sys.all_objects AS s2
  OPTION (MAXDOP 1);

create table orders (
	id integer, amount integer, customer_id int
	constraint pk_orders primary key clustered (id asc)
);

insert into orders 
SELECT TOP (1000000)
  CONVERT(INT, ROW_NUMBER() OVER (ORDER BY s1.[object_id])),
  CONVERT(INT, ROW_NUMBER() OVER (ORDER BY s1.[object_id])),
  ( CONVERT(INT, ROW_NUMBER() OVER (ORDER BY s1.[object_id])) - 1 )/ 10 + 1
  FROM sys.all_objects AS s1 CROSS JOIN sys.all_objects AS s2
  OPTION (MAXDOP 1);

CREATE NONCLUSTERED INDEX [ix_orders_1]
  ON [dbo].[orders] ([customer_id])
  INCLUDE ([id],[amount]);

