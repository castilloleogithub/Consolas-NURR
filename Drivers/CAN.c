  /******************************************************/
  /**                                                   */
  /**    ARCHIVO FUENTE "CAN.c"                         */
  /**    BLOQUE DE PROGRAMACION "CAN"                   */
  /**    CAPA: APPLICATION                              */
  /**    FECHA: FEB-2018                                */
  /**    VERSION: v1.0                                  */
  /**                                                   */
  /******************************************************/
  
  /*************/
  /** INCLUDE **/
  /*************/
  /* Archivo de cabecera */
  
  #include <CAN.h>
  #include <ioC8051f040.h>                          // SFR declarations
  #include "page_defs.h"
  
  
  /************/
  /** DEFINE **/
  /************/
  
  
  /***************/
  /** VARIABLES **/
  /***************/
  
  
  /***************/
  /** FUNCIONES **/
  /***************/
  /******************************************************/
  /**    BLOQUE DE PROGRAMACION "CAN"                   */
  /******************************************************/
  
  char dirmod;                 // Direccion del modulo
  
  unsigned char CAN_DatoRX[16];         //Datos recibidos
  unsigned char CAN_DatoTX[8];          //Datos a enviar
  unsigned char CAN_aux[8];             //Variable auxiliar para limpiar banderas de NEWDAT1
  
  char ResetSup = 0;                    //Solicitud de reseteo de supervisor
  __no_init __bit bool CAN_tx, Preg;            //CAN_tx: Indica que se terminó de recibir un dato Preg: variable utulizada para el display
  unsigned char verificador, msj;        
  unsigned char CantRx;
  unsigned char ReinicioCAN = 0;
  unsigned char cmd_TX, slave_diag, dir_cont;	// Variables usadas solo para display. Usadas tb en CAN.c
  
  //------------------------------------- start_CAN ----------------------------------------
  //Inicializa módulo CAN
  //----------------------------------------------------------------------------------------
  
  void canInit( void ) 
  {  
          /* Calculation of the CAN bit timing :
          
          System clock        f_sys = 24 MHz
          System clock period t_sys = 1/f_sys = 1/24 us.
          CAN time quantum       tq = t_sys (at BRP = 0)
          
          Desired bit rate is 1 MBit/s, desired bit time is 1000 ns.
          Actual bit time = 24 tq = 1000 ns
          Actual bit rate is 1 MBit/s 
          
          CAN bus length = 10 m, with 5 ns/m signal delay time.
          Propagation delay time : 2*(transceiver loop delay + bus line delay) = 400 ns
          (maximum loop delay between CAN nodes)
          
          Prop_Seg = 10 tq = 10/24 us ( >= 400 ns).
          Sync_Seg = 1 tq
          
          Phase_seg1 + Phase_Seg2 = (24-10-1) tq = 13 tq
          Phase_seg1 <= Phase_Seg2,  =>  Phase_seg1 = 6 tq and Phase_Seg2 = 7 tq
          SJW = (min(Phase_Seg1, 4) tq = 4 tq
          
          TSEG1 = (Prop_Seg + Phase_Seg1 - 1) = 10 + 6 - 1 = 15 
          TSEG2 = (Phase_Seg2 - 1)            = 6
          SJW_p = (SJW - 1)                   = 3
          
          Bit Timing Register = BRP = TSEG2*0x1000 + TSEG1*0x0100 + SJW_p*0x0040 +  = 6FC0
          
          Clock tolerance df : ???????????????
          
          A: df < min(Phase_Seg1, Phase_Seg2) / (2 * (13*bit_time - Phase_Seg2))
          B: df < SJW / (20 * bit_time)
          
          A: df < 2/(2*(13*11-3)) = 1/(141-3) = 1/138 = 0.7246%
          B: df < 2/(20*11)                   = 1/110 = 0.9091%
          
          Actual clock tolerance is 0.7246% - 0.5381% = 0.1865% (no problem for quartz)
          */
          char SFRPAGE_SAVE = SFRPAGE;  
                  
          CAN_tx = 0;  
          verificador = 0;
          
          SFRPAGE  = CAN0_PAGE;
          CAN0CN  |= 0x41;       // Configuration Change Enable CCE and INIT
          CAN0ADR  = BITREG;	  // Point to Bit Timing register
          CAN0DATH = 0x5E;       // see above                                         
          CAN0DATL = 0xC0;
          
          // RX-IF2 operation may interrupt TX-IF1 operation                            
          CAN0ADR  = IF2CMDMSK;  // Apunta al Command Mask 2
          CAN0DATL = 0x1F;       // Configura el RX : READ CAN RAM, read data bytes (Bit 7 = 0),
                                 // clr NewDat (Bit 2 = 1),clrIntPnd(Bit 3 = 1) and Data A y B Access (Bits 0 y 1 = 1)
          CAN0CN  |= 0x06;       // Global Int. Enable IE and SIE
          CAN0CN  &= ~0x41;      // Clear CCE and INIT bits, starts CAN state machine
          
          SFRPAGE = SFRPAGE_SAVE;
  }
  
  
  uint8_t canGetModuleAddress( void )
  {
          /* Direccion del modulo */
    	char SFRPAGE_SAVE = SFRPAGE;  
	SFRPAGE = CONFIG_PAGE;
	uint8_t direccionModulo;
        direccionModulo = ( (~P4) & 0x3F );     //Direccion del módulo en el puerto 4
 	SFRPAGE = SFRPAGE_SAVE;
  	return direccionModulo;
  
  }
  
  
  //--------------------------------- init_msg_object_RX -----------------------------------
  //Inicializa mensajes CAN para RX
  //----------------------------------------------------------------------------------------
  
  void canInitRxMessageObject( uint8_t MsgNum )  
  {
          uint8_t SFRPAGE_SAVE = SFRPAGE;  
          
          SFRPAGE  = CAN0_PAGE;
          CAN0ADR  = IF1CMDMSK;  // Selecciono Command Mask 1
          CAN0DATH = 0x00;// Set to WRITE, and alter all Msg Obj except ID MASK
          CAN0DATL = 0xBC;  // and data bits (anteriorment 0xBC)
          
          CAN0ADR  = IF1ARB1;    // Selecciono arbitration1 register
          CAN0DATH = 0x00;  // Set arbitration1 ID to "0"
          CAN0DATL = 0x00;
          
          CAN0ADR  = IF1ARB2;
          
          switch (MsgNum)
          {
            case 1:
                
                  //		CAN0DAT = 0x8001 << 2 = 0x8004;     // Arb2 high byte:Set MsgVal bit, no extended ID (standard),Dir = recieve
                  CAN0DATH = 0x80;
                  CAN0DATL = 0x04;
 
                  break;
            case 2:
                  //		CAN0DAT = 0x82A8 << 2 = 0x8AA0;     // Recibe msj de lógica Rx2
                  CAN0DATH = 0x8A;
                  CAN0DATL = 0xA0;

                  break;
            case 3:
                  //		CAN0DAT = 0x82B0 << 2 = 0x8AC0;     // Recibe 1° mensaje de comparador Rx3
                  CAN0DATH = 0x8A;
                  CAN0DATL = 0xC0;

                  break;
            case 4:
                  //		CAN0DAT = 0x82B8 << 2 = 0x8AE0;     // Recibe 2° mensaje comparador Rx4
                  CAN0DATH = 0x8A;
                  CAN0DATL = 0xE0;

                  break;
            case 5:
                  //		CAN0DAT = 0x82BC << 2 = 0x8AF0;     // Recibe mensaje diagnostico Rx4
                  CAN0DATH = 0x8A;
                  CAN0DATL = 0xF0;

                  break;
          }
          
          CAN0ADR  = IF1MSGC;
          //	CAN0DAT  = 0x0188; // Msg Cntrl: set TXRqst, EoB (End of Buffer) y DLC (Data Lenght Code) en 8
          CAN0DATH = 0x01;
          CAN0DATL = 0x88;	
          
          CAN0ADR  = IF1CMDRQST; // Point to Command Request reg.
          CAN0DATL = MsgNum;     // Select Msg Obj passed into function parameter list
          // --initiates write to Msg Obj
          // 3-6 CAN clock cycles to move IF register contents to the Msg Obj in CAN RAM
          
          SFRPAGE = SFRPAGE_SAVE;
  }
  
  
  //----------------------------------- init_msg_object_TX -------------------------------------
  //	Inicializa mensaje CAN MsgNum para TX
  //----------------------------------------------------------------------------------------
  
  void canInitTxMessageObject( uint8_t MsgNum )  
  {
          uint8_t SFRPAGE_SAVE = SFRPAGE;
          
          SFRPAGE = CAN0_PAGE;
          
          CAN0ADR = IF1CMDMSK;  // Point to Command Mask 1
          CAN0DATH = 0x00;  // Set to WRITE, & alter all Msg Obj except ID MASK bits
          CAN0DATL = 0xB0;    
              
          CAN0ADR  = IF1ARB1;    // Selecciono arbitration1 register
          CAN0DATH = 0x00;  // Set arbitration1 ID to "0"
          CAN0DATL = 0x00;
          
          CAN0ADR = IF1ARB2;
          
          
          switch (MsgNum)
          {
            case 1:
             //		CAN0DAT = 0xA000 + (0x0001 << 2) = 0xA004;     // Arb2 high byte:Set MsgVal bit, no extended ID (standard),Dir = transmit
                  CAN0DATH = 0xA0;
                  CAN0DATL = 0x04;

                  break;
            case 2:
                  //		CAN0DAT = 0xA000 + (0x02A8 << 2) = 0xAAA0;     // Transmite msj de lógica tx2
                  CAN0DATH = 0xAA;
                  CAN0DATL = 0xA0;

                  break;
            case 3:
                  //		CAN0DAT = 0xA000 + (0x02B0 << 2) = 0xAAC0;     // Transmite 1° mensaje de comparador tx3
                  CAN0DATH = 0xAA;
                  CAN0DATL = 0xC0;
      
                  break;
            case 4:
                  //		CAN0DAT = 0xA000 + (0x02B8 << 2) = 0xAAE0;     // Transmite 2° mensaje comparador tx4
                  CAN0DATH = 0xAA;
                  CAN0DATL = 0xE0;
         
                  break;
            case 5:
                  //		CAN0DAT = 0xA000 + (0x02BC << 2) = 0xAAF0;     // Transmite mensaje diagnostico tx5
                  CAN0DATH = 0xAA;
                  CAN0DATL = 0xF0;
               
                  break;
          }
          
          CAN0ADR = IF1MSGC;
          //    CAN0DAT = 0x0188;     // Msg Cntrl: set TXRqst, EoB (End of Buffer) y DLC (Data Lenght Code) en 8
          CAN0DATH = 0x00;
          CAN0DATL = 0x88;
          
          CAN0ADR = IF1CMDRQST; // Command Request
                                // Select Msg Obj passed into function parameter list
                                // --initiates write to Msg Obj
          CAN0DATH = 0x00;
          CAN0DATL = MsgNum;
          
          SFRPAGE = SFRPAGE_SAVE;
  }
  
  //--------------------------------- clear_msg_objects ------------------------------------
  // Limpia los mensajes CAN
  //----------------------------------------------------------------------------------------
  
  void canClearAllMessageObject( void )  // void clear_msg_objects (void)
  {
          int i;
          uint8_t SFRPAGE_SAVE = SFRPAGE;  
          
          SFRPAGE  = CAN0_PAGE;
          
          CAN0ADR  = IF1CMDMSK;    // Point to Command Mask Register 1 0x09
          CAN0DATL = 0xFF;         // Set direction to WRITE all IF registers to Msg Obj
          
          CAN0ADR = IF1ARB1;    // Arbitration1 Register
          CAN0DATH = 0x00;
          CAN0DATL = 0x00;
          
          CAN0ADR = IF1ARB2;
          CAN0DATH = 0x00;
          CAN0DATL = 0x00;
          
          CAN0ADR = IF1MSGC;
          CAN0DATH = 0x00;
          CAN0DATL = 0x00;
          
          for ( i = 1 ; i < 33 ; i++ )
          {
                  CAN0ADR = IF1CMDRQST; // Write blank (reset) IF registers to each msg obj
                  CAN0DATL = i;
          }
          
          SFRPAGE = SFRPAGE_SAVE;
  }
  
  
  //---------------------------------- transmitir_dato -------------------------------------
  //	Rutina de TX de Datos CAN
  //----------------------------------------------------------------------------------------
void canTxData( uint8_t MsgNum )  
  {
          uint8_t i;
          uint8_t SFRPAGE_SAVE = SFRPAGE;  

          SFRPAGE  = CAN0_PAGE;  
          
          CAN0ADR  = IF1CMDMSK;  // Point to Command Mask 1
          //	CAN0DAT  = 0x0087;    
          CAN0DATH = 0x00;    // Configura el TX : WRITE CAN RAM, write data bytes (Bit 7 = 1),
          CAN0DATL = 0x87;    //set TXrqst/NewDat(Bit 2 = 1),  DATA A y B (bits 0 y 1 = 1) 
          
          CAN0ADR  = IF1DATA1;   // Point to 1st byte of Data Field
          CAN0DATH = CAN_DatoTX[1];//txdato2;
          CAN0DATL = CAN_DatoTX[0];//txdato1;  
          
          CAN0ADR  = IF1DATA2;   // Point to 1st byte of Data Field
          CAN0DATH = CAN_DatoTX[3];//txdato4;
          CAN0DATL = CAN_DatoTX[2];//txdato3;
          
          CAN0ADR  = IF1DATB1;   // Point to 1st byte of Data Field
          CAN0DATH = CAN_DatoTX[5];//txdato6;
          CAN0DATL = CAN_DatoTX[4];//txdato5;
          
          CAN0ADR  = IF1DATB2;   // Point to 1st byte of Data Field
          CAN0DATH = CAN_DatoTX[7];//txdato8;
          CAN0DATL = CAN_DatoTX[6];//txdato7;
          
          CAN0ADR  = IF1CMDRQST; // Point to Command Request Reg.
          CAN0DATL = MsgNum;     // Move new data for TX to Msg Obj "MsgNum"
          
          for ( i = 0 ; i < 8 ; i++ )
          {
                  CAN_DatoTX[i] = 0;
          }
          
          SFRPAGE = SFRPAGE_SAVE;
  }              
			  
  
  //-------------------------------------- ISRname -----------------------------------------
  //	Interrupción de CAN 
  //----------------------------------------------------------------------------------------
  #pragma vector=0x9B // interrupt 19
  __interrupt void ISRname (void)
  {
          char status;
          char SFRPAGE_SAVE = SFRPAGE;  
          
          SFRPAGE = CAN0_PAGE;
          status = CAN0STA;
          
          
          
          if ((status&0x10) != 0) //RxOk es 1, interrupción por RX
          {                            
                  CAN0STA = (CAN0STA&0xEF)|0x07;	// Reset RxOk, LEC = 7 : NoChange
                  canRxData();
          }
          
          if ((status&0x08) != 0)	// TxOk es 1, interrupción por TX
          {  
                  CAN0STA = (CAN0STA&0xF7)|0x07;        // Reset TxOk, LEC =7 : NoChange
          }
          
          if (((status&0x07) != 0)&&((status&0x07) != 7))
          {                           // Error interrupt, LEC changed
                  // error handling ? 
                  CAN0STA = CAN0STA|0x07;              // Set LEC to NoChange
          }
          
          SFRPAGE = SFRPAGE_SAVE;
  }
  
  
//------------------------------------ recibir_dato --------------------------------------
  //	Rutina de Rx de Datos CAN hecha por Marvin
  //----------------------------------------------------------------------------------------
  
  void canRxData( void )  // void recibir_dato (void)
  {
          char SFRPAGE_SAVE = SFRPAGE; 
          unsigned char datonuevo_l1;
          uint8_t MsgNum = 0;		//Variable donde se almacenará el tipo de MsjObj recibido
          SFRPAGE  = CAN0_PAGE; 
          CAN0ADR = NEWDAT1;		// Registro new dat para saber cuantos mensajes nuevos llegaron a la RAM 
          datonuevo_l1 = CAN0DATL;
          IE_bit.EA = 0;	 // Apago las interrupciones para guardar el dato 
          
          if ((datonuevo_l1 == MSGOBJ1) || (datonuevo_l1 == MSGOBJ2) || (datonuevo_l1 == MSGOBJ3) ||(datonuevo_l1 == MSGOBJ3_4)
              || (datonuevo_l1 == MSGOBJ5)) 
          {
                  
		  switch (datonuevo_l1)
		      {
			case MSGOBJ1:            // recibio pregunta del maestro (esclavo)
			    MsgNum = 1;
			    break;
			case MSGOBJ2:            // recibio mensaje objeto 1 - Unico paquete
			    MsgNum = 2;
			    break;
			case MSGOBJ3:            // recibio mensaje objeto 3 - 1er paquete de 2
			    MsgNum = 3;
			    break;
			case MSGOBJ3_4:          // recibio mensaje objeto 3 - 1er paquete de 2 (queda el segundo paquete por recibir)
			    MsgNum = 3;
			    break;
			case MSGOBJ5:            // recibio mensaje objeto 5 - Respuesta de esclavo que requiere acción del maestro
			    MsgNum = 5;
			    break;
		      }
            
		  CAN0ADR  = IF2CMDRQST;              // Point to Command Request Reg.
		  CAN0DATL = MsgNum;                  // Move new data for RX from Msg Obj "MsgNum"
		  
		  CAN0ADR = IF2DATA1;                 // Point to 1st byte of Data Field
		  CAN_DatoRX[1] = CAN0DATH;
		  CAN_DatoRX[0] = CAN0DATL;
		  
		  CAN0ADR = IF2DATA2;  
		  CAN_DatoRX[3] = CAN0DATH;
		  CAN_DatoRX[2] = CAN0DATL;
		  
		  CAN0ADR = IF2DATB1;  
		  CAN_DatoRX[5] = CAN0DATH;
		  CAN_DatoRX[4] = CAN0DATL;
		  
		  CAN0ADR = IF2DATB2;  
		  CAN_DatoRX[7] = CAN0DATH;
		  CAN_DatoRX[6] = CAN0DATL;
	
		  CAN_tx = 0;
		  
		  switch (MsgNum)
		      {
			case 1:
			    #ifndef	CAUX2_ESCLAVO
			    
				    if ((CAN_DatoRX[0] & 0x3F) == dirmod) 
				      
			    #endif
				    {
					    verificador = CAN_DatoRX[1] & 0x60;
					    ReinicioCAN = 0;
					    CAN_tx = 1;
					    CantRx = 8;
				    }
			    break;
			case 2:
			    CantRx = 8;
			    break;
			case 3:
			    CantRx = 0;
			    break;
			case 5:
			    CantRx = 8;
			    break;
	 
		      }

          
          }      
          if ( (datonuevo_l1 & 0x08) == MSGOBJ4 )	                    // recibio mensaje 3 - 2° Msg Comparador
          {
                  MsgNum = 0x04;
                  
                  CAN0ADR  = IF2CMDRQST;// Point to Command Request Reg.
                  CAN0DATL = MsgNum;    // Move new data for RX from Msg Obj "MsgNum"
                  
                  CAN0ADR = IF2DATA1;  // Point to 1st byte of Data Field
                  CAN_DatoRX[9] = CAN0DATH;
                  CAN_DatoRX[8] = CAN0DATL;
                  
                  CAN0ADR = IF2DATA2;  
                  CAN_DatoRX[11] = CAN0DATH;
                  CAN_DatoRX[10] = CAN0DATL;
                  
                  CAN0ADR = IF2DATB1;  
                  CAN_DatoRX[13] = CAN0DATH;
                  CAN_DatoRX[12] = CAN0DATL;
                  
                  CAN0ADR = IF2DATB2;  
                  CAN_DatoRX[15] = CAN0DATH;
                  CAN_DatoRX[14] = CAN0DATL;
                  
                 // CAN_tx = 1;
                  CantRx = 16;
          }
          
          if ( (datonuevo_l1 == 0x0E) || (datonuevo_l1 == 0x1E) ) 	//Si se recibieron mas de un mensajes diferentes
          {								//entonces reinicio las banderas de NEWDATA1
                  for(msj=1;msj<5;msj++)
                  {
                          CAN0ADR  = IF2CMDRQST;	// Point to Command Request Reg.
                          CAN0DATL = msj;    		// Move new data for RX from Msg Obj "MsgNum"
                          
                          CAN0ADR = IF2DATA1;  		// Point to 1st byte of Data Field
                          CAN_aux[1] = CAN0DATH;
                          CAN_aux[0] = CAN0DATL;
                          
                          CAN0ADR = IF2DATA2;  
                          CAN_aux[3] = CAN0DATH;
                          CAN_aux[2] = CAN0DATL;
                          
                          CAN0ADR = IF2DATB1;  
                          CAN_aux[5] = CAN0DATH;
                          CAN_aux[4] = CAN0DATL;
                          
                          CAN0ADR = IF2DATB2;
                          CAN_aux[7] = CAN0DATH;
                          CAN_aux[6] = CAN0DATL;
                  }
          }
          
          IE_bit.EA = 1;	
          SFRPAGE = SFRPAGE_SAVE;
  }