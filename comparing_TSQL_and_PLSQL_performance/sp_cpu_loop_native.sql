SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[sp_cpu_loop_native] @iterations bigint
with native_compilation, schemabinding
AS
begin atomic with
(transaction isolation level = snapshot, language = N'English')
DECLARE @cnt int = 0;
BEGIN
	WHILE @cnt < @iterations
	BEGIN
	   SET @cnt = @cnt + 1;
	END;
END
end
GO
