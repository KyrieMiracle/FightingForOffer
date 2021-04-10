#include <stdio.h>
#include <iostream>
#include "mysql.h"
#include <string>
using namespace std;

#include <mysql.h> 如果配置ok就可以直接包含这个文件
int main(void)
{
    MYSQL mysql;    //一个数据库结构体
    MYSQL_RES* res; //一个结果集结构体
    MYSQL_ROW row;  //char** 二维数组，存放一条条记录
    //初始化数据库
    mysql_init(&mysql);
    //设置编码方式
    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");
    //连接数据库
    //判断如果连接失败就输出连接失败。
    //注意你连接的账户名密码
    if (mysql_real_connect(&mysql, "localhost", "root", "000000", "db_library", 3306, NULL, 0) == NULL)
        printf("连接失败！\\n");
    //注意你连接的账户名密码
            //注意你连接的账户名密码
                    //注意你连接的账户名密码
                            //注意你连接的账户名密码
//查询数据
    mysql_query(&mysql, "select * from t_book");
    //获取结果集
    res = mysql_store_result(&mysql);
    //显示数据
    
    //给ROW赋值，判断ROW是否为空，不为空就打印数据。
    
    while (row = mysql_fetch_row(res))
    {
        string a;
        if (row[1] != NULL) {
            cout << string(row[1]) << endl;
        }
        else {
            cout << "NULL" << endl;
        }

        //printf("t%s  ", row[1]);//打印姓名
        //printf("\\t%s  ", row[2]);//打印工资
        //printf("\\t%s  ", row[3]);//打印日期
    }
    
    //释放结果集
    mysql_free_result(res);
    //关闭数据库
    mysql_close(&mysql);
    //停留等待
    system("pause");
    return 0;
}
