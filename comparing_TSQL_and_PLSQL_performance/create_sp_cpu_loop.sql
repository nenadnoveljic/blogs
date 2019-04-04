SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[sp_cpu_loop] @iterations bigint
AS
DECLARE @cnt int = 0;
BEGIN
	SET NOCOUNT ON;
    WHILE @cnt < @iterations
	BEGIN
	   SET @cnt = @cnt + 1;
	END;
END
GO
