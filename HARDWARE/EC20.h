#ifndef __EC20_H__
#define __EC20_H__

#include "sys.h"
#define     ATI					"ATI\r\n"           //获取模块信息
#define 	AT				    "AT\r\n"            //测试指令
#define 	ATE0   			    "ATE0\r\n"			//关闭回显
#define     CSQ					"AT+CSQ\r\n"    	//检查CSQ
#define     CPIN				"AT+CPIN?\r\n"  	//检查SIM卡是否在位,卡的缺口朝外放置 
#define     CREG				"AT+CREG?\r\n"		//查看是否注册GSM网络
#define     CGREG 				"AT+CGREG?\r\n"		//查看是否注册GPRS网络

#define     CREG1				"AT+CREG=1\r\n"		//注册GSM网络
#define     CGREG1 				"AT+CGREG=1\r\n"	//注册GPRS网络


#define     COPS 				"AT+COPS?\r\n"		//查看注册到哪个运营商，支持移动 联通 电信 
#define     QICLOSE 			"AT+QICLOSE=0\r\n"	//关闭socket连接
#define     QICSGP 				"AT+QICSGP=1,1,\042CMNET\042,\042\042,\042\042,0\r\n"//接入APN，无用户名和密码
//#define     QICSGP              "AT+QICSGP=1,1,\"CMNET\","","",1\r\n"   //接入APN，无用户名和密码
#define     QICSGP_CMTDS 		"AT+QICSGP=1,1,\042CMTDS\042,\042\042,\042\042,0\r\n"//接入APN，无用户名和密码
#define     QICSGP_CHECK        "AT+QICSGP=?\r\n"
#define     QICSGP_CHECK_1      "AT+QICSGP=1\r\n"

#define     QIDEACT 			"AT+QIDEACT=1\r\n"	    //去激活
#define     QIACT				"AT+QIACT=1\r\n"		//激活
#define     QIACT_IP			"AT+QIACT?\r\n"		    //获取当前卡的IP地址
#define     QIOPEN				"AT+QIOPEN=1,0,\042TCP\042,\042103.46.128.49\042,28180,0,1\r\n"	//这里是需要登陆的IP号码，采用直接吐出模式


#define     ATE_0               "ATE0\r\n"                      //关闭回显
#define     CREG_0              "AT+CREG=0\r\n"                 //取消注册状态主动上报
#define     CMEE_N              "AT+CMEE=2\r\n"                 //设置错误反馈方式
#define     QCFG                "AT+QCFG=\"nwscanmode\",0\r\n"  //配置联网模式  
#define     QNWINFO             "AT+QNWINFO\r\n"                //查询当前网络参数
#define     QISWTMD             "AT+QISWTMD=0,2\r\n"            //切换传输方式，<connectID>: 0-11,<access_mode> 0：缓存，1：直接推送：2：透明转发



/*------------发送十六进制------------*/
#define     QISENDEX			"AT+QISENDEX=0,\042%s\042\r\n"  //选择十六进制方式发送数据
#define     QISEND				"AT+QISEND=0,0\r\n"



#define 	CWMODE   			"AT+CWMODE=3\r\n"      //station模式设置
//#define 	CIPMODE   			"AT+CIPMODE=1\r\n"    //透传模式设置，必须在建立连接前设置fei透传模式 (AT+CIPMODE=0)
#define   	CIPSTATUS 			"AT+CIPSTATUS\r\n"    //查询网络连接状态
#define 	GCIPMODE   			"AT+CIPMODE=0\r\n" 


#define     WIFI_TCP_CONNECT  	"AT+CIPSTART=\"TCP\",\"192.168.3.26\",5000\r\n"   //连接服务器

#define     CIPSEND     		"AT+CIPSEND=200\r\n"    //开始发送数据
#define     CIPCLOSE    		"AT+QICLOSE=0\r\n"      //断开tcp连接
#define 	CIPMUX				"AT+CIPMUX=0\r\n"       //单连接模式
#define 	RST         		"AT+RST\r\n"            //模块复位

#define     REGIST 				"POST /api/v1_0/nb/zc/ HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: 033\r\n\r\n{\"ID\":\"012C00000001\",\"FV\":\"GSV1\"}\r\n"
#define     TCPCONN_ZXAQ 		"AT+QIOPEN=1,0,\"TCP\",\"106.15.228.170\",5001,0,1\r\n"         //中消公司公网服务器   http://106.15.228.170:5001/
#define		TCPCONN_HSK			"AT+QIOPEN=1,0,\"TCP\",\"103.46.128.41\",57091,0,1\r\n"         //花生壳服务器
#define     TCPCONN_PUBLIC      "AT+QIOPEN=1,0,\"TCP\",\"101.200.44.18\",53881,0,2\r\n"         //自己搭建的公网  透传模式
#define     TCPCONN_PUBLIC_DIR  "AT+QIOPEN=1,0,\"TCP\",\"101.200.44.18\",53881,0,1\r\n"         //自己搭建的公网  直接推送模式


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

#define     CFUN                "AT+CFUN=1,1\r\n"  //重启模块
#define     TURN_OFF            "AT+QPOWD\r\n"
#define     QISDE               "AT+QISDE=0\r\n"
#define     CIMI                "AT+CIMI\r\n"

#define     CPBS                "AT+CPBS=ON\r\n"
#define     CPBW                "AT+CPBW=1\r\n"
#define     CNUM                "AT+CNUM\r\n"
#define     QSIMDET             "AT+QSIMDET=1,1\r\n"

#define   TRY_COUNT             10  //设置尝试次数
#define   AT_DELAY              10
#define   ATDATA_DELAY          50

#define  EC20_WAIT_TIME         1000  


void  EC20_BOOT(void);
void  EC20_Send_ATCMD(const uint8_t *data);
void  EC20_Init(void);

#endif
