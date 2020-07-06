#ifndef __EC20_H__
#define __EC20_H__

#include "sys.h"
#define     ATI					"ATI\r\n"           //��ȡģ����Ϣ
#define 	AT				    "AT\r\n"            //����ָ��
#define 	ATE0   			    "ATE0\r\n"			//�رջ���
#define     CSQ					"AT+CSQ\r\n"    	//���CSQ
#define     CPIN				"AT+CPIN?\r\n"  	//���SIM���Ƿ���λ,����ȱ�ڳ������ 
#define     CREG				"AT+CREG?\r\n"		//�鿴�Ƿ�ע��GSM����
#define     CGREG 				"AT+CGREG?\r\n"		//�鿴�Ƿ�ע��GPRS����

#define     CREG1				"AT+CREG=1\r\n"		//ע��GSM����
#define     CGREG1 				"AT+CGREG=1\r\n"	//ע��GPRS����


#define     COPS 				"AT+COPS?\r\n"		//�鿴ע�ᵽ�ĸ���Ӫ�̣�֧���ƶ� ��ͨ ���� 
#define     QICLOSE 			"AT+QICLOSE=0\r\n"	//�ر�socket����
#define     QICSGP 				"AT+QICSGP=1,1,\042CMNET\042,\042\042,\042\042,0\r\n"//����APN�����û���������
//#define     QICSGP              "AT+QICSGP=1,1,\"CMNET\","","",1\r\n"   //����APN�����û���������
#define     QICSGP_CMTDS 		"AT+QICSGP=1,1,\042CMTDS\042,\042\042,\042\042,0\r\n"//����APN�����û���������
#define     QICSGP_CHECK        "AT+QICSGP=?\r\n"
#define     QICSGP_CHECK_1      "AT+QICSGP=1\r\n"

#define     QIDEACT 			"AT+QIDEACT=1\r\n"	    //ȥ����
#define     QIACT				"AT+QIACT=1\r\n"		//����
#define     QIACT_IP			"AT+QIACT?\r\n"		    //��ȡ��ǰ����IP��ַ
#define     QIOPEN				"AT+QIOPEN=1,0,\042TCP\042,\042103.46.128.49\042,28180,0,1\r\n"	//��������Ҫ��½��IP���룬����ֱ���³�ģʽ


#define     ATE_0               "ATE0\r\n"                      //�رջ���
#define     CREG_0              "AT+CREG=0\r\n"                 //ȡ��ע��״̬�����ϱ�
#define     CMEE_N              "AT+CMEE=2\r\n"                 //���ô�������ʽ
#define     QCFG                "AT+QCFG=\"nwscanmode\",0\r\n"  //��������ģʽ  
#define     QNWINFO             "AT+QNWINFO\r\n"                //��ѯ��ǰ�������
#define     QISWTMD             "AT+QISWTMD=0,2\r\n"            //�л����䷽ʽ��<connectID>: 0-11,<access_mode> 0�����棬1��ֱ�����ͣ�2��͸��ת��



/*------------����ʮ������------------*/
#define     QISENDEX			"AT+QISENDEX=0,\042%s\042\r\n"  //ѡ��ʮ�����Ʒ�ʽ��������
#define     QISEND				"AT+QISEND=0,0\r\n"



#define 	CWMODE   			"AT+CWMODE=3\r\n"      //stationģʽ����
//#define 	CIPMODE   			"AT+CIPMODE=1\r\n"    //͸��ģʽ���ã������ڽ�������ǰ����fei͸��ģʽ (AT+CIPMODE=0)
#define   	CIPSTATUS 			"AT+CIPSTATUS\r\n"    //��ѯ��������״̬
#define 	GCIPMODE   			"AT+CIPMODE=0\r\n" 


#define     WIFI_TCP_CONNECT  	"AT+CIPSTART=\"TCP\",\"192.168.3.26\",5000\r\n"   //���ӷ�����

#define     CIPSEND     		"AT+CIPSEND=200\r\n"    //��ʼ��������
#define     CIPCLOSE    		"AT+QICLOSE=0\r\n"      //�Ͽ�tcp����
#define 	CIPMUX				"AT+CIPMUX=0\r\n"       //������ģʽ
#define 	RST         		"AT+RST\r\n"            //ģ�鸴λ

#define     REGIST 				"POST /api/v1_0/nb/zc/ HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: 033\r\n\r\n{\"ID\":\"012C00000001\",\"FV\":\"GSV1\"}\r\n"
#define     TCPCONN_ZXAQ 		"AT+QIOPEN=1,0,\"TCP\",\"106.15.228.170\",5001,0,1\r\n"         //������˾����������   http://106.15.228.170:5001/
#define		TCPCONN_HSK			"AT+QIOPEN=1,0,\"TCP\",\"103.46.128.41\",57091,0,1\r\n"         //�����Ƿ�����
#define     TCPCONN_PUBLIC      "AT+QIOPEN=1,0,\"TCP\",\"101.200.44.18\",53881,0,2\r\n"         //�Լ���Ĺ���  ͸��ģʽ
#define     TCPCONN_PUBLIC_DIR  "AT+QIOPEN=1,0,\"TCP\",\"101.200.44.18\",53881,0,1\r\n"         //�Լ���Ĺ���  ֱ������ģʽ


#define 	POST_ZC				"AT+QISENDEX=0,\"31323334\"\r\n"
#define 	POST_ET	    		"AT+QISEND=0,30,POST /api/v1_0/nb/sb/ HTTP/1.1\r\n"

#define 	CONTEXT_LENGTH		"AT+QISEND=0,19,Content-Length: 000\r\n"
#define 	CONTEXT_TYPE 		"AT+QISEND=0,30,Content-Type: application/json\r\n"

#define 	GET_XT				"AT+QISEND=0,45,GET /api/v1_0/nb/xt/?ID=012C00000000 HTTP/1.1\r\n"   //len=36
#define 	GET_TYPE			"AT+QISEND=0,21,User-Agent: CNFIRE1.0\r\n"


#define 	POST_ZC2			"AT+QISENDEX=0,\"POST /te-0/v1/heartbeatEvents HTTP/1.1User-Agent: Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)Content-Type: application/jsonHost: 192.168.3.135Content-Length: 39\"\r\n"
#define 	POST_ZC3			"AT+QISENDEX=0,\"557365722D4167656E743A204D6F7A696C6C612F342E302028636F6D70617469626C653B204D53494520352E30303B2057696E646F777320393829\"\r\n"
#define 	POST_ZC4			"AT+QISENDEX=0,\"436F6E74656E742D547970653A206170706C69636174696F6E2F6A736F6E\"\r\n"
#define 	POST_ZC5			"AT+QISENDEX=0,\"486F73743A203139322E3136382E332E313335\"\r\n"
#define 	POST_ZC6			"AT+QISENDEX=0,\"436F6E74656E742D4C656E6774683A203339\"\r\n"

#define 	POST_ET_7	    	"POST /api/v1_0/nb/sb/ HTTP/1.1\r\n"
#define     PUBLIC_GET          "GET / HTTP/1.0\r\n"


#define     DIR_OUTPU_1         "AT+QISEND=0\r\n"
#define     DIR_OUTPU_2         "/api/v1_0/nb/sb/ HTTP/1.1\r\n"

#define     DIR_OUTPU_1_1       "AT+QISEND=0,%d\r\n"
#define     DIR_OUTPU_3         "0x1A\r\n"
#define     EXIT_AUTO           "+++"

#define     CFUN                "AT+CFUN=1,1\r\n"  //����ģ��
#define     TURN_OFF            "AT+QPOWD\r\n"
#define     QISDE               "AT+QISDE=0\r\n"
#define     CIMI                "AT+CIMI\r\n"

#define     CPBS                "AT+CPBS=ON\r\n"
#define     CPBW                "AT+CPBW=1\r\n"
#define     CNUM                "AT+CNUM\r\n"
#define     QSIMDET             "AT+QSIMDET=1,1\r\n"

#define   TRY_COUNT             10  //���ó��Դ���
#define   AT_DELAY              10
#define   ATDATA_DELAY          50

#define  EC20_WAIT_TIME         1000  


void  EC20_BOOT(void);
void  EC20_Send_ATCMD(const uint8_t *data);
void  EC20_Init(void);

#endif
