-- Test case for http://nenadnoveljic.com/unexpected-statistics-collection

drop table t ;
create table t ( n integer ) ;
insert into t select rownum from dual connect by level <= 100 ;

begin
  dbms_stats.gather_table_stats( 
    ownname => null, tabname => 'T' ) ;
end ;
/

alter session set nls_date_format='dd.mm.yyyy hh24:mi:ss' ;
insert into t values (1);
exec DBMS_STATS.FLUSH_DATABASE_MONITORING_INFO ;

exec dbms_session.sleep(1) ;
select * from t where n = 1 ;
exec DBMS_STATS.FLUSH_DATABASE_MONITORING_INFO ;

select last_analyzed, stale_stats from user_tab_statistics 
  where table_name = 'T' ;

--column usage timestamp must be greater than analyze time
exec dbms_session.sleep(1) ;

begin
  dbms_stats.gather_schema_stats( 
    ownname => null, options => 'gather stale',
    method_opt => 'FOR ALL COLUMNS SIZE 1'
    --method_opt => 'FOR ALL COLUMNS SIZE REPEAT'
  ) ;
end ;
/

select last_analyzed, stale_stats from user_tab_statistics 
  where table_name = 'T' ;
