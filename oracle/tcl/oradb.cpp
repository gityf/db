#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include "oracledb.h"
#include "singleton.h"
#include "tcl.h"
OracleDB *gOracleDatabase;
extern "C" {
    int Oradb_Init(Tcl_Interp *Interp);
    int Oradb_Unload(Tcl_Interp *Interp, int flags);
}

/*
* exec other sql.
*/
int Action_DBExecSql(ClientData data,
                   Tcl_Interp *interp,
                   int argc, const char *argv[])
{
    if (argc != 3) {
        Tcl_SetResult(interp,
            "Usage: Action_DBExecSql sql commit.", NULL);
        return TCL_ERROR;
    }
    if (gOracleDatabase->ExecSql(argv[1], atoi(argv[2])) == RET_ERROR) {
        Tcl_SetResult(interp, "sql exec error.", NULL);
        return TCL_ERROR;
    }
    Tcl_SetResult(interp, "OK ", TCL_VOLATILE);
    return TCL_OK;
}

/*
* exec select sql
*/
int Action_DBSelectSql(ClientData data,
                   Tcl_Interp *interp,
                   int argc, const char *argv[])
{
    if (argc != 2) {
        Tcl_SetResult(interp,
            "Usage: Action_DBSelectSql sql.", NULL);
        return TCL_ERROR;
    }
    if (gOracleDatabase->ExecSelectSql(argv[1]) == RET_ERROR) {
        Tcl_SetResult(interp, "select sql exec error.", NULL);
        return TCL_ERROR;
    }
    Tcl_SetResult(interp, "OK ", TCL_VOLATILE);
    return TCL_OK;
}

/*
* fetch result from select sql
*/
int Action_DBFetch(ClientData data,
                       Tcl_Interp *interp,
                       int argc, const char *argv[])
{
    if (argc != 3) {
        Tcl_SetResult(interp,
            "Usage: Action_DBFetch bit10 limits.", NULL);
        return TCL_ERROR;
    }
    char strItem[1024] = {0};
    int intItem;
    string retstr = " ";
    int items = strlen(argv[1]);
    int limits = atoi(argv[2]);
    if (limits == 0) {
        limits = 1;
    }
    int ret = RET_OK;
    while (limits-- > 0) {
        retstr += " {";
        for (int ii = 0; ii < items; ii++) {
            if (argv[1][ii] == '0') {
                ret = gOracleDatabase->FetchNext(intItem);
                sprintf(strItem, "%lld", intItem);
            } else {
                ret = gOracleDatabase->FetchNext(strItem);
            }
            if (ret == RET_ERROR) {
                strItem[0] = 0x00;
            }
            retstr += " {";
            retstr += strItem;
            retstr += "}";
        }
        retstr += "}";
    }
    Tcl_SetResult(interp, const_cast<char *>(retstr.c_str()), TCL_VOLATILE);
    return TCL_OK;
}

/*
* connect to db.
*/
int Action_DBConnect(ClientData data,
                   Tcl_Interp *interp,
                   int argc, const char *argv[])
{
    if (argc != 4) {
        Tcl_SetResult(interp,
            "Usage: Action_DBConnect user pass db.", NULL);
        return TCL_ERROR;
    }
    struct SOracleConnect conn;
    conn.userName_ = argv[1];
    conn.database_ = argv[3];
    conn.password_ = argv[2];
    if (gOracleDatabase->InitData(conn) == RET_ERROR) {
        Tcl_SetResult(interp,
            "connect error.", NULL);
        return TCL_ERROR;
    }
    Tcl_SetResult(interp, "OK ", TCL_VOLATILE);
    return TCL_OK;
}

/*
* connect to db.
*/
int Action_DBDisConnect(ClientData data,
                     Tcl_Interp *interp,
                     int argc, const char *argv[])
{
    gOracleDatabase->Logoff();
    Tcl_SetResult(interp, "OK ", TCL_VOLATILE);
    return TCL_OK;
}

/*
* commit the transation.
*/
int Action_DBCommit(ClientData data,
                     Tcl_Interp *interp,
                     int argc, const char *argv[])
{
    gOracleDatabase->Commit();
    Tcl_SetResult(interp, "OK ", TCL_VOLATILE);
    return TCL_OK;
}

/*
* rollback the transation.
*/
int Action_DBRollBack(ClientData data,
                     Tcl_Interp *interp,
                     int argc, const char *argv[])
{
    gOracleDatabase->Rollback();
    Tcl_SetResult(interp, "OK ", TCL_VOLATILE);
    return TCL_OK;
}

int Oradb_Init(Tcl_Interp *Interp) {
    gOracleDatabase = Singleton<OracleDB>::Instance();
    Tcl_CreateCommand(Interp, "Action_DBExecSql",    (Tcl_CmdProc *)Action_DBExecSql,    (ClientData) 0, 0);
    Tcl_CreateCommand(Interp, "Action_DBSelectSql",  (Tcl_CmdProc *)Action_DBSelectSql,  (ClientData) 0, 0);
    Tcl_CreateCommand(Interp, "Action_DBFetch",      (Tcl_CmdProc *)Action_DBFetch,      (ClientData) 0, 0);
    Tcl_CreateCommand(Interp, "Action_DBCommit",     (Tcl_CmdProc *)Action_DBCommit,     (ClientData) 0, 0);
    Tcl_CreateCommand(Interp, "Action_DBRollBack",   (Tcl_CmdProc *)Action_DBRollBack,   (ClientData) 0, 0);
    Tcl_CreateCommand(Interp, "Action_DBConnect",    (Tcl_CmdProc *)Action_DBConnect,    (ClientData) 0, 0);
    Tcl_CreateCommand(Interp, "Action_DBDisConnect", (Tcl_CmdProc *)Action_DBDisConnect, (ClientData) 0, 0);
    return TCL_OK;
}

int Oradb_Unload(Tcl_Interp *Interp, int flags)
{
    return TCL_OK;
}
