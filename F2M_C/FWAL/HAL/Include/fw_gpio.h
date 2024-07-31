#ifndef _FW_GPIO_H_
#define _FW_GPIO_H_

#ifdef __cplusplus
extern "{"
#endif


#include "fw_device.h"


#define PIN_NULL   (0x0000)
#define PORT_NULL  (0x0000)

#define PA0        (0x0100)
#define PA1        (0x0101)
#define PA2        (0x0102)
#define PA3        (0x0103)
#define PA4        (0x0104)
#define PA5        (0x0105)
#define PA6        (0x0106)
#define PA7        (0x0107)
#define PA8        (0x0108)
#define PA9        (0x0109)
#define PA10       (0x010A)
#define PA11       (0x010B)
#define PA12       (0x010C)
#define PA13       (0x010D)
#define PA14       (0x010E)
#define PA15       (0x010F)
#define PA16       (0x0110)
#define PA17       (0x0111)
#define PA18       (0x0112)
#define PA19       (0x0113)
#define PA20       (0x0114)
#define PA21       (0x0115)
#define PA22       (0x0116)
#define PA23       (0x0117)
#define PA24       (0x0118)
#define PA25       (0x0119)
#define PA26       (0x011A)
#define PA27       (0x011B)
#define PA28       (0x011C)
#define PA29       (0x011D)
#define PA30       (0x011E)
#define PA31       (0x011F)
#define PORTA      (0x01FF)

#define PB0        (0x0200)
#define PB1        (0x0201)
#define PB2        (0x0202)
#define PB3        (0x0203)
#define PB4        (0x0204)
#define PB5        (0x0205)
#define PB6        (0x0206)
#define PB7        (0x0207)
#define PB8        (0x0208)
#define PB9        (0x0209)
#define PB10       (0x020A)
#define PB11       (0x020B)
#define PB12       (0x020C)
#define PB13       (0x020D)
#define PB14       (0x020E)
#define PB15       (0x020F)
#define PB16       (0x0210)
#define PB17       (0x0211)
#define PB18       (0x0212)
#define PB19       (0x0213)
#define PB20       (0x0214)
#define PB21       (0x0215)
#define PB22       (0x0216)
#define PB23       (0x0217)
#define PB24       (0x0218)
#define PB25       (0x0219)
#define PB26       (0x021A)
#define PB27       (0x021B)
#define PB28       (0x021C)
#define PB29       (0x021D)
#define PB30       (0x021E)
#define PB31       (0x021F)
#define PORTB      (0x02FF)

#define PC0        (0x0300)
#define PC1        (0x0301)
#define PC2        (0x0302)
#define PC3        (0x0303)
#define PC4        (0x0304)
#define PC5        (0x0305)
#define PC6        (0x0306)
#define PC7        (0x0307)
#define PC8        (0x0308)
#define PC9        (0x0309)
#define PC10       (0x030A)
#define PC11       (0x030B)
#define PC12       (0x030C)
#define PC13       (0x030D)
#define PC14       (0x030E)
#define PC15       (0x030F)
#define PC16       (0x0310)
#define PC17       (0x0311)
#define PC18       (0x0312)
#define PC19       (0x0313)
#define PC20       (0x0314)
#define PC21       (0x0315)
#define PC22       (0x0316)
#define PC23       (0x0317)
#define PC24       (0x0318)
#define PC25       (0x0319)
#define PC26       (0x031A)
#define PC27       (0x031B)
#define PC28       (0x031C)
#define PC29       (0x031D)
#define PC30       (0x031E)
#define PC31       (0x031F)
#define PORTC      (0x03FF)

#define PD0        (0x0400)
#define PD1        (0x0401)
#define PD2        (0x0402)
#define PD3        (0x0403)
#define PD4        (0x0404)
#define PD5        (0x0405)
#define PD6        (0x0406)
#define PD7        (0x0407)
#define PD8        (0x0408)
#define PD9        (0x0409)
#define PD10       (0x040A)
#define PD11       (0x040B)
#define PD12       (0x040C)
#define PD13       (0x040D)
#define PD14       (0x040E)
#define PD15       (0x040F)
#define PD16       (0x0410)
#define PD17       (0x0411)
#define PD18       (0x0412)
#define PD19       (0x0413)
#define PD20       (0x0414)
#define PD21       (0x0415)
#define PD22       (0x0416)
#define PD23       (0x0417)
#define PD24       (0x0418)
#define PD25       (0x0419)
#define PD26       (0x041A)
#define PD27       (0x041B)
#define PD28       (0x041C)
#define PD29       (0x041D)
#define PD30       (0x041E)
#define PD31       (0x041F)
#define PORTD      (0x04FF)    

#define PE0        (0x0500)
#define PE1        (0x0501)
#define PE2        (0x0502)
#define PE3        (0x0503)
#define PE4        (0x0504)
#define PE5        (0x0505)
#define PE6        (0x0506)
#define PE7        (0x0507)
#define PE8        (0x0508)
#define PE9        (0x0509)
#define PE10       (0x050A)
#define PE11       (0x050B)
#define PE12       (0x050C)
#define PE13       (0x050D)
#define PE14       (0x050E)
#define PE15       (0x050F)
#define PE16       (0x0510)
#define PE17       (0x0511)
#define PE18       (0x0512)
#define PE19       (0x0513)
#define PE20       (0x0514)
#define PE21       (0x0515)
#define PE22       (0x0516)
#define PE23       (0x0517)
#define PE24       (0x0518)
#define PE25       (0x0519)
#define PE26       (0x051A)
#define PE27       (0x051B)
#define PE28       (0x051C)
#define PE29       (0x051D)
#define PE30       (0x051E)
#define PE31       (0x051F)
#define PORTE      (0x05FF)

#define PF0        (0x0600)
#define PF1        (0x0601)
#define PF2        (0x0602)
#define PF3        (0x0603)
#define PF4        (0x0604)
#define PF5        (0x0605)
#define PF6        (0x0606)
#define PF7        (0x0607)
#define PF8        (0x0608)
#define PF9        (0x0609)
#define PF10       (0x060A)
#define PF11       (0x060B)
#define PF12       (0x060C)
#define PF13       (0x060D)
#define PF14       (0x060E)
#define PF15       (0x060F)
#define PF16       (0x0610)
#define PF17       (0x0611)
#define PF18       (0x0612)
#define PF19       (0x0613)
#define PF20       (0x0614)
#define PF21       (0x0615)
#define PF22       (0x0616)
#define PF23       (0x0617)
#define PF24       (0x0618)
#define PF25       (0x0619)
#define PF26       (0x061A)
#define PF27       (0x061B)
#define PF28       (0x061C)
#define PF29       (0x061D)
#define PF30       (0x061E)
#define PF31       (0x061F)
#define PORTF      (0x06FF)

#define PG0        (0x0700)
#define PG1        (0x0701)
#define PG2        (0x0702)
#define PG3        (0x0703)
#define PG4        (0x0704)
#define PG5        (0x0705)
#define PG6        (0x0706)
#define PG7        (0x0707)
#define PG8        (0x0708)
#define PG9        (0x0709)
#define PG10       (0x070A)
#define PG11       (0x070B)
#define PG12       (0x070C)
#define PG13       (0x070D)
#define PG14       (0x070E)
#define PG15       (0x070F)
#define PG16       (0x0710)
#define PG17       (0x0711)
#define PG18       (0x0712)
#define PG19       (0x0713)
#define PG20       (0x0714)
#define PG21       (0x0715)
#define PG22       (0x0716)
#define PG23       (0x0717)
#define PG24       (0x0718)
#define PG25       (0x0719)
#define PG26       (0x071A)
#define PG27       (0x071B)
#define PG28       (0x071C)
#define PG29       (0x071D)
#define PG30       (0x071E)
#define PG31       (0x071F)
#define PORTG      (0x07FF)

#define PH0        (0x0800)
#define PH1        (0x0801)
#define PH2        (0x0802)
#define PH3        (0x0803)
#define PH4        (0x0804)
#define PH5        (0x0805)
#define PH6        (0x0806)
#define PH7        (0x0807)
#define PH8        (0x0808)
#define PH9        (0x0809)
#define PH10       (0x080A)
#define PH11       (0x080B)
#define PH12       (0x080C)
#define PH13       (0x080D)
#define PH14       (0x080E)
#define PH15       (0x080F)
#define PH16       (0x0810)
#define PH17       (0x0811)
#define PH18       (0x0812)
#define PH19       (0x0813)
#define PH20       (0x0814)
#define PH21       (0x0815)
#define PH22       (0x0816)
#define PH23       (0x0817)
#define PH24       (0x0818)
#define PH25       (0x0819)
#define PH26       (0x081A)
#define PH27       (0x081B)
#define PH28       (0x081C)
#define PH29       (0x081D)
#define PH30       (0x081E)
#define PH31       (0x081F)
#define PORTH      (0x08FF)

#define PI0        (0x0900)
#define PI1        (0x0901)
#define PI2        (0x0902)
#define PI3        (0x0903)
#define PI4        (0x0904)
#define PI5        (0x0905)
#define PI6        (0x0906)
#define PI7        (0x0907)
#define PI8        (0x0908)
#define PI9        (0x0909)
#define PI10       (0x090A)
#define PI11       (0x090B)
#define PI12       (0x090C)
#define PI13       (0x090D)
#define PI14       (0x090E)
#define PI15       (0x090F)
#define PI16       (0x0910)
#define PI17       (0x0911)
#define PI18       (0x0912)
#define PI19       (0x0913)
#define PI20       (0x0914)
#define PI21       (0x0915)
#define PI22       (0x0916)
#define PI23       (0x0917)
#define PI24       (0x0918)
#define PI25       (0x0919)
#define PI26       (0x091A)
#define PI27       (0x091B)
#define PI28       (0x091C)
#define PI29       (0x091D)
#define PI30       (0x091E)
#define PI31       (0x091F)
#define PORTI      (0x09FF)

#define PJ0        (0x0A00)
#define PJ1        (0x0A01)
#define PJ2        (0x0A02)
#define PJ3        (0x0A03)
#define PJ4        (0x0A04)
#define PJ5        (0x0A05)
#define PJ6        (0x0A06)
#define PJ7        (0x0A07)
#define PJ8        (0x0A08)
#define PJ9        (0x0A0A)
#define PJ10       (0x0A0A)
#define PJ11       (0x0A0B)
#define PJ12       (0x0A0C)
#define PJ13       (0x0A0D)
#define PJ14       (0x0A0E)
#define PJ15       (0x0A0F)
#define PJ16       (0x0A10)
#define PJ17       (0x0A11)
#define PJ18       (0x0A12)
#define PJ19       (0x0A13)
#define PJ20       (0x0A14)
#define PJ21       (0x0A15)
#define PJ22       (0x0A16)
#define PJ23       (0x0A17)
#define PJ24       (0x0A18)
#define PJ25       (0x0A19)
#define PJ26       (0x0A1A)
#define PJ27       (0x0A1B)
#define PJ28       (0x0A1C)
#define PJ29       (0x0A1D)
#define PJ30       (0x0A1E)
#define PJ31       (0x0A1F)
#define PORTJ      (0x0AFF)

#define PK0        (0x0B00)
#define PK1        (0x0B01)
#define PK2        (0x0B02)
#define PK3        (0x0B03)
#define PK4        (0x0B04)
#define PK5        (0x0B05)
#define PK6        (0x0B06)
#define PK7        (0x0B07)
#define PK8        (0x0B08)
#define PK9        (0x0B0B)
#define PK10       (0x0B0A)
#define PK11       (0x0B0B)
#define PK12       (0x0B0C)
#define PK13       (0x0B0D)
#define PK14       (0x0B0E)
#define PK15       (0x0B0F)
#define PK16       (0x0B10)
#define PK17       (0x0B11)
#define PK18       (0x0B12)
#define PK19       (0x0B13)
#define PK20       (0x0B14)
#define PK21       (0x0B15)
#define PK22       (0x0B16)
#define PK23       (0x0B17)
#define PK24       (0x0B18)
#define PK25       (0x0B19)
#define PK26       (0x0B1A)
#define PK27       (0x0B1B)
#define PK28       (0x0B1C)
#define PK29       (0x0B1D)
#define PK30       (0x0B1E)
#define PK31       (0x0B1F)
#define PORTK      (0x0BFF)

#define P0_0       (0x0100)
#define P0_1       (0x0101)
#define P0_2       (0x0102)
#define P0_3       (0x0103)
#define P0_4       (0x0104)
#define P0_5       (0x0105)
#define P0_6       (0x0106)
#define P0_7       (0x0107)
#define P0_8       (0x0108)
#define P0_9       (0x0109)
#define P0_10      (0x010A)
#define P0_11      (0x010B)
#define P0_12      (0x010C)
#define P0_13      (0x010D)
#define P0_14      (0x010E)
#define P0_15      (0x010F)
#define P0_16      (0x0110)
#define P0_17      (0x0111)
#define P0_18      (0x0112)
#define P0_19      (0x0113)
#define P0_20      (0x0114)
#define P0_21      (0x0115)
#define P0_22      (0x0116)
#define P0_23      (0x0117)
#define P0_24      (0x0118)
#define P0_25      (0x0119)
#define P0_26      (0x011A)
#define P0_27      (0x011B)
#define P0_28      (0x011C)
#define P0_29      (0x011D)
#define P0_30      (0x011E)
#define P0_31      (0x011F)

#define P0_32      (0x0120)
#define P0_33      (0x0121)
#define P0_34      (0x0122)
#define P0_35      (0x0123)
#define P0_36      (0x0124)
#define P0_37      (0x0125)
#define P0_38      (0x0126)
#define P0_39      (0x0127)
#define P0_40      (0x0128)
#define P0_41      (0x0129)
#define P0_42      (0x012A)
#define P0_43      (0x012B)
#define P0_44      (0x012C)
#define P0_45      (0x012D)
#define P0_46      (0x012E)
#define P0_47      (0x012F)
#define P0_48      (0x0130)
#define P0_49      (0x0131)
#define P0_50      (0x0132)
#define P0_51      (0x0133)
#define P0_52      (0x0134)
#define P0_53      (0x0135)
#define P0_54      (0x0136)
#define P0_55      (0x0137)
#define P0_56      (0x0138)
#define P0_57      (0x0139)
#define P0_58      (0x013A)
#define P0_59      (0x013B)
#define P0_60      (0x013C)
#define P0_61      (0x013D)
#define P0_62      (0x013E)
#define P0_63      (0x013F)
#define P0_64      (0x0140)
#define P0_65      (0x0141)
#define P0_66      (0x0142)
#define P0_67      (0x0143)
#define P0_68      (0x0144)
#define P0_69      (0x0145)
#define P0_70      (0x0146)
#define P0_71      (0x0147)
#define P0_72      (0x0148)
#define P0_73      (0x0149)
#define P0_74      (0x014A)
#define P0_75      (0x014B)
#define P0_76      (0x014C)
#define P0_77      (0x014D)
#define P0_78      (0x014E)
#define P0_79      (0x014F)
#define P0_80      (0x0150)
#define P0_81      (0x0151)
#define P0_82      (0x0152)
#define P0_83      (0x0153)
#define P0_84      (0x0154)
#define P0_85      (0x0155)
#define P0_86      (0x0156)
#define P0_87      (0x0157)
#define P0_88      (0x0158)
#define P0_89      (0x0159)
#define P0_90      (0x015A)
#define P0_91      (0x015B)
#define P0_92      (0x015C)
#define P0_93      (0x015D)
#define P0_94      (0x015E)
#define P0_95      (0x015F)
#define P0_96      (0x0160)
#define P0_97      (0x0161)
#define P0_98      (0x0162)
#define P0_99      (0x0163)
#define PORT0      (0x01FF)

#define P1_0       (0x0200)
#define P1_1       (0x0201)
#define P1_2       (0x0202)
#define P1_3       (0x0203)
#define P1_4       (0x0204)
#define P1_5       (0x0205)
#define P1_6       (0x0206)
#define P1_7       (0x0207)
#define P1_8       (0x0208)
#define P1_9       (0x0209)
#define P1_10      (0x020A)
#define P1_11      (0x020B)
#define P1_12      (0x020C)
#define P1_13      (0x020D)
#define P1_14      (0x020E)
#define P1_15      (0x020F)
#define P1_16      (0x0210)
#define P1_17      (0x0211)
#define P1_18      (0x0212)
#define P1_19      (0x0213)
#define P1_20      (0x0214)
#define P1_21      (0x0215)
#define P1_22      (0x0216)
#define P1_23      (0x0217)
#define P1_24      (0x0218)
#define P1_25      (0x0219)
#define P1_26      (0x021A)
#define P1_27      (0x021B)
#define P1_28      (0x021C)
#define P1_29      (0x021D)
#define P1_30      (0x021E)
#define P1_31      (0x021F)

#define P1_32      (0x0220)
#define P1_33      (0x0221)
#define P1_34      (0x0222)
#define P1_35      (0x0223)
#define P1_36      (0x0224)
#define P1_37      (0x0225)
#define P1_38      (0x0226)
#define P1_39      (0x0227)
#define P1_40      (0x0228)
#define P1_41      (0x0229)
#define P1_42      (0x022A)
#define P1_43      (0x022B)
#define P1_44      (0x022C)
#define P1_45      (0x022D)
#define P1_46      (0x022E)
#define P1_47      (0x022F)
#define P1_48      (0x0230)
#define P1_49      (0x0231)
#define P1_50      (0x0232)
#define P1_51      (0x0233)
#define P1_52      (0x0234)
#define P1_53      (0x0235)
#define P1_54      (0x0236)
#define P1_55      (0x0237)
#define P1_56      (0x0238)
#define P1_57      (0x0239)
#define P1_58      (0x023A)
#define P1_59      (0x023B)
#define P1_60      (0x023C)
#define P1_61      (0x023D)
#define P1_62      (0x023E)
#define P1_63      (0x023F)
#define P1_64      (0x0240)
#define P1_65      (0x0241)
#define P1_66      (0x0242)
#define P1_67      (0x0243)
#define P1_68      (0x0244)
#define P1_69      (0x0245)
#define P1_70      (0x0246)
#define P1_71      (0x0247)
#define P1_72      (0x0248)
#define P1_73      (0x0249)
#define P1_74      (0x024A)
#define P1_75      (0x024B)
#define P1_76      (0x024C)
#define P1_77      (0x024D)
#define P1_78      (0x024E)
#define P1_79      (0x024F)
#define P1_80      (0x0250)
#define P1_81      (0x0251)
#define P1_82      (0x0252)
#define P1_83      (0x0253)
#define P1_84      (0x0254)
#define P1_85      (0x0255)
#define P1_86      (0x0256)
#define P1_87      (0x0257)
#define P1_88      (0x0258)
#define P1_89      (0x0259)
#define P1_90      (0x025A)
#define P1_91      (0x025B)
#define P1_92      (0x025C)
#define P1_93      (0x025D)
#define P1_94      (0x025E)
#define P1_95      (0x025F)
#define P1_96      (0x0260)
#define P1_97      (0x0261)
#define P1_98      (0x0262)
#define P1_99      (0x0263)
#define PORT1      (0x02FF)

#define P2_0       (0x0300)
#define P2_1       (0x0301)
#define P2_2       (0x0302)
#define P2_3       (0x0303)
#define P2_4       (0x0304)
#define P2_5       (0x0305)
#define P2_6       (0x0306)
#define P2_7       (0x0307)
#define P2_8       (0x0308)
#define P2_9       (0x0309)
#define P2_10      (0x030A)
#define P2_11      (0x030B)
#define P2_12      (0x030C)
#define P2_13      (0x030D)
#define P2_14      (0x030E)
#define P2_15      (0x030F)
#define P2_16      (0x0310)
#define P2_17      (0x0311)
#define P2_18      (0x0312)
#define P2_19      (0x0313)
#define P2_20      (0x0314)
#define P2_21      (0x0315)
#define P2_22      (0x0316)
#define P2_23      (0x0317)
#define P2_24      (0x0318)
#define P2_25      (0x0319)
#define P2_26      (0x031A)
#define P2_27      (0x031B)
#define P2_28      (0x031C)
#define P2_29      (0x031D)
#define P2_30      (0x031E)
#define P2_31      (0x031F)

#define P2_32      (0x0320)
#define P2_33      (0x0321)
#define P2_34      (0x0322)
#define P2_35      (0x0323)
#define P2_36      (0x0324)
#define P2_37      (0x0325)
#define P2_38      (0x0326)
#define P2_39      (0x0327)
#define P2_40      (0x0328)
#define P2_41      (0x0329)
#define P2_42      (0x032A)
#define P2_43      (0x032B)
#define P2_44      (0x032C)
#define P2_45      (0x032D)
#define P2_46      (0x032E)
#define P2_47      (0x032F)
#define P2_48      (0x0330)
#define P2_49      (0x0331)
#define P2_50      (0x0332)
#define P2_51      (0x0333)
#define P2_52      (0x0334)
#define P2_53      (0x0335)
#define P2_54      (0x0336)
#define P2_55      (0x0337)
#define P2_56      (0x0338)
#define P2_57      (0x0339)
#define P2_58      (0x033A)
#define P2_59      (0x033B)
#define P2_60      (0x033C)
#define P2_61      (0x033D)
#define P2_62      (0x033E)
#define P2_63      (0x033F)
#define P2_64      (0x0340)
#define P2_65      (0x0341)
#define P2_66      (0x0342)
#define P2_67      (0x0343)
#define P2_68      (0x0344)
#define P2_69      (0x0345)
#define P2_70      (0x0346)
#define P2_71      (0x0347)
#define P2_72      (0x0348)
#define P2_73      (0x0349)
#define P2_74      (0x034A)
#define P2_75      (0x034B)
#define P2_76      (0x034C)
#define P2_77      (0x034D)
#define P2_78      (0x034E)
#define P2_79      (0x034F)
#define P2_80      (0x0350)
#define P2_81      (0x0351)
#define P2_82      (0x0352)
#define P2_83      (0x0353)
#define P2_84      (0x0354)
#define P2_85      (0x0355)
#define P2_86      (0x0356)
#define P2_87      (0x0357)
#define P2_88      (0x0358)
#define P2_89      (0x0359)
#define P2_90      (0x035A)
#define P2_91      (0x035B)
#define P2_92      (0x035C)
#define P2_93      (0x035D)
#define P2_94      (0x035E)
#define P2_95      (0x035F)
#define P2_96      (0x0360)
#define P2_97      (0x0361)
#define P2_98      (0x0362)
#define P2_99      (0x0363)
#define PORT2      (0x03FF)

#define P3_0       (0x0400)
#define P3_1       (0x0401)
#define P3_2       (0x0402)
#define P3_3       (0x0403)
#define P3_4       (0x0404)
#define P3_5       (0x0405)
#define P3_6       (0x0406)
#define P3_7       (0x0407)
#define P3_8       (0x0408)
#define P3_9       (0x0409)
#define P3_10      (0x040A)
#define P3_11      (0x040B)
#define P3_12      (0x040C)
#define P3_13      (0x040D)
#define P3_14      (0x040E)
#define P3_15      (0x040F)
#define P3_16      (0x0410)
#define P3_17      (0x0411)
#define P3_18      (0x0412)
#define P3_19      (0x0413)
#define P3_20      (0x0414)
#define P3_21      (0x0415)
#define P3_22      (0x0416)
#define P3_23      (0x0417)
#define P3_24      (0x0418)
#define P3_25      (0x0419)
#define P3_26      (0x041A)
#define P3_27      (0x041B)
#define P3_28      (0x041C)
#define P3_29      (0x041D)
#define P3_30      (0x041E)
#define P3_31      (0x041F)

#define P3_32      (0x0420)
#define P3_33      (0x0421)
#define P3_34      (0x0422)
#define P3_35      (0x0423)
#define P3_36      (0x0424)
#define P3_37      (0x0425)
#define P3_38      (0x0426)
#define P3_39      (0x0427)
#define P3_40      (0x0428)
#define P3_41      (0x0429)
#define P3_42      (0x042A)
#define P3_43      (0x042B)
#define P3_44      (0x042C)
#define P3_45      (0x042D)
#define P3_46      (0x042E)
#define P3_47      (0x042F)
#define P3_48      (0x0430)
#define P3_49      (0x0431)
#define P3_50      (0x0432)
#define P3_51      (0x0433)
#define P3_52      (0x0434)
#define P3_53      (0x0435)
#define P3_54      (0x0436)
#define P3_55      (0x0437)
#define P3_56      (0x0438)
#define P3_57      (0x0439)
#define P3_58      (0x043A)
#define P3_59      (0x043B)
#define P3_60      (0x043C)
#define P3_61      (0x043D)
#define P3_62      (0x043E)
#define P3_63      (0x043F)
#define P3_64      (0x0440)
#define P3_65      (0x0441)
#define P3_66      (0x0442)
#define P3_67      (0x0443)
#define P3_68      (0x0444)
#define P3_69      (0x0445)
#define P3_70      (0x0446)
#define P3_71      (0x0447)
#define P3_72      (0x0448)
#define P3_73      (0x0449)
#define P3_74      (0x044A)
#define P3_75      (0x044B)
#define P3_76      (0x044C)
#define P3_77      (0x044D)
#define P3_78      (0x044E)
#define P3_79      (0x044F)
#define P3_80      (0x0450)
#define P3_81      (0x0451)
#define P3_82      (0x0452)
#define P3_83      (0x0453)
#define P3_84      (0x0454)
#define P3_85      (0x0455)
#define P3_86      (0x0456)
#define P3_87      (0x0457)
#define P3_88      (0x0458)
#define P3_89      (0x0459)
#define P3_90      (0x045A)
#define P3_91      (0x045B)
#define P3_92      (0x045C)
#define P3_93      (0x045D)
#define P3_94      (0x045E)
#define P3_95      (0x045F)
#define P3_96      (0x0460)
#define P3_97      (0x0461)
#define P3_98      (0x0462)
#define P3_99      (0x0463)
#define PORT3      (0x04FF)

#define P4_0       (0x0500)
#define P4_1       (0x0501)
#define P4_2       (0x0502)
#define P4_3       (0x0503)
#define P4_4       (0x0504)
#define P4_5       (0x0505)
#define P4_6       (0x0506)
#define P4_7       (0x0507)
#define P4_8       (0x0508)
#define P4_9       (0x0509)
#define P4_10      (0x050A)
#define P4_11      (0x050B)
#define P4_12      (0x050C)
#define P4_13      (0x050D)
#define P4_14      (0x050E)
#define P4_15      (0x050F)
#define P4_16      (0x0510)
#define P4_17      (0x0511)
#define P4_18      (0x0512)
#define P4_19      (0x0513)
#define P4_20      (0x0514)
#define P4_21      (0x0515)
#define P4_22      (0x0516)
#define P4_23      (0x0517)
#define P4_24      (0x0518)
#define P4_25      (0x0519)
#define P4_26      (0x051A)
#define P4_27      (0x051B)
#define P4_28      (0x051C)
#define P4_29      (0x051D)
#define P4_30      (0x051E)
#define P4_31      (0x051F)

#define P4_32      (0x0520)
#define P4_33      (0x0521)
#define P4_34      (0x0522)
#define P4_35      (0x0523)
#define P4_36      (0x0524)
#define P4_37      (0x0525)
#define P4_38      (0x0526)
#define P4_39      (0x0527)
#define P4_40      (0x0528)
#define P4_41      (0x0529)
#define P4_42      (0x052A)
#define P4_43      (0x052B)
#define P4_44      (0x052C)
#define P4_45      (0x052D)
#define P4_46      (0x052E)
#define P4_47      (0x052F)
#define P4_48      (0x0530)
#define P4_49      (0x0531)
#define P4_50      (0x0532)
#define P4_51      (0x0533)
#define P4_52      (0x0534)
#define P4_53      (0x0535)
#define P4_54      (0x0536)
#define P4_55      (0x0537)
#define P4_56      (0x0538)
#define P4_57      (0x0539)
#define P4_58      (0x053A)
#define P4_59      (0x053B)
#define P4_60      (0x053C)
#define P4_61      (0x053D)
#define P4_62      (0x053E)
#define P4_63      (0x053F)
#define P4_64      (0x0540)
#define P4_65      (0x0541)
#define P4_66      (0x0542)
#define P4_67      (0x0543)
#define P4_68      (0x0544)
#define P4_69      (0x0545)
#define P4_70      (0x0546)
#define P4_71      (0x0547)
#define P4_72      (0x0548)
#define P4_73      (0x0549)
#define P4_74      (0x054A)
#define P4_75      (0x054B)
#define P4_76      (0x054C)
#define P4_77      (0x054D)
#define P4_78      (0x054E)
#define P4_79      (0x054F)
#define P4_80      (0x0550)
#define P4_81      (0x0551)
#define P4_82      (0x0552)
#define P4_83      (0x0553)
#define P4_84      (0x0554)
#define P4_85      (0x0555)
#define P4_86      (0x0556)
#define P4_87      (0x0557)
#define P4_88      (0x0558)
#define P4_89      (0x0559)
#define P4_90      (0x055A)
#define P4_91      (0x055B)
#define P4_92      (0x055C)
#define P4_93      (0x055D)
#define P4_94      (0x055E)
#define P4_95      (0x055F)
#define P4_96      (0x0560)
#define P4_97      (0x0561)
#define P4_98      (0x0562)
#define P4_99      (0x0563)
#define PORT4      (0x05FF)


//#define PA         (0x0100)
//#define PB         (0x0200)
//#define PC         (0x0300)
//#define PD         (0x0400)
//#define PE         (0x0500)
//#define PF         (0x0600)
//#define PG         (0x0700)
//#define PH         (0x0800)
//#define PI         (0x0900)
//#define PJ         (0x0A00)
//#define PK         (0x0B00)


#define PORTx(pin) ((u8)(pin >> 8))
#define PINx(pin)  ((u8)pin)

#define PORT_MASK  (0xFF00)
#define PIN_MASK   (0x00FF)


#define GPIO_RET_INVALUE     (0xFFFFFFFF)


/* GPIO驱动编号 */
#define GPIO_DRV_NAT_NUM     0
#define GPIO_DRV_EXT_NUM     2 


typedef enum
{
    FW_GPIO_Mode_AIN = 0,                   //模拟输入
    FW_GPIO_Mode_AOUT,                      //模拟输出

    FW_GPIO_Mode_AF_IPN,                    //复用浮空输入
    FW_GPIO_Mode_AF_IPU,                    //复用上拉输入
    FW_GPIO_Mode_AF_IPD,                    //复用下拉输入

    FW_GPIO_Mode_AF_Out_PPN,                //复用浮空推挽输出
    FW_GPIO_Mode_AF_Out_PPU,                //复用上拉推挽输出
    FW_GPIO_Mode_AF_Out_PPD,                //复用下拉推挽输出

    FW_GPIO_Mode_AF_Out_ODN,                //复用浮空开漏输出
    FW_GPIO_Mode_AF_Out_ODU,                //复用浮空上拉输出
    FW_GPIO_Mode_AF_Out_ODD,                //复用浮空下拉输出

    FW_GPIO_Mode_IPN,                       //浮空输入
    FW_GPIO_Mode_IPU,                       //上拉输入
    FW_GPIO_Mode_IPD,                       //下拉输入

    FW_GPIO_Mode_Out_PPN,                   //浮空推挽输出
    FW_GPIO_Mode_Out_PPU,                   //上拉推挽输出
    FW_GPIO_Mode_Out_PPD,                   //下拉推挽输出

    FW_GPIO_Mode_Out_ODN,                   //浮空开漏输出
    FW_GPIO_Mode_Out_ODU,                   //上拉开漏输出
    FW_GPIO_Mode_Out_ODD,                   //下拉开漏输出
}FW_GPIO_Mode_Enum;                         //GPIO工作模式
/**/


typedef enum
{
    FW_GPIO_Speed_Low,                      //低速
    FW_GPIO_Speed_Medium,                   //中速
    FW_GPIO_Speed_High,                     //高速
    FW_GPIO_Speed_Ultra,                    //极速
}FW_GPIO_Speed_Enum;
/**/


typedef struct FW_GPIO
{
    FW_Device_Type Device;
    
    u32 Default_Mode : 8;
    u32 Default_Speed : 4;
    
    u32 : 20;
    
    void (*Pin_Toggle)(struct FW_GPIO *pdev, u16 pin);
}FW_GPIO_Type;


typedef struct
{
    /* Pin */
    void (*Pin_DeInit)(FW_GPIO_Type *dev, u16 pin);
    void (*Pin_Init)(FW_GPIO_Type *dev, u16 pin, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed);
    
    void (*Pin_Write)(FW_GPIO_Type *dev, u16 pin, u8 value);
    u8   (*Pin_GetOutput)(FW_GPIO_Type *dev, u16 pin);
    
    u8   (*Pin_Read)(FW_GPIO_Type *dev, u16 pin);
    
    void (*Pin_Toggle)(FW_GPIO_Type *dev, u16 pin);
    
    /* Port */
    void (*Port_GetDefault)(FW_GPIO_Type *dev);
    
    void (*Port_DeInit)(FW_GPIO_Type *dev, u16 port);
    void (*Port_Init)(FW_GPIO_Type *dev, u16 port, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed);
    
    void (*Port_Write)(FW_GPIO_Type *dev, u16 port, u32 value);
    u32  (*Port_GetOutput)(FW_GPIO_Type *dev, u16 port);
    
    u32  (*Port_Read)(FW_GPIO_Type *dev, u16 port);
}FW_GPIO_Driver_Type;


/**
 * @API
 */
void GPIO_PinDeInit(FW_GPIO_Type *dev, u16 pin);
void GPIO_PinInit(FW_GPIO_Type *dev, u16 pin, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed);

void GPIO_PinWrite(FW_GPIO_Type *dev, u16 pin, u8 value);
u8   GPIO_PinGetOutput(FW_GPIO_Type *dev, u16 pin);

u8   GPIO_PinRead(FW_GPIO_Type *dev, u16 pin);

void GPIO_PinToggle(FW_GPIO_Type *dev, u16 pin);


void GPIO_PortDeInit(FW_GPIO_Type *dev, u16 port);
void GPIO_PortInit(FW_GPIO_Type *dev, u16 port, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed);

void GPIO_PortWrite(FW_GPIO_Type *dev, u16 port, u32 value);
u32  GPIO_PortGetOutput(FW_GPIO_Type *dev, u16 port);

u32  GPIO_PortRead(FW_GPIO_Type *dev, u16 port);


/**
 * @FW_GPIO
 */
extern FW_GPIO_Type GPIO;

#if defined(GPIO_RELOAD_EN) && (GPIO_RELOAD_EN)

/**
 * @Pin
 */
#define FW_GPIO_DeInit(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 1)  GPIO_PinDeInit(&GPIO, VA0(__VA_ARGS__));\
    else if(n == 2)  GPIO_PinDeInit((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_GPIO_Init(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 3)  GPIO_PinInit(&GPIO, VA0(__VA_ARGS__), (FW_GPIO_Mode_Enum)VA1(__VA_ARGS__), (FW_GPIO_Speed_Enum)VA2(__VA_ARGS__));\
    else if(n == 4)  GPIO_PinInit((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__), (FW_GPIO_Mode_Enum)VA2(__VA_ARGS__), (FW_GPIO_Speed_Enum)VA3(__VA_ARGS__));\
    else VA_ARGS_NUM_ERR();\
}while(0)

#define FW_GPIO_Write(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 2)  GPIO_PinWrite(&GPIO, VA0(__VA_ARGS__), VA1(__VA_ARGS__));\
    else if(n == 3)  GPIO_PinWrite((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__), VA2(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_GPIO_GetOutput(...)(\
(VA_NUM(__VA_ARGS__) == 1) ? GPIO_PinGetOutput(&GPIO, VA0(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 2) ? GPIO_PinGetOutput((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_GPIO_Read(...)(\
(VA_NUM(__VA_ARGS__) == 1) ? GPIO_PinRead(&GPIO, VA0(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 2) ? GPIO_PinRead((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_GPIO_Toggle(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 1)  GPIO_PinToggle(&GPIO, VA0(__VA_ARGS__));\
    else if(n == 2)  GPIO_PinToggle((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)


#define FW_GPIO_SET(...)  FW_GPIO_Write(__VA_ARGS__, LEVEL_H)
#define FW_GPIO_CLR(...)  FW_GPIO_Write(__VA_ARGS__, LEVEL_L)


/**
 * @Port
 */
#define FW_GPIO_ProtDeInit(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 1)  GPIO_PortDeInit(&GPIO, VA0(__VA_ARGS__));\
    else if(n == 2)  GPIO_PortDeInit((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_GPIO_PortInit(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 3)  GPIO_PortInit(&GPIO, VA0(__VA_ARGS__), (FW_GPIO_Mode_Enum)VA1(__VA_ARGS__), (FW_GPIO_Speed_Enum)VA2(__VA_ARGS__));\
    else if(n == 4)  GPIO_PortInit((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__), (FW_GPIO_Mode_Enum)VA2(__VA_ARGS__), (FW_GPIO_Speed_Enum)VA3(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_GPIO_PortWrite(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 2)  GPIO_PortWrite(&GPIO, VA0(__VA_ARGS__), VA1(__VA_ARGS__));\
    else if(n == 3)  GPIO_PortWrite((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__), VA2(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_GPIO_PortRead(...)(\
(VA_NUM(__VA_ARGS__) == 1) ? GPIO_Read(&GPIO, VA0(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 2) ? GPIO_Read((FW_GPIO_Type *)VA0(__VA_ARGS__), VA1(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#else

/**
 * @Pin
 */
__INLINE_STATIC_ void FW_GPIO_DeInit(u16 pin)
{
    GPIO_PinDeInit(&GPIO, pin);
}

__INLINE_STATIC_ void FW_GPIO_Init(u16 pin, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    GPIO_PinInit(&GPIO, pin, mode, speed);
}

__INLINE_STATIC_ void FW_GPIO_Write(u16 pin, u8 value)
{
    GPIO_PinWrite(&GPIO, pin, value);
}

__INLINE_STATIC_ u8   FW_GPIO_GetOutput(u16 pin)
{
    return GPIO_PinGetOutput(&GPIO, pin);
}

__INLINE_STATIC_ u8   FW_GPIO_Read(u16 pin)
{
    return GPIO_PinRead(&GPIO, pin);
}

__INLINE_STATIC_ void FW_GPIO_Toggle(u16 pin)
{
    GPIO_PinToggle(&GPIO, pin);
}


#define FW_GPIO_SET(pin)     FW_GPIO_Write(pin, LEVEL_H)
#define FW_GPIO_CLR(pin)     FW_GPIO_Write(pin, LEVEL_L)


/**
 * @Port
 */
__INLINE_STATIC_ void FW_GPIO_PortDeInit(u16 port)
{
    GPIO_PortDeInit(&GPIO, port);
}

__INLINE_STATIC_ void FW_GPIO_PortInit(u16 port, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    GPIO_PortInit(&GPIO, port, mode, speed);
}

__INLINE_STATIC_ void FW_GPIO_PortWrite(u16 port, u32 value)
{
    GPIO_PortWrite(&GPIO, port, value);
}

__INLINE_STATIC_ u32  FW_GPIO_PortGetOutput(u16 port)
{
    return GPIO_PortGetOutput(&GPIO, port);
}

__INLINE_STATIC_ u32  FW_GPIO_PortRead(u16 port)
{
    return GPIO_PortRead(&GPIO, port);
}

#endif  /* #if defined(GPIO_RELOAD_EN) && (GPIO_RELAOD_EN) */


#ifdef __cplusplus
}
#endif

#endif

