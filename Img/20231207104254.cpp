
/*========================================================================*/
/*== [service名  ]:  om00b2_upd       ||  [对应VC#画面 ]:  ALL         ==*/
/*== [程序编制人 ]:  张颖             ||  [程序定稿日期]:2017-11-7 16:24:11==*/
/*== [程序修改人 ]：                  ||  [程序修改日期]:               ==*/
/*========================================================================*/
/*== [数据库表   ]： tom00                                              ==*/
/*== [调用函数   ]： 无				                                    ==*/
/*== [service功能]： 订单修改【订单释放前的内容调整】                   ==*/
/*========================================================================*/
/***** C/C++ 的标准头文件部分 *****/
// New Include
#include "stdafx.h"
#include "tom00.h"
#include "tom00modi.h" //订单变更申请表。
#include "tpmof01.h"

 
#define zero 0.00001

int f_edsetcustominfo(EIClass *bcls_rec, EIClass *bcls_ret);//动态显示数据


//订单录入_基本必输项校验。【ED54】配置的必输项。
int f_omom00_chk(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

//订单录入_可操作性校验。【合同属性】的可操作性校验。
int f_omom00_chk2(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

 

//订单接收的核心函数。
int f_omom00_rec(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
 

/*<remark>=========================================================
/// <summary>
///  订单修改【订单释放前的内容调整】
/// <para>
///    功能叙述段落
/// </para>
/// </summary>
/// <param name="bcls_rec">传入块  </param>
/// <param name="bcls_ret">返回块  </param>
/// <returns>返回：新增是否成功</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(om00b2_upd)
//-EP_SYSTEM_HEAD_END                                                  
int f_om00b2_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_om00b2_upd";                //定义函数英文名称  
	CString FunctionCname = "订单信息_订单调整";              //定义函数中文名称

	 

	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	CString sqlstr = "";

	CString  systime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString  v_userid = s.userid;
	CString  function_id = "om00b2_upd";  //自定义显示项目功能号    
	CString cjrq = "";
	CString cjrq2 = "";

	CString v_order_no = "";         //合同号
	CString v_order_modi_type = ""; //订单修改类型
	CString v_prod_dif = "";  //产品分类。
	CString v_psc = "";
	CDecimal v_cnt = 0;
	CDecimal v_cnt2 = 0;
	CString v_table_name = ""; //v_table_name



	CDecimal v_pass_wt = 0; //通过量。


	CString  c_sql_where = "  WHERE   1 = 1 "; //查询条件。
	CString  c_sql_condition = " SELECT substr(to_char(sysdate,'yyyymmddHHMMss'),6,9) as AA FROM XX t "; //获取日期后9位信息。

	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
		CTOM00 tom00(conn);
		CTOM00 tom00_chk(conn);

		CTOM00MODI tom00modi(conn);
		CTPMOF01 tpmof01(conn); 
 


		//从指定TABLE[]中获取信息。
		v_table_name = "OM00_QT";
		if (bcls_rec->Tables[v_table_name].Columns.Contains("ORDER_NO"))
			v_order_no = bcls_rec->Tables[v_table_name].Rows[0]["ORDER_NO"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[v_table_name].Columns.Contains("PROD_DIF"))
			v_prod_dif = bcls_rec->Tables[v_table_name].Rows[0]["PROD_DIF"].ToString().TrimOrBlank();

		Log::Trace("", __FUNCTION__, "in == v_order_no =[{0}] v_prod_dif =[{1}]", v_order_no, v_prod_dif);

		if (v_order_no.Trim() == "")
		{
			sprintf(s.msg, "[合同号]不能为空。");
			throw CApplicationException(-1, s.msg, FunctionEname);
		}
		if (v_prod_dif.Trim() == "")
		{
			sprintf(s.msg, "[产品分类]不允许为空。");
			throw CApplicationException(-1, s.msg, FunctionEname);
		}


		//获取前台输入数据,整表信息的获取模式
		tom00.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//根据合同号 ，获取[tpmof01]表的最新信息。
		c_sql_condition = " select * from tpmof01 where order_no = @order_no ";
		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句
		// 设置SQL中的变量
		cmd_sql.Parameters.Set("order_no", v_order_no);
		cmd_sql.ExecuteReader(); //执行读取
		if (cmd_sql.Read()) //只读取单记录，可用IF 语句。
		{
			cmd_sql.Fetch(tpmof01);
		}
		cmd_sql.Close(); //关闭游标
		tpmof01.TrimOrBlank();

		if (tpmof01.ORDER_NO.Trim() == "")
		{
			sprintf(s.msg, "合同[%s]的合同跟踪主信息不存在。[tpmof01]"
				, (const char*)v_order_no);
			throw CApplicationException(-1, s.msg, FunctionEname);
		}

		Log::Trace("", __FUNCTION__, "合同状态 =[{0}] ", tpmof01.ORDER_STATUS); 
		if (tpmof01.ORDER_STATUS.Trim() >= "19"
			)
		{
			sprintf(s.msg, "合同[%s]的当前状态[%s],说明已经订单下发，\n不允许进行[订单调整]操作。"
				, (const char*)v_order_no, (const char*)tpmof01.ORDER_STATUS);
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		//订单属性_可操作性校验。 
		doFlag = f_omom00_chk2(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}


		//订单录入_基本必输项校验。
		//========================
		doFlag = f_omom00_chk(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		 

		//tom00.Print(); //整个头文件结构打印。
		// bcls_rec 对0#BLK 中包括 TOM00,TOM00HP 等信息。
		//=================
		//订单接收_总入口函数
		doFlag = f_omom00_rec(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		} 
  


		sprintf(s.msg, "合同[%s]的当前业务处理完成。", (const char*)v_order_no);

	}



	/*捕获数据库操作异常*/
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CString str = "DB error:[" + sqlstr + "]\r\n" + ex.GetMsg();
		sprintf(s.msg, "%s,sqlCode[%d]", (const char*)str, ex.GetCode());
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex) //其他错误。
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	//此处记录一下当前service名称,用于显示前台。
	//=============
	strcpy(s.sysmsg, s.svc_name);
	s.flag = doFlag;
	return doFlag;

}


