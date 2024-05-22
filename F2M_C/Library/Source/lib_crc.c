#include "lib_crc.h"


#if defined(CRC32_TYPE)

#endif

#if defined(CRC16_TYPE)
    #if (CRC16_TYPE == CRC16_IBM) || (CRC16_TYPE == CRC16_MAXIM) || (CRC16_TYPE == CRC16_USB) || (CRC16_TYPE == CRC16_MODBUS)
        u16 CRC16_Get(u8 *pdata, u32 num)
        {
            #if (CRC16_TYPE == CRC16_IBM) || (CRC16_TYPE == CRC16_MAXIM)
                u8 crcHi = 0x00, crcLo = 0x00;
            #elif (CRC16_TYPE == CRC16_USB) || (CRC16_TYPE == CRC16_MODBUS)
                u8 crcHi = 0xFF, crcLo = 0xFF;
            #endif
            
            u8 index;
            
            while(num--)
            {
                index = crcHi ^ *pdata++;
                crcHi = crcLo ^ CRC16_Hi_Tab[index];
                crcLo = CRC16_Lo_Tab[index];
            }
            
            #if (CRC16_TYPE == CRC16_IBM) || (CRC16_TYPE == CRC16_MODBUS)
//                return ((crcHi << 8) | crcLo);
                return ((crcLo << 8) | crcHi);
            #elif (CRC16_TYPE == CRC16_MAXIM) || (CRC16_TYPE == CRC16_USB)
                return (((crcHi << 8) | crcLo) ^ 0xFFFF);
            #endif
        }
    #elif ((CRC16_TYPE == CRC16_CCITT) || (CRC16_TYPE == CRC16_CCITT_FALSE) || (CRC16_TYPE == CRC16_X25) || (CRC16_TYPE == CRC16_XMODEM))
        u16 CRC16_Get(u8 *pdata, u32 num)
        {
          u16 crc = 0;
            u8  temp;
            
        #if CRC16_SEQ == CRC_RES_SEQ  //逆序运算

          while(num--)
          {
            temp = crc & 0XFF;
              crc = (crc >> 8) ^ CRC16_Tab[(temp ^ *buf++) & 0XFF];
          }
         
        #else                         //正序运算

          while(num--)
          {
            temp = crc >> 8;
              crc = (crc << 8) ^ CRC16_Tab[(temp ^*buf++) & 0XFF];
          }

        #endif

          return crc;
        }
    #elif (CRC16_TYPE == CRC16_DNP)

    #else

    #endif
#endif  /* CRC16_TYPE */

#if defined(CRC8_TYPE)
#if (CRC8_TYPE == CRC8) || (CRC8_TYPE == CRC8_ITU) || (CRC8_TYPE == CRC8_ROHC)

#elif CRC8_TYPE == CRC8_MAXIM

#else

#endif
#endif



////常见的三种CRC标准用到的各个参数表：
////              CRC_CCITT     CRC16         CRC32
////校验和位宽W   16            16            32
////生成多项式    X16+X12+X5+1  X16+X15+X2+1  X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X1+1
////除数（多项式）0X1021        0X8005        0X04C11DB7
////余数初始值    0XFFFF        0X0000        0XFFFFFFFF
////结果与或值    0X0000        0X0000        0XFFFFFFFF
// 

////static u32 CRC32_Tab_Compute(void)
////{
////  u32 tab;
////	u32 i;
////	u32 bit;
////	
////	for(i = 0; i < 256; i++)
////	{
////	  tab = (u32)i;
////		
////		for(bit = 0; bit < 8; bit++)
////		{
////			if(tab & 1)
////			{
////				tab = (tab >> 1) ^ (0XEDB88320);
////			}
////			else
////			{
////				tab = tab >> 1;
////			}
////	  }
////	
////	  CRC32_Tab[i] = tab;
////	}
////}

///*****************************************************************************
//功能：计算一串数据的CRC32校验值
//入参：*pdata, 输入数据指针
//      len, 输入数据长度
//返回：32位CRC校验值
//例程：crc = CRC32_Get(msg, 16);  //获取一个16字节数据包的crc32值
//说明：
//******************************************************************************/
//u32 CRC32_Get(u8 *pdata, u32 num)
//{
//	u32 crc = 0XFFFFFFFF;
//	u8 index;
//	
//	while(num--)
//	{
//	  index = (u8)(crc ^ *pdata++);
//		crc = (crc >> 8) ^ CRC32_Tab[index];
//	}
//	
//  return (crc ^ 0XFFFFFFFF);
//}

///*****************************************************************************
//功能：计算一串数据的CRC32校验值
//入参：*pdata, 输入数据指针
//      num, 输入数据长度
//返回：32位CRC校验值
//例程：crc = CRC32_MPEG2_Get(msg, 16);  //获取一个16字节数据包的crc32值
//说明：适用于STIM300
//******************************************************************************/
//u32 CRC32_MPEG2_Get(u8 *pdata, u32 num)
//{
//  u32 crc = 0xFFFFFFFF;
//	u32 i;
//	
//	for(i = 0; i < num; i++)
//	{
//	  crc = (crc << 8) ^ CRC32_MPEG2_Tab[((crc >> 24) ^ pdata[i]) & 0xFF];
//	}
//	
//	return crc;
//}


///*****************************************************************************
//功能：计算一串数据的CRC16校验值
//入参：*buf, 输入数据指针
//      len, 输入数据长度
//出参：无
//返回：16位CRC校验值
//例程：crc = CRC16_Get(msg, 16);  //获取一个16字节数据包的crc16值
//说明：当使用CRC16时，计算结果与CRC校验工具-V2.0一致；当使用CRC16-CCITT时，需选择
//      算法逆序，两者计算结果才会一致
//	  Ymodem标准协议使用的是CRC16-CCITT正序算法
//******************************************************************************/
//u16 CRC16_Get(u8 *buf, u32 num)
//{
//  u16 crc = 0;
//	u8  temp;
//	
//#if CRC16_SEQ == CRC_RES_SEQ  //逆序运算

//  while(num--)
//  {
//    temp = crc & 0XFF;
//	  crc = (crc >> 8) ^ CRC16_Tab[(temp ^ *buf++) & 0XFF];
//  }
// 
//#else                         //正序运算

//  while(num--)
//  {
//    temp = crc >> 8;
//	  crc = (crc << 8) ^ CRC16_Tab[(temp ^*buf++) & 0XFF];
//  }

//#endif

//  return crc;
//}

///*****************************************************************************
//功能：计算一串数据的CRC8校验值
//入参：*buf, 输入数据指针
//      len, 输入数据长度
//出参：无
//返回：8位CRC校验值
//例程：crc = CRC8_Get(msg, 16);  //获取一个16字节数据包的crc16值
//说明：CRC-8/MAXIM,多项式x8+x5+x4+1,初始值0，结果异或值0
//******************************************************************************/
//u8  CRC8_Get(u8 *buf, u32 num)
//{
//  u8 crc = 0;
//	
//	while(num--)
//	{
//	  crc = CRC8_Tab[crc ^ *buf++];
//	}
//	
//	return crc;
//}



//#include "project_config.h"
//#if SOFTWARE_TEST == ON && ALGO_TEST == ON && CRC_TEST == ON
//#include "hal_system.h"
//#include "hal_delay.h"
//#include "hal_led.h"
//#include "algo_lib.h"

//#include "smodem.h"
///*****************************************************************************
//功能：测试函数
//入参：无
//出参：无
//返回：无
//例程：Test();
//说明：在main()函数中直接调用
//******************************************************************************/ 
//void Test(void)
//{
////CRC16测试需要与CRCTool.exe配合使用，
//	
////  u8 pd1[16]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0X0A, 0X0B, 0X0C, 0X0D, 0X0E, 0X0F};
////  u8 pd2[16]={0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
////	
////	u16 crc1, crc2;
////	
////	crc1 = crc1;             //防止编译警告
////	crc2 = crc2;             
////	
////	crc1 = CRC16_Get(pd1, 16);
////	crc2 = CRC16_Get(pd2, 16);
////	
////	while(1);


////  u8 pdata[16] = {0, 0};
//	
////  u32 crc;
////	
////  crc = CRC32_Tab_Compute();


//  typedef struct
//	{
////	  u32 i;
////		u8 j;
////		u8 m;
////		u8 n;
//		u8 x;
//		u8 y;
//	}type;
//	
//	typedef struct
//	{
//		u8 n;
//	  type  m;
//	}cnt;
//	
//	typedef enum
//	{
//	  i,
//		j,
//		m = 255,
//		n,
//		p,
//	}e;
//	
//	typedef union
//	{
//	  u16 i;
//		u8 j;
//	}u;
//	
//	u8 size_type, size_cnt, size_e, size_u;
//	
//	size_type = sizeof(type);
//	size_cnt = sizeof(cnt);
//	
//	size_e = sizeof(e);
//	size_e = sizeof(Smodem_State_Enum);
//	
//	size_u = sizeof(u);
//	
//	if(size_type == size_cnt - 4  && size_e == size_u)
//	{
//	  size_type = 0, size_cnt = 0;
//	}
//  
//  while(1);
//}
//#endif



