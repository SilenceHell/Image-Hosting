
/*========================================================================*/
/*== [service��  ]:  om00b2_upd       ||  [��ӦVC#���� ]:  ALL         ==*/
/*== [��������� ]:  ��ӱ             ||  [���򶨸�����]:2017-11-7 16:24:11==*/
/*== [�����޸��� ]��                  ||  [�����޸�����]:               ==*/
/*========================================================================*/
/*== [���ݿ��   ]�� tom00                                              ==*/
/*== [���ú���   ]�� ��				                                    ==*/
/*== [service����]�� �����޸ġ������ͷ�ǰ�����ݵ�����                   ==*/
/*========================================================================*/
/***** C/C++ �ı�׼ͷ�ļ����� *****/
// New Include
#include "stdafx.h"
#include "tom00.h"
#include "tom00modi.h" //������������
#include "tpmof01.h"

 
#define zero 0.00001

int f_edsetcustominfo(EIClass *bcls_rec, EIClass *bcls_ret);//��̬��ʾ����


//����¼��_����������У�顣��ED54�����õı����
int f_omom00_chk(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

//����¼��_�ɲ�����У�顣����ͬ���ԡ��Ŀɲ�����У�顣
int f_omom00_chk2(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

 

//�������յĺ��ĺ�����
int f_omom00_rec(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
 

/*<remark>=========================================================
/// <summary>
///  �����޸ġ������ͷ�ǰ�����ݵ�����
/// <para>
///    ������������
/// </para>
/// </summary>
/// <param name="bcls_rec">�����  </param>
/// <param name="bcls_ret">���ؿ�  </param>
/// <returns>���أ������Ƿ�ɹ�</returns>
===========================================================</remark>*/

// service���
BM2F_ENTERACE(om00b2_upd)
//-EP_SYSTEM_HEAD_END                                                  
int f_om00b2_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // ϵͳ��־�������ڴ���ο�ʼ������


	/****** ���庯������ ***** */
	CString FunctionEname = "f_om00b2_upd";                //���庯��Ӣ������  
	CString FunctionCname = "������Ϣ_��������";              //���庯����������

	 

	//�����ñ���
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	CString sqlstr = "";

	CString  systime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString  v_userid = s.userid;
	CString  function_id = "om00b2_upd";  //�Զ�����ʾ��Ŀ���ܺ�    
	CString cjrq = "";
	CString cjrq2 = "";

	CString v_order_no = "";         //��ͬ��
	CString v_order_modi_type = ""; //�����޸�����
	CString v_prod_dif = "";  //��Ʒ���ࡣ
	CString v_psc = "";
	CDecimal v_cnt = 0;
	CDecimal v_cnt2 = 0;
	CString v_table_name = ""; //v_table_name



	CDecimal v_pass_wt = 0; //ͨ������


	CString  c_sql_where = "  WHERE   1 = 1 "; //��ѯ������
	CString  c_sql_condition = " SELECT substr(to_char(sysdate,'yyyymmddHHMMss'),6,9) as AA FROM XX t "; //��ȡ���ں�9λ��Ϣ��

	try
	{
		CPageInfo pageInfo;

		/* ���ݿ�����ඨ�� */
		CDbCommand cmd_sql(conn); //��DB �������ӡ�

		/* ʵ���ඨ�� */
		CTOM00 tom00(conn);
		CTOM00 tom00_chk(conn);

		CTOM00MODI tom00modi(conn);
		CTPMOF01 tpmof01(conn); 
 


		//��ָ��TABLE[]�л�ȡ��Ϣ��
		v_table_name = "OM00_QT";
		if (bcls_rec->Tables[v_table_name].Columns.Contains("ORDER_NO"))
			v_order_no = bcls_rec->Tables[v_table_name].Rows[0]["ORDER_NO"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[v_table_name].Columns.Contains("PROD_DIF"))
			v_prod_dif = bcls_rec->Tables[v_table_name].Rows[0]["PROD_DIF"].ToString().TrimOrBlank();

		Log::Trace("", __FUNCTION__, "in == v_order_no =[{0}] v_prod_dif =[{1}]", v_order_no, v_prod_dif);

		if (v_order_no.Trim() == "")
		{
			sprintf(s.msg, "[��ͬ��]����Ϊ�ա�");
			throw CApplicationException(-1, s.msg, FunctionEname);
		}
		if (v_prod_dif.Trim() == "")
		{
			sprintf(s.msg, "[��Ʒ����]������Ϊ�ա�");
			throw CApplicationException(-1, s.msg, FunctionEname);
		}


		//��ȡǰ̨��������,������Ϣ�Ļ�ȡģʽ
		tom00.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//���ݺ�ͬ�� ����ȡ[tpmof01]���������Ϣ��
		c_sql_condition = " select * from tpmof01 where order_no = @order_no ";
		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);// ����ִ�е�SQL���
		// ����SQL�еı���
		cmd_sql.Parameters.Set("order_no", v_order_no);
		cmd_sql.ExecuteReader(); //ִ�ж�ȡ
		if (cmd_sql.Read()) //ֻ��ȡ����¼������IF ��䡣
		{
			cmd_sql.Fetch(tpmof01);
		}
		cmd_sql.Close(); //�ر��α�
		tpmof01.TrimOrBlank();

		if (tpmof01.ORDER_NO.Trim() == "")
		{
			sprintf(s.msg, "��ͬ[%s]�ĺ�ͬ��������Ϣ�����ڡ�[tpmof01]"
				, (const char*)v_order_no);
			throw CApplicationException(-1, s.msg, FunctionEname);
		}

		Log::Trace("", __FUNCTION__, "��ͬ״̬ =[{0}] ", tpmof01.ORDER_STATUS); 
		if (tpmof01.ORDER_STATUS.Trim() >= "19"
			)
		{
			sprintf(s.msg, "��ͬ[%s]�ĵ�ǰ״̬[%s],˵���Ѿ������·���\n���������[��������]������"
				, (const char*)v_order_no, (const char*)tpmof01.ORDER_STATUS);
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		//��������_�ɲ�����У�顣 
		doFlag = f_omom00_chk2(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}


		//����¼��_����������У�顣
		//========================
		doFlag = f_omom00_chk(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		 

		//tom00.Print(); //����ͷ�ļ��ṹ��ӡ��
		// bcls_rec ��0#BLK �а��� TOM00,TOM00HP ����Ϣ��
		//=================
		//��������_����ں���
		doFlag = f_omom00_rec(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		} 
  


		sprintf(s.msg, "��ͬ[%s]�ĵ�ǰҵ������ɡ�", (const char*)v_order_no);

	}



	/*�������ݿ�����쳣*/
	catch (CDbException& ex)  //�������ݿ�����쳣
	{
		CString str = "DB error:[" + sqlstr + "]\r\n" + ex.GetMsg();
		sprintf(s.msg, "%s,sqlCode[%d]", (const char*)str, ex.GetCode());
		doFlag = -1;                 //���ݿ��쳣ʱ����-1�����񽫱��ع�
	}
	catch (CApplicationException& ex)  //����Ӧ�ô���
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex) //��������
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	//�˴���¼һ�µ�ǰservice����,������ʾǰ̨��
	//=============
	strcpy(s.sysmsg, s.svc_name);
	s.flag = doFlag;
	return doFlag;

}


