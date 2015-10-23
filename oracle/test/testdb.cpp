#include <iostream>
using namespace std;
#include <stdio.h>
#include "oracledb.h"
#include "log/log.h"
#include "singleton.h"

OracleDB& gOdb = Singleton<OracleDB>::InstanceRef();

void createTab()
{
    string insertStr = "create table testdb(a varchar2(10), b number(10))";
    gOdb.ExecSql(insertStr, 1);
}
void delTab()
{
    string insertStr = "delete from testdb where b=100";
    gOdb.ExecSql(insertStr, 1);
}
void dropTab()
{
    string insertStr = "drop table testdb";
    gOdb.ExecSql(insertStr, 1);
}
void insert()
{
    string insertStr = "insert into testdb(a, b) values('str.111', 100)";
    gOdb.ExecSql(insertStr, 1);
}

void updata()
{
    string insertStr = "update testdb set a='str.222' where b>99";
    gOdb.ExecSql(insertStr, 1);
}

void selectDb()
{
    string insertStr = "select a, b from testdb where b>99";
    int isOK = 0;

    gOdb.ExecSelectSql(insertStr);
    int b;
    char a[100] = {0};
    int ret = RET_OK;
    while(ret == RET_OK)
    {
        ret = gOdb.FetchNext(a);
        ret = gOdb.FetchNext(b);
        cout << "a is:" << a << endl;
        cout << "b is:" << b << endl;
    }

}

int main ()
{
    log_init(LL_ALL, "testdb", "./log/");
    struct SOracleConnect conn;
    conn.userName_ = "user";
    conn.database_ = "test";
    conn.password_ = "123456";
    gOdb.InitData(conn);

    createTab();
    getchar();
    insert();
    getchar();
    updata();
    getchar();
    selectDb();
    getchar();
    delTab();
    getchar();
    dropTab();
    return 0;
}

