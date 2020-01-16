SELECT TOP (1000000)
id = CONVERT(INT, ROW_NUMBER() OVER (ORDER BY s1.[object_id]))
INTO t
FROM sys.all_objects AS s1 CROSS JOIN sys.all_objects AS s2
OPTION (MAXDOP 1);