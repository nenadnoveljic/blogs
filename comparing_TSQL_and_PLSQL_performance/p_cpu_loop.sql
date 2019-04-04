create or replace procedure p_cpu_loop ( p_iterations integer) is
  i integer := 0 ;
begin
  while i <= p_iterations
  loop
    i := i + 1 ;
  end loop ;
end ;
