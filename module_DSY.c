/******************************************************/
/**                                                   */
/**    ARCHIVO FUENTE "module_DSY.c"                  */
/**    CAPA: ESPECIFICO DEL MODULO DSY                */
/**    FECHA: AGO-2018                                */
/**    VERSION: v1.0                                  */
/**                                                   */
/******************************************************/

/*************/
/** INCLUDE **/
/*************/
/* Archivo de cabecera */
#include <module_DSY.h>
#include <SafetyBus.h>
#include <adc.h>
#include <Supervisor.h>


/************/
/** DEFINE **/
/************/


/***************/
/** VARIABLES **/
/***************/

uint8_t Tx_trama[16] = {0};  //variable donde se arma la trama a envíar para cada módulo.


struct Parametros_ Param = {10000, 0, 10000, 0, 10000, 0, 10000, 0, 10000, 0, 10000, 0, 10000, 0, 10000, 0,
			    10000, 0, 10000, 0, 10000, 0, 10000, 0, 10000, 0, 10000, 0, 10000, 0, 10000, 0,
                            0x0E, 0};

struct Analog_  In1 = {0, 0, 0, 0, 0,0};
bool estadoCalibracion;

/*, Out11 = {0, 0, 0, 0, 0}, Out12 = {0, 0, 0, 0, 0}, Out13 = {0, 0, 0, 0, 0};
struct Analog_  In2 = {0, 0, 0, 0, 0}, Out21 = {0, 0, 0, 0, 0}, Out22 = {0, 0, 0, 0, 0}, Out23 = {0, 0, 0, 0, 0};
struct Analog_  In3 = {0, 0, 0, 0, 0}, Out31 = {0, 0, 0, 0, 0}, Out32 = {0, 0, 0, 0, 0}, Out33 = {0, 0, 0, 0, 0};
struct Analog_  In4 = {0, 0, 0, 0, 0}, Out41 = {0, 0, 0, 0, 0}, Out42 = {0, 0, 0, 0, 0}, Out43 = {0, 0, 0, 0, 0};

struct OutDatos OutCh1 = {0,0,0,0,0,0,0,0,0,0}; 
struct OutDatos OutCh2 = {0,0,0,0,0,0,0,0,0,0}; 
struct OutDatos OutCh3 = {0,0,0,0,0,0,0,0,0,0}; 
struct OutDatos OutCh4 = {0,0,0,0,0,0,0,0,0,0}; 

*/
/***************/
/** FUNCIONES **/
/***************/

void modulePortInit( void )  
{
   	uint8_t SFRPAGE_SAVE = SFRPAGE;       
	
	SFRPAGE = CONFIG_PAGE;
	
	Param.Tipo = TIPMOD;

	XBR0 = 0x04;	// Habilitar SPI (P0.0,P0.1,P0.2)
        XBR1 = 0x00;
	XBR2 = 0x40;     // Enable Crossbar/low ports - Enable Weak Pull-Up
	XBR3 = 0x80;     // Configure CAN TX pin (CTX) as push-pull digital output
	

	P0MDOUT = 0xFC; // 1011-1110
  	P0 = 0xFF;  
        
        P1MDOUT = 0xFF; // 1111-1111
  	P1 = 0xFF;  
        
        P2MDOUT = 0xBF;	//bf 1011-1111
	P2 = 0x7F;
        
        P3MDOUT = 0xE0;//1110-0000				
  	P3 = 0xFF;
        
        P4MDOUT = 0xC0; //1100-0000		     	
  	P4 = 0xFF;
       
        P5MDOUT = 0xFF;		     	
  	P5 = 0xFF;

  	P6MDOUT = 0xFF;				 
  	P6 = 0xFF;
        
        P7MDOUT = 0xFF; // 1111-1111
  	P7 = 0xFF;
	
        
	SFRPAGE = SFRPAGE_SAVE;
        
}



//------------------------------------ external_osc --------------------------------------
// Configuración del oscilador externo
//----------------------------------------------------------------------------------------
void external_osc (void)
{
  	int n;                        
  	char SFRPAGE_SAVE = SFRPAGE;  

	SFRPAGE = CONFIG_PAGE;        // selecciono config_page para configurar el oscilador
	OSCXCN  = 0x67;               // inicio oscilador externo; cristal de 22.1MHz 
                                      //(XOSCMD2-0 = 110 = Crystall Oscillator Mode)(XFCN2-0 = 111 = 10MHz<f<=30MHz))
	for (n=0;n<255;n++);          // delay de 1ms
	while ((OSCXCN & 0x80) == 0); // espero a que el oscilador se estabilize (XTLVLD == 1)
	CLKSEL |= 0x01;               // selecciono oscilador externo 
   
        SFRPAGE = SFRPAGE_SAVE;
}



//-------------------------------------- wait_us -----------------------------------------
//	Delay en us
//----------------------------------------------------------------------------------------
void wait_us (void)
{
	char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
	
	SFRPAGE = TMR3_PAGE;
	
	TMR3CN = 0x00;                      // Stop Timer3; Clear TF3;
	TMR3CF = 0x08;                      // use SYSCLK as timebase
	
	RCAP3L = T_MUESTREOL;//-((SYSCLK/1000 * us)/1000);     // Timer 3 overflows at 130us
	RCAP3H = T_MUESTREOH;
	TMR3L =  T_MUESTREOL;
	TMR3H =  T_MUESTREOH;
	
	EIE2 &= 0xFE;                       // Disable Timer 3 interrupts
	TMR3CN_bit.TOG3 = 1; // TR3 = 1;                            // Start Timer 3
	TMR3CN_bit.TF3 = 0; // TF3 = 0;
	
	SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}

//-------------------------------------- wait_ms -----------------------------------------
//	Delay en ms - Se usa el timer 2 
//  Mirar el flags if (TF2 == 1) y luego parar el timer "TMR2CN_bit.TOG2 = 0;"
//  Maximo tiempo a contar 35 ms
//----------------------------------------------------------------------------------------
void wait_ms (int ms)
{
    char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
    unsigned int aux;

    SFRPAGE = TMR2_PAGE;

    TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
    TMR2CF = 0x00;                      // use SYSCLK/12 as timebase

    aux = -(SYSCLK/1000/12 * ms);     // Timer 2 overflows at 1 kHz
    RCAP2L = aux;
    RCAP2H = (aux >> 8);
    TMR2L = RCAP2L;
    TMR2H = RCAP2H;

    IE_bit.ET2 = 0;                            // Disable Timer 2 interrupts
    TMR2CN_bit.TOG2 = 1; // TR2                            // Start Timer 2
    
    TMR2CN_bit.TF2 = 0;

    SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}

//------------------------------------ Tx_Build --------------------------------------
// Preparación de la trama para 1 o 2 paquetes (según el módulo) para enviar por CAN
//----------------------------------------------------------------------------------------
uint8_t * TxBuild ()
{
  	//configuro error de canal, fuera de rango y primeros 4 bits de Dato In1
	Tx_trama[2] = 0x00; //Ver con Emanuel que clase de errores se consideran de "Errores de CANAL 1" VEEEER!!!!
	Tx_trama[2] |= 0x00 << 4; //Fuera de rango
        Tx_trama[2] |= (In1.Dato >> 8) & 0x0F;  //Primeros 4 bits del Dato In1
	//Ultimos 8 bits de Dato In1
	Tx_trama[3] = In1.Dato;
	//Discrepancia con salidas de entradas 1 y 2
	Tx_trama[4] = 0x00;
       	Tx_trama[5] = 0x00;
       //configuro error de canal, fuera de rango y primeros 4 bits de Dato In2
	Tx_trama[6] = 0x00; //Ver con Emanuel que clase de errores se consideran de "Errores de CANAL 2" VEEEER!!!!
	//Ultimos 8 bits de Dato In2
	Tx_trama[7] = 0x00;
	
	
	return ( Tx_trama );
}



//------------------------------------ Tx_Build --------------------------------------
// Preparación de la trama para 1 para enviar por CAN ante la solicitud de calibración
//----------------------------------------------------------------------------------------
uint8_t* TxBuildCalib (uint8_t CH, uint8_t paquete)
{
  	//int CS,CH;
       
	/*
        if (paquete == 1)
        {
            Tx_trama[2] = 0x00;
      	    Tx_trama[2] = CALIBRAR; //envío comando respuesta de calibración
           
              Tx_trama[3] = 0x00;
              Tx_trama[3] |= 0x10 & 0xF0; //CS4 CH1 (In Ch1)
              Tx_trama[3] |= 0x00 & 0x0F; //CS1 CH1 (Out1 Ch1)
              Tx_trama[4] = (In1.valorCalib >> 8) & 0x0F;  //MSB Valor In CH1
              Tx_trama[5] = In1.valorCalib;       //LSB Valor In CH1
              Tx_trama[6] = 0x00;
              Tx_trama[7] = 0x00;
               
              
        }*/
             if (paquete == 1)
        {
            Tx_trama[2] = 0x00;
      	    Tx_trama[2] = CALIBRAR; //envío comando respuesta de calibración
            switch (CH)
            {
            case 1:
              Tx_trama[3] = 0x00;
              Tx_trama[3] |= 0xD0 & 0xF0; //CS4 CH1 (In Ch1)
              Tx_trama[3] |= 0x01 & 0x0F; //CS1 CH1 (Out1 Ch1)
              Tx_trama[4] =  (In1.valorCalib >> 8) & 0x0F;  //MSB Valor In CH1
              Tx_trama[5] = In1.valorCalib;       //LSB Valor In CH1
              Tx_trama[6] =  0x00;
              Tx_trama[7] =  0x00;
              break;
             case 2:
              Tx_trama[3] = 0x00;
              Tx_trama[3] |= 0xE0 & 0xF0; //CS4 CH2 (In Ch2)
              Tx_trama[3] |= 0x02 & 0x0F; //CS1 CH2 (Out1 Ch2)
              Tx_trama[4] =  0x00;
              Tx_trama[5] =  0x00;
              Tx_trama[6] =  0x00;
              Tx_trama[7] =  0x00;
              break;
             case 3:
              Tx_trama[3] = 0x00;
              Tx_trama[3] |= 0xF0 & 0xF0; //CS4 CH3 (In Ch3)
              Tx_trama[3] |= 0x03 & 0x0F; //CS1 CH3 (Out1 Ch3)
             Tx_trama[4] =  0x00;
              Tx_trama[5] =  0x00;
              Tx_trama[6] =  0x00;
              Tx_trama[7] =  0x00;
              break;
             case 4:
              Tx_trama[3] = 0x00;
              Tx_trama[3] |= 0xC0 & 0xF0; //CS4 CH4 (In Ch4)
              Tx_trama[3] |= 0x00 & 0x0F; //CS1 CH4 (Out1 Ch4)
            Tx_trama[4] =  0x00;
              Tx_trama[5] =  0x00;
              Tx_trama[6] =  0x00;
              Tx_trama[7] =  0x00;
              break; 
            default :
              //error maquina de estados;
              break;
            } 
        }
        else if (paquete == 2)
          
        {   Tx_trama[10] = 0x00;
      	    Tx_trama[10] = CALIBRAR; //envío comando respuesta de calibración
            switch (CH)
            {
            case 1:
              Tx_trama[11] = 0x00;
              Tx_trama[11] |= 0x50 & 0xF0; //CS2 CH1 (Out2 Ch1)
              Tx_trama[11] |= 0x09 & 0x0F; //CS3 CH1 (Out3 Ch1)
              Tx_trama[12] = 0x00;
              Tx_trama[13] = 0x00;
              Tx_trama[14] =  0x00;
              Tx_trama[15] =  0x00;
              break;
             case 2:
              Tx_trama[11] = 0x00;
              Tx_trama[11] |= 0x60 & 0xF0; //CS2 CH2 (Out2 Ch2)
              Tx_trama[11] |= 0x0A & 0x0F; //CS3 CH2 (Out3 Ch2)
              Tx_trama[12] =  0x00;
              Tx_trama[13] =  0x00;
              Tx_trama[14] =  0x00;
              Tx_trama[15] =  0x00;
              break;
             case 3:
              Tx_trama[11] = 0x00;
              Tx_trama[11] |= 0x70 & 0xF0; //CS2 CH3 (Out2 Ch3)
              Tx_trama[11] |= 0x0B & 0x0F; //CS3 CH3 (Out3 Ch3)
              Tx_trama[12] =  0x00;
              Tx_trama[13] =  0x00;
              Tx_trama[14] =  0x00;
              Tx_trama[15] =  0x00;
              break;
             case 4:
              Tx_trama[11] = 0x00;
              Tx_trama[11] |= 0x40 & 0xF0; //CS2 CH4 (Out2 Ch4)
              Tx_trama[11] |= 0x08 & 0x0F; //CS3 CH4 (Out3 Ch4)
              Tx_trama[12] =  0x00;
              Tx_trama[13] =  0x00;
              Tx_trama[14] =  0x00;
              Tx_trama[15] =  0x00;
              break; 
            default :
              //error maquina de estados;
              break;
            }
        }
	return (Tx_trama);
}


//------------------------------------ Tx_Build Calib Response --------------------------------------
// Confirmación de calibración
//----------------------------------------------------------------------------------------
uint8_t * TxBuildCalibResponse ()
{
              //int CS,CH;
              Tx_trama[2] = 0x00;
              Tx_trama[2] = CALIBRAR; //envío comando respuesta de calibración
                
              Tx_trama[3] = 0x00;
              Tx_trama[4] = 0x00;
              Tx_trama[5] = 0x00;
              Tx_trama[6] = 0x00;
              Tx_trama[7] = 0x00;
             
          
	return ( Tx_trama );
}


//------------------------------------ RXCanCalib ----------------------------------------
// Iniciar proceso de calibracion (guardo los valores de calibracion recibido en el módulo)
//----------------------------------------------------------------------------------------
 void ConfigCalib(void)    
{
	
	switch( CAN_DatoRX[3] & 0x0F )
	{
	case 0:
                Param.Ch4_Out1_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
                Param.Ch4_Out1_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
 		break;
	case 1:
                Param.Ch1_Out1_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
                Param.Ch1_Out1_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 2:
                Param.Ch2_Out1_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
                Param.Ch2_Out1_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 3:
                Param.Ch3_Out1_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
                Param.Ch3_Out1_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 4:
                Param.Ch4_Out2_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
                Param.Ch4_Out2_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
 		break;
	case 5:
		Param.Ch1_Out2_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch1_Out2_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 6:
		Param.Ch2_Out2_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch2_Out2_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 7:
		Param.Ch3_Out2_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch3_Out2_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
        case 8:
                Param.Ch4_Out3_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
                Param.Ch4_Out3_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
 		break;
	case 9:
		Param.Ch1_Out3_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch1_Out3_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 10:
		Param.Ch2_Out3_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch2_Out3_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 11:
		Param.Ch3_Out3_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch3_Out3_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
        case 12:
                Param.Ch4_In_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch4_In_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
 		break;
	case 13:
		Param.Ch1_In_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch1_In_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 14:
		Param.Ch2_In_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch2_In_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
		break;
	case 15:
		Param.Ch3_In_pend = (CAN_DatoRX[4] << 8) | (CAN_DatoRX[5]);
		Param.Ch3_In_ord = (CAN_DatoRX[6] << 8) | (CAN_DatoRX[7]);
                safetyBusTxFrameData( TxBuildCalibResponse() , 3 ); //envio los datos
                safetyBusTxFrameData( TxBuildCalibResponse() , 4 ); //envio los datos 
                if ( configurationSaveData() == false ) //si guardado de parametros en memoria EEPROM es correcto
                {
               CalibOK = CAN_DatoRX[3]; //guardo que ADC se calibró exitosamente
               estadoCalibracion = 1;
                 }
                else
                {
                estadoCalibracion = 0;
                }
		break;
	}
	
	
        
		
}
