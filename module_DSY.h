/******************************************************/
/**                                                   */
/**    ARCHIVO DE CABECERA "module_DSY.h"             */
/**    CAPA: ESPECIFICO DEL MODULO DSY                */
/**    FECHA: AGO-2018                                */
/**    VERSION: v1.0                                  */
/**                                                   */
/******************************************************/

/* Include Guards */
#ifndef MODULE_DSY_H
#define MODULE_DSY_H
        
        /*************/
        /** INCLUDE **/
        /*************/
        #include <ioC8051f040.h>                          // SFR declarations
        #include <page_defs.h>
        
        /* typedef */
        /* uint8_t - uint16_t - uint32_t - uint64_t */
        #include <stdint.h>
        
        /* typedef */
        /* bool - true - false */
        #include <stdbool.h>
        
        
        /************/
        /** DEFINE **/
        /************/
     /*           // Estados de la maquina de estados para seleccionar los CS y los CH
                #define READ_OUT41	0x00
                #define READ_OUT11	0x01 
                #define READ_OUT21	0x02 
                #define READ_OUT31	0x03
                #define READ_OUT42	0x04
                #define READ_OUT12	0x05
                #define READ_OUT22	0x06
                #define READ_OUT32	0x07
                #define READ_OUT43	0x08
                #define READ_OUT13	0x09
                #define READ_OUT23	0x0A
                #define READ_OUT33	0x0B
                #define READ_IN4	0x0C
                #define READ_IN1	0x0D
                #define READ_IN2	0x0E  
                #define READ_IN3	0x0F
                #define WAIT		0x10        */
        
        /************************************/
        /** PUERTOS ENTRADA SALIDA GENERAL **/
        /************************************/
        
        /* Entradas - Salidas Duras */
             
        
        /* Watchdog Externo */
        #define  portBitGetWDExtern                                             P2_bit.P27 
        #define  portBitSetWDExtern_ON                                          P2_bit.P27 = 1 
        #define  portBitSetWDExtern_OFF                                         P2_bit.P27 = 0 
        
        
        /* AutoDiasgnosticTest */
	
/*	#define  portBitGetVNimState		                                P1_bit.P12

        #define  portBitSetStateLed_ON                                          P1_bit.P16 = 1 
        #define  portBitSetStateLed_OFF                                         P1_bit.P16 = 0 

	#define  portBitSetEnable24_ON                                          P1_bit.P11 = 1 
        #define  portBitSetEnable24_OFF                                         P1_bit.P11 = 0 

	#define  portBitSetAlive_ON                                          	P1_bit.P10 = 1 
        #define  portBitSetAlive_OFF                                         	P1_bit.P10 = 0 */
        
        /* Power NIM */
        #define  portBitGetVP24Nim                                           	0
        #define  portBitGetVP12Nim                                           	0
        #define  portBitGetVN24Nim                                           	0  
        #define  portBitGetVN12Nim                                           	0        
         

       

        /* Output Extern (Relay Command) */
        
/*	#define  portBitSetOut_1_ON                                             P6_bit.P60 = 1    
	#define  portBitSetOut_1_OFF                                            P6_bit.P60 = 0  
	#define  portBitSetOut_2_ON                                             P6_bit.P61 = 1  
	#define  portBitSetOut_2_OFF                                            P6_bit.P61 = 0  
	#define  portBitSetOut_3_ON                                             P6_bit.P62 = 1  
	#define  portBitSetOut_3_OFF                                            P6_bit.P62 = 0  
	#define  portBitSetOut_4_ON                                             P6_bit.P63 = 1  
	#define  portBitSetOut_4_OFF                                            P6_bit.P63 = 0  
	#define  portBitSetOut_5_ON                                             P6_bit.P64 = 1  
	#define  portBitSetOut_5_OFF                                            P6_bit.P64 = 0  
	#define  portBitSetOut_6_ON                                             P6_bit.P65 = 1  
	#define  portBitSetOut_6_OFF                                            P6_bit.P65 = 0  
	#define  portBitSetOut_7_ON                                             P6_bit.P66 = 1  
	#define  portBitSetOut_7_OFF                                            P6_bit.P66 = 0  
	#define  portBitSetOut_8_ON                                             P5_bit.P50 = 1  
	#define  portBitSetOut_8_OFF                                            P5_bit.P50 = 0  
	#define  portBitSetOut_9_ON                                             P5_bit.P51 = 1  
	#define  portBitSetOut_9_OFF                                            P5_bit.P51 = 0  
	#define  portBitSetOut_10_ON                                            P5_bit.P52 = 1  
	#define  portBitSetOut_10_OFF                                           P5_bit.P52 = 0  
	#define  portBitSetOut_11_ON                                            P5_bit.P53 = 1  
	#define  portBitSetOut_11_OFF                                           P5_bit.P53 = 0  
	#define  portBitSetOut_12_ON                                            P5_bit.P54 = 1  
	#define  portBitSetOut_12_OFF                                           P5_bit.P54 = 0  */

        /*************/
        /** DRIVERS **/
        /*************/

	/* ADC (CS y EN) */

	#define  CS1 P0_bit.P05 // sbit CS1 = P7^4;
  //      #define  CS2 P7_bit.P75 // sbit CS2 = P7^5;
  //      #define  CS3 P7_bit.P76 // sbit CS3 = P7^6;
  //      #define  CS4 P7_bit.P77 // sbit CS4 = P7^7;
        
       // #define  EN4 P2_bit.P23 // sbit EN4 = P2^3;
   //     #define  EN3 P2_bit.P22 // sbit EN3 = P2^2;
   //     #define  EN2 P2_bit.P21 // sbit EN2 = P2^1;
   //     #define  EN1 P2_bit.P20 // sbit EN1 = P2^0;

	#define REF2_5 0x2// +/- 2.5V
	#define REF10 0x0// +/- 10V

             

	/* CAN */

	#define portBitGetCanAddress_00						P4_bit.P40
	#define portBitGetCanAddress_01						P4_bit.P41	
	#define portBitGetCanAddress_02						P4_bit.P42
	#define portBitGetCanAddress_03						P4_bit.P43
	#define portBitGetCanAddress_04						P4_bit.P44
	#define portBitGetCanAddress_05						P4_bit.P45

        /* I2C */

        #define  SDA                                  				P0_bit.P06  
        #define  SCL                                  				P0_bit.P07  

        /* SPI */

	/*Habilitación de los tri-states para seleccionar el MISO correspondiente (ver de que el micro inicialice estos bits en cero y que se enciendan solo uno a la vez)*/
/*	#define portBitSetSpiMiso1_ON						P2_bit.P20 = 1
	#define portBitSetSpiMiso1_OFF						P2_bit.P20 = 0
	#define portBitSetSpiMiso2_ON						P2_bit.P21 = 1
	#define portBitSetSpiMiso2_OFF						P2_bit.P21 = 0
	#define portBitSetSpiMiso3_ON						P2_bit.P22 = 1
	#define portBitSetSpiMiso3_OFF						P2_bit.P22 = 0
	#define portBitSetSpiMiso4_ON						P2_bit.P23 = 1
	#define portBitSetSpiMiso4_OFF						P2_bit.P23 = 0      */

	/*Habilitación del Chip Select con el que selecciono el ADC a utilizar (ver de que el micro inicialice estos bits en cero y que se enciendan solo uno a la vez)*/
/*	#define portBitSetSpiCs1_ON						P7_bit.P74 = 1
	#define portBitSetSpiCs1_OFF						P7_bit.P74 = 0
	#define portBitSetSpiCs2_ON						P7_bit.P75 = 1
	#define portBitSetSpiCs2_OFF						P7_bit.P75 = 0
	#define portBitSetSpiCs3_ON						P7_bit.P76 = 1
	#define portBitSetSpiCs3_OFF						P7_bit.P76 = 0
	#define portBitSetSpiCs4_ON						P7_bit.P77 = 1
	#define portBitSetSpiCs4_OFF						P7_bit.P77 = 0       */


	
	#define DO_Led P1_bit.P16 
	#define Sup_Enable P1_bit.P11
	#define Sup_Alive P1_bit.P10
      
        #define SYSCLK 22118400
        #define T_MUESTREOL 	0xC4 	// 130 us
        #define T_MUESTREOH 	0xF4 	// 130 us

         #define TIPMOD             0x09                 //tipo de modulo 0E
       // #define In_min             500               //valor mínimo de entrada para fuera de rango (600 cuentas)
      //  #define In_max             4000               //valor máximo de entrada para fuera de rango (1400 cuentas)

        /***************/
        /** VARIABLES **/
        /***************/
        
        /* Estructura de memoria */
        
        // Param.Logicas

        struct Parametros_ {
                uint16_t Ch1_In_pend; // pendiente en cuentas
                uint16_t Ch1_In_ord;  // ordenada en cuentas
		uint16_t Ch2_In_pend; // pendiente en cuentas
                uint16_t Ch2_In_ord;  // ordenada en cuentas
		uint16_t Ch3_In_pend; // pendiente en cuentas
                uint16_t Ch3_In_ord;  // ordenada en cuentas
		uint16_t Ch4_In_pend; // pendiente en cuentas
                uint16_t Ch4_In_ord;  // ordenada en cuentas
                uint16_t Ch1_Out1_pend; // pendiente en cuentas
                uint16_t Ch1_Out1_ord;  // ordenada en cuentas
            	uint16_t Ch2_Out1_pend; // pendiente en cuentas
                uint16_t Ch2_Out1_ord;  // ordenada en cuentas
		uint16_t Ch3_Out1_pend; // pendiente en cuentas
                uint16_t Ch3_Out1_ord;  // ordenada en cuentas
		uint16_t Ch4_Out1_pend; // pendiente en cuentas
                uint16_t Ch4_Out1_ord;  // ordenada en cuentas
		uint16_t Ch1_Out2_pend; // pendiente en cuentas
                uint16_t Ch1_Out2_ord;  // ordenada en cuentas
            	uint16_t Ch2_Out2_pend; // pendiente en cuentas
                uint16_t Ch2_Out2_ord;  // ordenada en cuentas
		uint16_t Ch3_Out2_pend; // pendiente en cuentas
                uint16_t Ch3_Out2_ord;  // ordenada en cuentas
		uint16_t Ch4_Out2_pend; // pendiente en cuentas
                uint16_t Ch4_Out2_ord;  // ordenada en cuentas
		uint16_t Ch1_Out3_pend; // pendiente en cuentas
                uint16_t Ch1_Out3_ord;  // ordenada en cuentas
            	uint16_t Ch2_Out3_pend; // pendiente en cuentas
                uint16_t Ch2_Out3_ord;  // ordenada en cuentas
		uint16_t Ch3_Out3_pend; // pendiente en cuentas
                uint16_t Ch3_Out3_ord;  // ordenada en cuentas
		uint16_t Ch4_Out3_pend; // pendiente en cuentas
                uint16_t Ch4_Out3_ord;  // ordenada en cuentas
                uint8_t Tipo;
                uint16_t CRC;
};

	/*Estructuras de variables analógicas de los ADC*/

	struct Analog_ {
		int16_t Dato;
		int16_t Cuentas_Ant;
		int8_t Cont;
                uint16_t valorCalib;
		uint16_t Acum;
                char Prom;
	};
        
        /*Estructuras con banderas y contadores de errores de salida*/
        struct OutDatos {
                             bool DiscrepOut1;
                             bool DiscrepOut2;
                             bool DiscrepOut3;
                             bool OutOfRange;
                             bool FallaOut1;
                             bool FallaOut2;
                             bool FallaOut3;
                             uint8_t FallaContOut1;
                             uint8_t FallaContOut2;
                             uint8_t FallaContOut3;                     
                        };
        
      
        
	
	/* Valor de inicio */
        extern uint8_t Tx_trama[16];
        
        extern struct Parametros_ Param;
	extern struct Analog_  In1;// Out11, Out12, Out13;
	//extern struct Analog_  In2, Out21, Out22, Out23;
	//extern struct Analog_  In3, Out31, Out32, Out33;
	//extern struct Analog_  In4, Out41, Out42, Out43;
        //extern struct OutDatos OutCh1,OutCh2,OutCh3,OutCh4;
        
        
        
        /***************/
        /** FUNCIONES **/
        /***************/
        void modulePortInit( void );  
	void external_osc (void);
	uint8_t * TxBuild ();
        uint8_t * TxBuildCalib (uint8_t CH, uint8_t paquete);
        uint8_t * TxBuildCalibResponse ();
        void wait_us (void);
        void wait_ms (int ms);
        void ConfigCalib( void );


/* End Include Guards */
#endif