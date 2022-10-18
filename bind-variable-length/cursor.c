#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oci.h>

static text *username = (text *) "Username";
static text *password = (text *) "Password";
  /* this dbname can be changed to point to a remote DB for demo to run with */
static text *dbname   = (text *) "DBName";

/* Define SQL statements to be used in program. */
static text *insert = (text *)"INSERT INTO emp(empno, ename, job, sal, deptno)\
  VALUES (:empno, :ename, :job, :sal, :deptno)";
static text *seldept = (text *)"SELECT dname FROM dept WHERE deptno = :1";
static text *maxemp = (text *)"SELECT NVL(MAX(empno), 0) FROM emp";
static text *selemp = (text *)"SELECT ename, job FROM emp";
static text *selt = (text *)"SELECT 1 FROM t WHERE c = :C";
static text *trace = (text *)"BEGIN sys.DBMS_MONITOR.session_trace_enable(waits=>FALSE, binds=>TRUE); end;";

static OCIEnv *envhp = NULL;
static OCIError *errhp = NULL;

static void checkerr(/*_ OCIError *errhp, sword status _*/);
static void myfflush(/*_ void _*/);
static void free_handles(/*svchp, srvhp, authp, stmthp, stmthp1, stmthp2*/);
static void logout_detach_server(/*svchp, srvhp, authp*/);
static sword finish_demo(/*svchp, srvhp, authp, stmthp, stmthp1, stmthp2 */);
int main(/*_ int argc, char *argv[] _*/);

static sword status;

static boolean logged_on = FALSE;

int main(argc, argv)
int argc;
char *argv[];
{

  sword    empno, sal, deptno;
  sword    len, len2, rv, dsize, dsize2;
  sb4      enamelen = 10;
  sb4      clen = 20;
  sb4      joblen = 9;
  sb4      deptlen = 14;
  sb2      sal_ind, job_ind;
  sb2      db_type, db2_type;
  sb1      name_buf[20], name2_buf[20];
  text     *cp, *ename, *job, *dept;
  text     *c = (text *)"AAAA";

  sb2      ind[2];                                              /* indicator */
  ub2      alen[2];                                         /* actual length */
  ub2      rlen[2];                                         /* return length */

  OCIDescribe  *dschndl1 = (OCIDescribe *) 0,
               *dschndl2 = (OCIDescribe *) 0,
               *dschndl3 = (OCIDescribe *) 0;

  OCISession *authp = (OCISession *) 0;
  OCIServer *srvhp = NULL;
  OCISvcCtx *svchp = NULL;
  OCIStmt   *inserthp = NULL,
            *stmthp = NULL,
            *stmthp1= NULL;
  OCIDefine *defnp = (OCIDefine *) 0;

  OCIBind  *bnd1p = (OCIBind *) 0;             /* the first bind handle */
  OCIBind  *bnd2p = (OCIBind *) 0;             /* the second bind handle */
  OCIBind  *bnd3p = (OCIBind *) 0;             /* the third bind handle */
  OCIBind  *bnd4p = (OCIBind *) 0;             /* the fourth bind handle */
  OCIBind  *bnd5p = (OCIBind *) 0;             /* the fifth bind handle */
  OCIBind  *bnd6p = (OCIBind *) 0;             /* the sixth bind handle */

  sword errcode = 0;

  errcode = OCIEnvCreate((OCIEnv **) &envhp, (ub4) OCI_DEFAULT,
                  (dvoid *) 0, (dvoid * (*)(dvoid *,size_t)) 0,
                  (dvoid * (*)(dvoid *, dvoid *, size_t)) 0,
                  (void (*)(dvoid *, dvoid *)) 0, (size_t) 0, (dvoid **) 0);

  if (errcode != 0) {
    (void) printf("OCIEnvCreate failed with errcode = %d.\n", errcode);
    exit(1);
  }

  (void) OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &errhp, OCI_HTYPE_ERROR,
                   (size_t) 0, (dvoid **) 0);

  /* server contexts */
  (void) OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &srvhp, OCI_HTYPE_SERVER,
                   (size_t) 0, (dvoid **) 0);

  (void) OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &svchp, OCI_HTYPE_SVCCTX,
                   (size_t) 0, (dvoid **) 0);

  checkerr(errhp, OCIServerAttach ( srvhp, errhp, (text *)dbname,
                                    (sb4) strlen((char *)dbname), 0));

  /* set attribute server context in the service context */
  (void) OCIAttrSet( (dvoid *) svchp, OCI_HTYPE_SVCCTX, (dvoid *)srvhp,
                     (ub4) 0, OCI_ATTR_SERVER, (OCIError *) errhp);

  (void) OCIHandleAlloc((dvoid *) envhp, (dvoid **)&authp,
                        (ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0);

  (void) OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
                 (dvoid *) username, (ub4) strlen((char *)username),
                 (ub4) OCI_ATTR_USERNAME, errhp);

  (void) OCIAttrSet((dvoid *) authp, (ub4) OCI_HTYPE_SESSION,
                 (dvoid *) password, (ub4) strlen((char *)password),
                 (ub4) OCI_ATTR_PASSWORD, errhp);

  checkerr(errhp, OCISessionBegin ( svchp,  errhp, authp, OCI_CRED_RDBMS,
                          (ub4) OCI_DEFAULT));

  (void) OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,
                   (dvoid *) authp, (ub4) 0,
                   (ub4) OCI_ATTR_SESSION, errhp);

  logged_on = TRUE;
  
  checkerr(errhp, OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &stmthp,
           OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0));

  checkerr(errhp, OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &stmthp1,
           OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0));

  checkerr(errhp, OCIStmtPrepare(stmthp1, errhp, selt,
                                (ub4) strlen((char *) selt),
                                (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

  checkerr(errhp, OCIStmtPrepare(stmthp, errhp, trace,
                                (ub4) strlen((char *) trace),
                                (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

  if ((status = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1, (ub4) 0,
           (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT))
      && (status != OCI_NO_DATA))
  {
    checkerr(errhp, status);
    finish_demo(svchp, srvhp, authp,stmthp, stmthp1,inserthp);
    return OCI_ERROR;
  }

  /* bind the input variable */
  checkerr(errhp, OCIDefineByPos(stmthp1, &defnp, errhp, 1, (dvoid *) &empno,
                   (sword) sizeof(sword), SQLT_INT, (dvoid *) 0, (ub2 *)0,
                   (ub2 *)0, OCI_DEFAULT));

  if ((status = OCIBindByName(stmthp1, &bnd1p, errhp, (text *) ":C",
             -1, (dvoid *) c,
             90, SQLT_STR, (dvoid *) 0,
             (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT)) )
  {
    checkerr(errhp, status);
    finish_demo(svchp, srvhp, authp,stmthp, stmthp1,inserthp);
    return OCI_ERROR;
  }

  if ((status = OCIStmtExecute(svchp, stmthp1, errhp, (ub4) 1, (ub4) 0,
           (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT))
      && (status != OCI_NO_DATA))
  {
    checkerr(errhp, status);
    finish_demo(svchp, srvhp, authp,stmthp, stmthp1,inserthp);
    return OCI_ERROR;
  }

}


void checkerr(errhp, status)
OCIError *errhp;
sword status;
{
  text errbuf[512];
  sb4 errcode = 0;

  switch (status)
  {
  case OCI_SUCCESS:
    break;
  case OCI_SUCCESS_WITH_INFO:
    (void) printf("Error - OCI_SUCCESS_WITH_INFO\n");
    break;
  case OCI_NEED_DATA:
    (void) printf("Error - OCI_NEED_DATA\n");
    break;
  case OCI_NO_DATA:
    (void) printf("Error - OCI_NODATA\n");
    break;
  case OCI_ERROR:
    (void) OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,
                        errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
    (void) printf("Error - %.*s\n", 512, errbuf);
    break;
  case OCI_INVALID_HANDLE:
    (void) printf("Error - OCI_INVALID_HANDLE\n");
    break;
  case OCI_STILL_EXECUTING:
    (void) printf("Error - OCI_STILL_EXECUTE\n");
    break;
  case OCI_CONTINUE:
    (void) printf("Error - OCI_CONTINUE\n");
    break;
  default:
    break;
  }
}

void myfflush()
{
  eb1 buf[50];

  fgets((char *) buf, 50, stdin);
}

/* ----------------------------------------------------------------- */
/*  Free the specified handles                                       */
/* ----------------------------------------------------------------- */
void free_handles(svchp, srvhp, authp, stmthp, stmthp1, stmthp2)
OCISvcCtx *svchp; 
OCIServer *srvhp; 
OCISession *authp; 
OCIStmt *stmthp;
OCIStmt *stmthp1;
OCIStmt *stmthp2;
{
  if (srvhp)
    (void) OCIHandleFree((dvoid *) srvhp, (ub4) OCI_HTYPE_SERVER);
  if (svchp)
    (void) OCIHandleFree((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX);
  if (errhp)
    (void) OCIHandleFree((dvoid *) errhp, (ub4) OCI_HTYPE_ERROR);
  if (authp)
    (void) OCIHandleFree((dvoid *) authp, (ub4) OCI_HTYPE_SESSION);
  if (stmthp)
    (void) OCIHandleFree((dvoid *) stmthp, (ub4) OCI_HTYPE_STMT);
  if (stmthp1)
    (void) OCIHandleFree((dvoid *) stmthp1, (ub4) OCI_HTYPE_STMT);
  if (stmthp2)
    (void) OCIHandleFree((dvoid *) stmthp2, (ub4) OCI_HTYPE_STMT);
    
  if (envhp)
    (void) OCIHandleFree((dvoid *) envhp, (ub4) OCI_HTYPE_ENV);

  return;
}

/*-------------------------------------------------------------------*/ 
/* Logout and detach from the server                                 */
/*-------------------------------------------------------------------*/ 
void logout_detach_server(svchp, srvhp, authp)
OCISvcCtx *svchp; 
OCIServer *srvhp; 
OCISession *authp; 
{
  if ((status = OCISessionEnd(svchp, errhp, authp, (ub4) 0)))
  {
    printf("FAILED: OCISessionEnd()\n");
    checkerr(errhp, status);
  }

  if ((status = OCIServerDetach(srvhp, errhp, (ub4) OCI_DEFAULT)))
  {
    printf("FAILED: OCIServerDetach()\n");
    checkerr(errhp, status);
  }

  return;
}

/*---------------------------------------------------------------------*/
/* Finish demo and clean up                                            */
/*---------------------------------------------------------------------*/
sword finish_demo(svchp, srvhp, authp, stmthp, stmthp1, stmthp2)
OCISvcCtx *svchp; 
OCIServer *srvhp; 
OCISession *authp; 
OCIStmt *stmthp;
OCIStmt *stmthp1;
OCIStmt *stmthp2;
{

  if (logged_on)
    logout_detach_server(svchp, srvhp, authp);

  free_handles(svchp, srvhp, authp, stmthp, stmthp1, stmthp2);

  OCITerminate(OCI_DEFAULT);

  return OCI_SUCCESS;
}

/* end of file cdemo81.c */ 

