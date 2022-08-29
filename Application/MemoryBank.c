/******************************************************/
/**                                                   */
/**    ARCHIVO FUENTE "MemoryBank.c"                  */
/**    BLOQUE DE PROGRAMACION "CONFIGURATION"         */
/**    BLOQUE DE PROGRAMACION "FAULT"                 */
/**    CAPA: APPLICATION                              */
/**    FECHA: ABR-2018                                */
/**    VERSION: v1.0                                  */
/**                                                   */
/******************************************************/

/*************/
/** INCLUDE **/
/*************/
/* Archivo de cabecera */
#include <MemoryBank.h>


/************/
/** DEFINE **/
/************/
#define memoryPageLimit								0x3C

/***************/
/** VARIABLES **/
/***************/
bool configurationErrorRW;
bool faultErrorRW;



// Variables de errores y de configurción

uint8_t V_NIM_OK_Cont = 0, Inicio_uC = 1, ContErr_EEPROM = 0, ContErr_Discrep = 0;

struct memoryBankFaultCounters faultTotalCounter, faultPartialCounter;
struct FaultFlags faultFlags;
uint8_t microOK;
struct Parametros_ ParamV;
uint8_t * pointerStruct =  (uint8_t *) &Param;
uint8_t * pointerStructV=  (uint8_t *) &ParamV;
uint8_t sizeOfParam = sizeof(Param);
uint8_t sizeOfParamV = sizeof(ParamV);
bool checkCRC;

uint8_t * pointerStructTotal =  (uint8_t *) &faultTotalCounter;
uint8_t * pointerStructPartial =  (uint8_t *) &faultPartialCounter;
uint8_t sizeOfTotalFault = sizeof(faultTotalCounter);
uint8_t sizeOfPartialFault = sizeof(faultPartialCounter);


/***************/
/** FUNCIONES **/
/***************/
void memoryBankInit( void )
{
        i2cInit();
}


/******************************************************/
/**    BLOQUE DE PROGRAMACION "CONFIGURATION"         */
/******************************************************/

//LEER CONFIGURACIÓN DE MEMORIA

bool configurationGetData( void )    
{
	//Chequea el tamanio del paquete de datos de configuracion y lo lee desde la memoria EEPROM
	if (sizeOfParam <= memoryPageLimit) 
	{	
	  	configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStruct , sizeOfParam );
		
	}
	else if ((sizeOfParam > memoryPageLimit) && (sizeOfParam < (2*memoryPageLimit))) 
  	{
        	configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStruct , memoryPageLimit );
        	configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x01U , 0x00U , (pointerStruct + memoryPageLimit) , ( sizeOfParam - memoryPageLimit) );  
  	}
  	else
	{
	configurationErrorRW = true;
	}
	  
	//Verifica que el paquete sea válido
	
      	checkCRC = (Param.CRC == processCodeRunCRC( pointerStruct , ( sizeOfParam - 2 ) ));
           
        if ( (checkCRC) && (configurationErrorRW == false) )
        {	
	  	  // La lectura fue correcta
    		  faultFlags.Err_EEPROM = 0;
                  ContErr_EEPROM = 0;
        }
        else
        {
                /* Fallo primera lectura, reintentar una vez mas */
                // Reiniciar I2C
                IE_bit.EA = 0;		// Desabilita interrupcion global
                /* Reinicio Driver I2C */
                i2cInit();                      // Configure and enable SMBus
                IE_bit.EA = 1;		// Habilita interrupcion global
   
		//Chequea el tamanio del paquete de datos de configuracion y lo lee desde la memoria EEPROM nuevamente
		if (sizeOfParam <= memoryPageLimit)
		{	
	  		configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStruct , sizeOfParam );
		
		}
		else if ((sizeOfParam > memoryPageLimit) && (sizeOfParam < (2*memoryPageLimit)))
  		{	
        		configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStruct , memoryPageLimit );
        		configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x01U , 0x00U , (pointerStruct + memoryPageLimit) , ( sizeOfParam - memoryPageLimit) );  
  		}
  		else
		{
		configurationErrorRW = true;
		}
           	
		//Verifica que el paquete sea válido
       	 	
		checkCRC = (Param.CRC == processCodeRunCRC( pointerStruct , (sizeOfParam - 2)));
		if ((checkCRC) && (configurationErrorRW == false))
                { 
                        faultFlags.Err_EEPROM = 0;
                        ContErr_EEPROM = 0;                        
		}
                else
                {
		  	/* Fallo primera lectura, reintentar una vez mas */
                	// Reiniciar I2C
                	IE_bit.EA = 0;		// Desabilita interrupcion global
                	/* Reinicio Driver I2C */
                	i2cInit();                      // Configure and enable SMBus
                	IE_bit.EA = 1;		// Habilita interrupcion global
   
			//Chequea el tamanio del paquete de datos de configuracion y lo lee desde la memoria EEPROM nuevamente
			if (sizeOfParam <= memoryPageLimit)
			{	
	  			configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStruct , sizeOfParam );
		
			}
			else if ((sizeOfParam > memoryPageLimit) && (sizeOfParam < (2*memoryPageLimit)))
  			{	
        			configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStruct , memoryPageLimit );
        			configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x01U , 0x00U , (pointerStruct + memoryPageLimit) , ( sizeOfParam - memoryPageLimit) );  
  			}
  			else
			{
			configurationErrorRW = true;
			}
           	
			//Verifica que el paquete sea válido
       	 	
			checkCRC = (Param.CRC == processCodeRunCRC( pointerStruct , (sizeOfParam - 2)));
			if ((checkCRC) && (configurationErrorRW == false))
               		{ 
                        	faultFlags.Err_EEPROM = 0;
                        	ContErr_EEPROM = 0;                        
			}
               		else
                	{       
                          
                          Param.Ch1_In_pend = 10000;  // pendiente en cuentas
                          Param.Ch1_In_ord = 0;  // ordenada en cuentas
                          Param.Ch2_In_pend = 10000; // pendiente en cuentas
                          Param.Ch2_In_ord = 0;  // ordenada en cuentas
                          Param.Ch3_In_pend = 10000; // pendiente en cuentas
                          Param.Ch3_In_ord = 0;  // ordenada en cuentas
                          Param.Ch4_In_pend = 10000; // pendiente en cuentas
                          Param.Ch4_In_ord = 0;  // ordenada en cuentas
                          Param.Ch1_Out1_pend = 10000; // pendiente en cuentas
                          Param.Ch1_Out1_ord = 0;  // ordenada en cuentas
                          Param.Ch2_Out1_pend = 10000; // pendiente en cuentas
                          Param.Ch2_Out1_ord = 0;  // ordenada en cuentas
                          Param.Ch3_Out1_pend = 10000; // pendiente en cuentas
                          Param.Ch3_Out1_ord = 0;  // ordenada en cuentas
                          Param.Ch4_Out1_pend = 10000; // pendiente en cuentas
                          Param.Ch4_Out1_ord = 0;  // ordenada en cuentas
                          Param.Ch1_Out2_pend = 10000; // pendiente en cuentas
                          Param.Ch1_Out2_ord = 0;  // ordenada en cuentas
                          Param.Ch2_Out2_pend = 10000; // pendiente en cuentas
                          Param.Ch2_Out2_ord = 0;  // ordenada en cuentas
                          Param.Ch3_Out2_pend = 10000; // pendiente en cuentas
                          Param.Ch3_Out2_ord = 0;  // ordenada en cuentas
                          Param.Ch4_Out2_pend = 10000; // pendiente en cuentas
                          Param.Ch4_Out2_ord = 0;  // ordenada en cuentas
                          Param.Ch1_Out3_pend = 10000; // pendiente en cuentas
                          Param.Ch1_Out3_ord = 0;  // ordenada en cuentas
                          Param.Ch2_Out3_pend = 10000; // pendiente en cuentas
                          Param.Ch2_Out3_ord = 0;  // ordenada en cuentas
                          Param.Ch3_Out3_pend = 10000; // pendiente en cuentas
                          Param.Ch3_Out3_ord = 0;  // ordenada en cuentas
                          Param.Ch4_Out3_pend = 10000; // pendiente en cuentas
                          Param.Ch4_Out3_ord = 0;  // ordenada en cuentas
                          Param.Tipo = TIPMOD;  
                          Param.CRC = processCodeRunCRC ((unsigned char *) &Param, sizeof(Param)-2);
                                
                        	faultFlags.Err_EEPROM = 1;
                        	ContErr_EEPROM++;
                        	configurationErrorRW = true; 
		  	}
		}
	}
        
        
        return configurationErrorRW;
}

//GUARDAR CONFIGURACIÓN EN MEMORIA

bool configurationSaveData( void )   
{  	
  	//Genera el CRC de validación de datos
        
  	Param.CRC = processCodeRunCRC( pointerStruct , (sizeOfParam - 2) );
	
	//Chequea el tamanio del paquete de datos de configuracion y lo escribe en memoria EEPROM
	
	if (sizeOfParam <= memoryPageLimit)
		{	
	  		configurationErrorRW = i2cWriteDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStruct , sizeOfParam );
		}
	else if ((sizeOfParam > memoryPageLimit) && (sizeOfParam < (2*memoryPageLimit)))
  		{ 		
		  	configurationErrorRW = i2cWriteDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStruct , memoryPageLimit );
        		configurationErrorRW = i2cWriteDataArray( EEPROM_CALIBRATIONS , 0x01U , 0x00U , (pointerStruct + memoryPageLimit) , ( sizeOfParam - memoryPageLimit) );  
  		}
     	else
		{
		configurationErrorRW = true;
		}
	
        if( configurationErrorRW == false )
        {
                // Verificar escritura utilizando variable ParamV

                if (sizeOfParamV <= memoryPageLimit)
		{	
	  		configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStructV , sizeOfParamV );
		
		}
		else if ((sizeOfParamV > memoryPageLimit) && (sizeOfParamV < (2*memoryPageLimit)))
  		{
	  		// Lee los valores parametros de calibracion de la memoria externa "Configuration"
        		configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x00U , 0x00U , pointerStructV , memoryPageLimit );
        		configurationErrorRW = i2cReadDataArray( EEPROM_CALIBRATIONS , 0x01U , 0x00U , (pointerStructV + memoryPageLimit) , ( sizeOfParamV - memoryPageLimit) );  
  		}
                 
		//Verifica que el paquete sea válido
               	checkCRC = (ParamV.CRC == processCodeRunCRC( pointerStructV , (sizeOfParamV - 2) ));
                
		if( (checkCRC) && (configurationErrorRW == false) )
                {
                        faultFlags.Err_EEPROM = 0;
                        ContErr_EEPROM = 0;
			
			if (ParamV.CRC != Param.CRC)
			{
				faultFlags.Err_EEPROM = 1;
				ContErr_EEPROM++;
				
                       		configurationErrorRW = true;
			  
			}
	        }
                else
                {
                        faultFlags.Err_EEPROM = 1;
                        ContErr_EEPROM++;
                        configurationErrorRW = true;
                        
                }
        }
        else
        {
                faultFlags.Err_EEPROM = 1;
                ContErr_EEPROM++;
                
                configurationErrorRW = true;
        }
        return configurationErrorRW;
}


bool configurationTestData( void )   
{
        // Error en CS de parametros -> Restear micro
	if( ( Param.CRC != processCodeRunCRC( pointerStruct , ( sizeOfParam - 2 ) ) ) && ( faultFlags.Err_EEPROM == 0 ) )
        {
                /* RESET MICRO CAPA AUTODIAGNOSTIC */
                /* Bucle infinito - Técnica reset watch dog externo */
                /***** RESET MICRO ****/
                while( 1 );
                /***** RESET MICRO ****/
        }
               
        
        return false;
}


/******************************************************/
/**    BLOQUE DE PROGRAMACION "FAULT"                 */
/******************************************************/
bool faultSaveAddData( uint8_t tipo )  
{
  	// Acumulador de errores con reset
        if( faultPartialCounter.CRC == processCodeRunCRC( pointerStructPartial , ( sizeOfPartialFault - 2 ) ) )
        {
                switch( tipo )
                {
                        case ERR_DI:
                        {
                                faultPartialCounter.DI++;
                                break;
                        }
                        case ERR_AI:
                        {
                                faultPartialCounter.AI++;
                                break;
                        }
                        case ERR_CAN:
                        {  
                                faultPartialCounter.CAN++;
                                break;
                        }
                        case ERR_CRC_PARAM:
                        {
                                faultPartialCounter.CRCParam++;
                                break;
                        }
                        case ERR_MAQ_EST:
                        {
                                faultPartialCounter.MaqEst++;
                                break;
                        }
                        case ERR_ADC:
                        {
                                faultPartialCounter.ADC++;
                                break;
                        }
                        case ERR_DAC:
                        {
                                faultPartialCounter.DAC++;
                                break;
                        }
                }

                if((tipo != ERR_DISCREP) && (tipo != ERR_V_NIM)) 
                {
                        faultPartialCounter.Totales++;
                }

                faultPartialCounter.CRC = processCodeRunCRC( pointerStructPartial , ( sizeOfPartialFault - 2 ) );

		if( (tipo != ERR_DISCREP) && (tipo != ERR_V_NIM) )
                        {
                                faultErrorRW = i2cWriteDataArray( EEPROM_FAILURES, 0x00U, 0x00U, pointerStructPartial, sizeOfPartialFault); //se agregó un parámetro a la función Agosto 2017
                        }

        }
        else
        {
                // Si hay error de CS resetear todo
                faultResetAllData();
        }

        // Acumulador de errores (sin reset)
        if( faultTotalCounter.CRC == processCodeRunCRC( pointerStructTotal , (sizeOfTotalFault - 2 ) ) )
        {
                if( (tipo != ERR_DISCREP) && (tipo != ERR_V_NIM) )
                {
                        faultTotalCounter.Totales++;
                }

                switch( tipo )
                {
                        case ERR_DISCREP:
                        {
                                faultTotalCounter.Discrep++;
                                break;
                        }      
                        case ERR_V_NIM:
                        {
                                faultTotalCounter.V_NIM++;
                                break;
                        }
                        case ERR_DI:
                        {
                                faultTotalCounter.DI++;
                                break;
                        }
                        case ERR_AI:
                        {
                                faultTotalCounter.AI++;
                                break;
                        }
                        case ERR_CAN:
                        {
                                faultTotalCounter.CAN++;
                                break;
                        }
                        case ERR_CRC_PARAM:
                        {
                                faultTotalCounter.CRCParam++;
                                break;
                        }
                        case ERR_MAQ_EST:
                        {
                                faultTotalCounter.MaqEst++;
                                break;
                        }
                        case ERR_ADC:
                        {
                                faultTotalCounter.ADC++;
                                break;
                        }
                        case ERR_DAC:
                        {
                                faultTotalCounter.DAC++;
                                break;
                        }
                }

                faultTotalCounter.CRC = processCodeRunCRC( pointerStructTotal , ( sizeOfTotalFault - 2 ) );
              

                        if( (tipo != ERR_DISCREP) && (tipo != ERR_V_NIM))
                        {
                                faultErrorRW = i2cWriteDataArray( EEPROM_FAILURES , 0x01U , 0x00U , pointerStructTotal , sizeOfTotalFault );
                        }

              

        }
        else
        {
                // Si hay error de CS resetear todo
                faultTotalCounter.Totales = 0;
                faultTotalCounter.Discrep = 0;	// Falla discrepancia en Capa Dura
                faultTotalCounter.V_NIM = 0;		// Falla Tensiones NIM
                faultTotalCounter.DI = 0;			// Falla en timer 2
                faultTotalCounter.AI = 0;			// Falla en timer 3
                faultTotalCounter.CAN = 0;		// No se estan recibiendo paquetes CAN en XX mseg
                faultTotalCounter.CRCParam = 0;	// Error en CRC de parametros de calibracion
                faultTotalCounter.MaqEst = 0;		// Estado de maquinas de estado no valido
                faultTotalCounter.ADC = 0;		// Falla en lectura de referencia externa
                faultTotalCounter.DAC = 0;		// Falla en lectura de retorno DAC

                
                faultTotalCounter.CRC = processCodeRunCRC(pointerStructTotal, ( sizeOfTotalFault-2));

			faultErrorRW = i2cWriteDataArray( EEPROM_FAILURES , 0x01U , 0x00U , pointerStructTotal ,  sizeOfTotalFault ); //se agregó un parámetro a la función Agosto 2017

       }

        
        return faultErrorRW;
}


bool faultResetAllData( void )   
{
	faultPartialCounter.Totales = 0;
        faultPartialCounter.Discrep = 0;	// Falla discrepancia en Capa Dura
        faultPartialCounter.V_NIM = 0;		// Falla Tensiones NIM
        faultPartialCounter.DI = 0;			// Falla en timer 2
        faultPartialCounter.AI = 0;			// Falla en timer 3
        faultPartialCounter.CAN = 0;		// No se estan recibiendo paquetes CAN en XX mseg
        faultPartialCounter.CRCParam = 0;	// Error en CRC de parametros de calibracion
        faultPartialCounter.MaqEst = 0;		// Estado de maquinas de estado no valido
        faultPartialCounter.ADC = 0;		// Falla en lectura de referencia externa
        faultPartialCounter.DAC = 0;		// Falla en lectura de retorno DAC

        
        faultPartialCounter.CRC = processCodeRunCRC( pointerStructPartial , ( sizeOfPartialFault - 2 ) );
          
        faultErrorRW = i2cWriteDataArray( EEPROM_FAILURES, 0x00U, 0x00U, pointerStructPartial, sizeOfPartialFault);

                
                
        return faultErrorRW;
                
}

bool faultGetData (void)
{
        //Lee y almacena la memoria de fallas en la estructura pasada por el parámetro
        faultErrorRW= ( i2cReadDataArray( EEPROM_FAILURES , 0x01U , 0x00U , pointerStructTotal , sizeOfTotalFault ) );
	faultErrorRW= ( i2cReadDataArray( EEPROM_FAILURES , 0x00U , 0x00U , pointerStructPartial , sizeOfPartialFault ) );
        return faultErrorRW;
        
}

