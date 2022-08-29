/******************************************************/
/**                                                   */
/**    ARCHIVO DE CABECERA "CAN.h"                    */
/**    BLOQUE DE PROGRAMACION "CAN"                   */
/**    CAPA: APPLICATION                              */
/**    FECHA: FEB-2018                                */
/**    VERSION: v1.0                                  */
/**                                                   */
/******************************************************/

/* Include Guards */
#ifndef CAN_H
#define CAN_H

        /*************/
        /** INCLUDE **/
        /*************/
        /* Registros microcontrolador */
        #include <ioC8051f040.h>
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
                
        /***************/
        /** VARIABLES **/
        /***************/
        
        
        //************************************ Definiciones **************************************
        
        // Registros de CAN - Tabla 18.2 del datasheet de C8051F040
        
        #define CANCTRL            0x00                 //Control Register
        #define CANSTAT            0x01                 //Status Register
        #define ERRCNT             0x02                 //Error Counter Register
        #define BITREG             0x03                 //Bit Timing Register
        #define INTREG             0x04                 //Interrupt Low Byte Register
        #define CANTSTR            0x05                 //Test Register
        #define BRPEXT             0x06                 //BRP Extension Register
        //Registros de Interfase 1 - IF1 Registers
        #define IF1CMDRQST         0x08                 //IF1 Command Rest      Register
        #define IF1CMDMSK          0x09                 //IF1 Command Mask      Register
        #define IF1MSK1            0x0A                 //IF1 Mask1             Register
        #define IF1MSK2            0x0B                 //IF1 Mask2             Register
        #define IF1ARB1            0x0C                 //IF1 Arbitration 1     Register
        #define IF1ARB2            0x0D                 //IF1 Arbitration 2     Register
        #define IF1MSGC            0x0E                 //IF1 Message Control   Register
        #define IF1DATA1           0x0F                 //IF1 Data A1           Register
        #define IF1DATA2           0x10                 //IF1 Data A2           Register
        #define IF1DATB1           0x11                 //IF1 Data B1           Register
        #define IF1DATB2           0x12                 //IF1 Data B2           Register
        //Registros de Interfase 2 - IF2 Registers
        #define IF2CMDRQST         0x20                 //IF2 Command Rest      Register
        #define IF2CMDMSK          0x21                 //IF2 Command Mask      Register
        #define IF2MSK1            0x22                 //IF2 Mask1             Register
        #define IF2MSK2            0x23                 //IF2 Mask2             Register
        #define IF2ARB1            0x24                 //IF2 Arbitration 1     Register
        #define IF2ARB2            0x25                 //IF2 Arbitration 2     Register
        #define IF2MSGC            0x26                 //IF2 Message Control   Register
        #define IF2DATA1           0x27                 //IF2 Data A1           Register
        #define IF2DATA2           0x28                 //IF2 Data A2           Register
        #define IF2DATB1           0x29                 //IF2 Data B1           Register
        #define IF2DATB2           0x2A                 //IF2 Data B2           Register
        //Message Handler Registers
        #define TRANSREQ1          0x40                 //Transmission Rest1 Register
        #define TRANSREQ2          0x41                 //Transmission Rest2 Register
        #define NEWDAT1            0x48                 //New Data 1            Register
        #define NEWDAT2            0x49                 //New Data 2            Register
        #define INTPEND1           0x50                 //Interrupt Pending 1   Register
        #define INTPEND2           0x51                 //Interrupt Pending 2   Register
        #define MSGVAL1            0x58                 //Message Valid 1       Register
        #define MSGVAL2            0x59                 //Message Valid 2       Register
        
        
        //__sfr __no_init volatile unsigned short CAN0DAT @ 0xD8;
        
        extern unsigned char CAN_DatoRX[16];
  	extern unsigned char CAN_DatoTX[8];
        extern __no_init __bit bool CAN_tx;
        extern __no_init __bit bool Preg;
        extern unsigned char verificador;
        extern char dirmod;                 // Direccion del modulo
        
        extern unsigned char CantRx;
        extern unsigned char ReinicioCAN;
        
        extern void TXCanID(void);
        extern char ResetSup;
        extern unsigned char cmd_TX, slave_diag, dir_cont;	// usadas tb en CAN.c
        
        // Identificación de tipo de modulo
        #define MCM_ID 	0
        #define CMP_ID 	1
        #define DIR_ID 	2
        #define BLL_ID 	3
        #define VLO_ID	4
        #define TMR_ID 	5
        #define PLO_ID 	6
        #define FSP_ID 	7
        #define FAT_ID 	8
        #define DSY_ID 	9
        #define DSY3_ID 1	//10
        #define LACA_ID 11
        
        // CMD desde CAN
        #define NO_CMD 		0
        #define RESET 		1
        #define CALIBRAR 	2
        #define DIAGNOSTICO	3
        #define COUNTER_QUERY   4
        #define COUNTERS_RESET	5
        #define SET_LOGIC       6
        #define SET_ID		7

        // Msj Obj en NEWDAT-1
        #define MSGOBJ1 	0x01
        #define MSGOBJ2 	0x02
        #define MSGOBJ3 	0x04
        #define MSGOBJ4 	0x08
        #define MSGOBJ5 	0x10
        #define MSGOBJ3_4 	0x0C    
        #define MSGOBJ2_3_4 	0x0E   
        #define MSGOBJ2_3_4_5 	0x1E   
        
        /***************/
        /** FUNCIONES **/
        /***************/
        /******************************************************/
        /**    BLOQUE DE PROGRAMACION "CAN"                   */
        /******************************************************/
        void canInit( void ); // void start_CAN (void)
        uint8_t canGetModuleAddress( void );
        void canTxData( uint8_t );  // void transmitir_dato (char MsgNum)
        void canRxData( void );  
        void canRxData2 ( void );  //(Marvin)
        void canInitTxMessageObject( uint8_t );  // void init_msg_object_TX (char MsgNum)
        void canInitRxMessageObject( uint8_t );  // void init_msg_object_RX (char MsgNum)
        void canClearAllMessageObject( void );  // void clear_msg_objects (void)
        bool canGetFlagRx( void );

/* End Include Guards */
#endif