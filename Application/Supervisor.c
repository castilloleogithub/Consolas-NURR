
/*************/
/** INCLUDE **/
/*************/
/* Archivo de cabecera */
#include <Supervisor.h>
#include <configuration.h>

bool VP12_OK, VN12_OK, VP24_OK, VN24_OK;
bool OutOfRange_CH1 = 0, OutOfRange_CH2 = 0, OutOfRange_CH3 = 0, OutOfRange_CH4 = 0;
bool V_NIM_OK;
char EsperaVNIM = 0; // Temporizador para esperar vuelta de tension NIM luego de un corte
char Est_VP12_OK = 0, Est_VN12_OK = 0, Est_VP24_OK = 0, Est_VN24_OK = 0;
char Temp_VP12_OK = 0, Temp_VN12_OK = 0, Temp_VP24_OK = 0, Temp_VN24_OK = 0;

//----------------------------------- Supervisor_Fault ------------------------------------
// 
//----------------------------------------------------------------------------------------
void Supervisor_Fault(void)
{
	static char TempInicio = 0;
	static unsigned char Cont1seg = 0;
	static unsigned int TempContResetErrores = 0;
        

 //SENSAR_VNIM
	VP12_OK = !Leer_DI (portBitGetVP12Nim, &Est_VP12_OK, &Temp_VP12_OK);
       	VN12_OK = !Leer_DI (portBitGetVN12Nim, &Est_VN12_OK, &Temp_VN12_OK);        
	VP24_OK = !Leer_DI (portBitGetVP24Nim, &Est_VP24_OK, &Temp_VP24_OK);
	VN24_OK = !Leer_DI (portBitGetVN24Nim, &Est_VN24_OK, &Temp_VN24_OK);


	if(TempInicio <= 7) // Contar 45 mseg
	{	// Inicio -> Anular transmision CAN y setear/resetear fallas segun corresponda
		Inicio_uC = 1;

		faultFlags.Err_Discrep = 0;
		faultFlags.Err_V_NIM = 0;
		V_NIM_OK_Cont = 0;
		faultFlags.Err_DI = 0;
                faultFlags.Err_AI = 0;
                faultFlags.Err_CAN = 0;
                faultFlags.Err_CRCParam = 0;
                faultFlags.Err_MaqEst = 0;
                ReinicioCAN = 0;

		if((VP12_OK == 1) && (VN12_OK == 1) && (VP24_OK == 1) && (VN24_OK == 1)) V_NIM_OK = 1;
		else V_NIM_OK = 0;


		if(TempInicio == 7)
		{
			Inicio_uC = 0; // Habilitar COM CAN
			
			// Flag de micro OK para CAN
			microOK |= 0x04;

			if(V_NIM_OK == 0) V_NIM_OK_Cont = 1;
			else V_NIM_OK_Cont = 0;

			// Leer acumulador de errores parciales

                        if(i2cReadDataArray( EEPROM_FAILURES , 0x00U , 0x00U ,(uint8_t *) &faultPartialCounter , sizeof (faultPartialCounter) ) == 1)
                        {
                              faultFlags.Err_EEPROM = 1;
                              ContErr_EEPROM++;
                        }

                        faultFlags.Err_EEPROM = 0;
                        ContErr_EEPROM = 0;
	
			if(faultPartialCounter.CRC == processCodeRunCRC ((unsigned char *) &faultPartialCounter, (sizeof(faultPartialCounter)-2)))
			{
				if(faultPartialCounter.DI >= 5) faultFlags.Err_DI = 1;
				else  faultFlags.Err_DI = 0;
				if(faultPartialCounter.AI >= 5) faultFlags.Err_AI = 1;
				else faultFlags.Err_AI = 0;
				if(faultPartialCounter.CAN >= 5) faultFlags.Err_CAN = 1;
				else faultFlags.Err_CAN = 0;
				if(faultPartialCounter.CRCParam >= 5) faultFlags.Err_CRCParam = 1;
				else faultFlags.Err_CRCParam = 0;
				if(faultPartialCounter.MaqEst >= 5) faultFlags.Err_MaqEst = 1;
				else faultFlags.Err_MaqEst = 0;
			} else 
			{
				faultPartialCounter.Totales = 0;
                                faultPartialCounter.Discrep = 0;
                                faultPartialCounter.V_NIM = 0;
                                faultPartialCounter.DI = 0;
                                faultPartialCounter.AI = 0;
                                faultPartialCounter.CAN = 0;
                                faultPartialCounter.CRCParam = 0;
                                faultPartialCounter.MaqEst = 0;
                                faultPartialCounter.ADC = 0;
                                faultPartialCounter.DAC = 0;
                                faultPartialCounter.CRC = processCodeRunCRC ((unsigned char *) &faultPartialCounter, (sizeof(faultPartialCounter)-2));
			}

			// Leer acumulador de errores sin reset

			 if(i2cReadDataArray( EEPROM_FAILURES , 0x01U , 0x00U ,(uint8_t *) &faultTotalCounter , sizeof (faultTotalCounter) ) == 1)
                        {
                                ContErr_EEPROM++;
                        }

                        faultFlags.Err_EEPROM = 0;
                        ContErr_EEPROM = 0;

			if(faultTotalCounter.CRC != processCodeRunCRC ((unsigned char *) &faultTotalCounter, (sizeof(faultTotalCounter)-2)))
			{ // Si hay error de CS resetear todo
				faultTotalCounter.Totales = 0;
				faultTotalCounter.Discrep = 0;
				faultTotalCounter.V_NIM = 0;
				faultTotalCounter.DI = 0;
				faultTotalCounter.AI = 0;
				faultTotalCounter.CAN = 0;
				faultTotalCounter.CRCParam = 0;
				faultTotalCounter.MaqEst = 0;
				faultTotalCounter.ADC = 0;
				faultTotalCounter.DAC = 0;
		
				faultTotalCounter.CRC = processCodeRunCRC ((unsigned char *) &faultTotalCounter, (sizeof(faultTotalCounter)-2));
			}
		}

		TempInicio++;
	} else
	{
		// Reset contadores parciales de errores del módulo por tiempo
		if(Cont1seg == 200)
		{
			if(TempContResetErrores == SP_RESET_CONT_ERRORES)
			{
				faultResetAllData ();
				TempContResetErrores++; // Esto asegura que el reset por tiempo se haga solo una vez

			} else
			{
				TempContResetErrores++;
				Cont1seg = 0;
			}
		}
                
                // Evitar errores de CAN en el inicio por transitorios en red
                if(TempInicio <= 200) // Espera de 1 seg
		{
			// Flag de micro OK para CAN
			microOK |= 0x04;
                        TempInicio++;
                }
	
		// Supervision de tensiones NIM
		if((VP12_OK == 1) && (VN12_OK == 1) && (VP24_OK == 1) && (VN24_OK == 1))
		{
			if(V_NIM_OK == 0)
			{
				// Espera para estabilizacion de tensiones NIM
				if(EsperaVNIM == 0)
				{
					V_NIM_OK = 1; // Esta OK cuando sensa todas las tensiones
					//RetornoVINM();
				} else EsperaVNIM--;
			}
		} else
		{
			if(V_NIM_OK == 1)
			{ // Incrementar contador de Falla en tensiones NIM
				incrContErrores (ERR_V_NIM);
				if(V_NIM_OK_Cont < 3) V_NIM_OK_Cont++;
			}
			EsperaVNIM = 40; // 200 mseg de espera (Verificado con OSC)
			faultFlags.Err_V_NIM = 1; // Queda retenido hasta que viene el reset por CAN
			V_NIM_OK = 0; // Esta OK cuando sensa todas las tensiones
		}

                faultFlags.Err_V_NIM = 0;
                V_NIM_OK_Cont = 0;

	}

	// Reset errores del módulo por CAN
	if(ResetSup == 1)
	{		
		faultFlags.Err_Discrep = 0;
		ContErr_Discrep = 0;


		faultFlags.Err_V_NIM = 0; // Error en tensiones NIM no esta temporizado
		if(V_NIM_OK == 0) V_NIM_OK_Cont = 1;
		else V_NIM_OK_Cont = 0;

                faultFlags.Err_V_NIM = 0;
                V_NIM_OK_Cont = 0;

		faultFlags.Err_DI = 0;
		faultFlags.Err_AI = 0;
		faultFlags.Err_CAN = 0;
                ReinicioCAN = 0;
		faultFlags.Err_CRCParam = 0;
		faultFlags.Err_MaqEst = 0;

		faultResetAllData ();

                if(faultFlags.Err_EEPROM == 1) 
		{
                        // EEPROM -> Reintentar lectura
		        // Este es el unico contador de errores que no se resetea con 
		        // ResetSup, se resetea con lectura OK
		        
                        if(i2cReadDataArray( EEPROM_FAILURES , 0x01U , 0x00U ,(uint8_t *) &faultTotalCounter , sizeof (faultTotalCounter)) == 1)
                        {
                              faultFlags.Err_EEPROM = 1;
                              ContErr_EEPROM++;
                        }
	
                        ResetSup = 0;
	        }
      }
}
//-------------------------------------- feed_WD -----------------------------------------
//	- Alimentar WD Ext
//
//  - Chequea que la CPU haya pasado por diferentes lugares del programa
// Se verifica cada vez que se alimenta el WD
//
// bit0: timer2 - Lectura de entradas digitales -> si falla, resetear micro
// bit1: timer3 - Lectura de entradas analogicas -> si falla, resetear micro
// bit2: CAN -> intertar levantar la COM 3 veces consecutivas, sino resetear micro
//---------------------------------------------------------------------------------------

void feed_WD (unsigned int temp)
{
	static int time = 0;

	char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
	
        SFRPAGE = CONFIG_PAGE;
	time++;
	
	if(time == temp)
	{
        
             

                // Alimentar Watch Dog Externo
	if( portBitGetWDExtern == 1 )
                {
                  portBitSetWDExtern_OFF;
                }
		else
                {
                  portBitSetWDExtern_ON;
                }
                
                
                
		time = 0;
                faultPartialCounter.DI = 0;

		// bit1: timer3 - Lectura de entradas analogicas
		if((microOK & 0x02) != 0x02)
		{ 
			incrContErrores (ERR_AI);

			// Resetear micro
			//SFRPAGE = LEGACY_PAGE; 
			//RSTSRC |= 0x10;
                        while( 1 );
                        
                        
                        
                        
		}
                else
                {
                        faultPartialCounter.AI = 0;
                }

		// bit2: CAN
		if(((microOK & 0x04) != 0x04) && (dirmod <= 36))
		{ 	// Dir de PeM > 36, se adopta 48
			incrContErrores (ERR_CAN);

			// Reinicializar CAN
			if(ReinicioCAN < 3)
			{
				IE_bit.EA = 0;		// Desabilita interrupcion global
				// Iniciar CAN
				SFRPAGE  = CAN0_PAGE;
				CAN0CN  |= 0x41;       // Configuration Change Enable CCE and INIT
                                canInit(); 
				safetyBusInitSlave();
                                ReinicioCAN++;

				IE_bit.EA = 1;		// Habilita interrupcion global
			} else
			{
				// Resetear micro
				//SFRPAGE = LEGACY_PAGE; 
				//RSTSRC |= 0x10;
                                
                                while( 1 );
			}
		}

		
                
                // Verificar errores particulares del módulo, ej Error en Maq de estado
	
                
                //	VerifmicroOK();

		microOK = 0;
	}

	SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
	
}


//------------------------------------ VerifmicroOK --------------------------------------
// Verificar CheckSum de memoria
//----------------------------------------------------------------------------------------
void VerifmicroOK(void)
{
	// Error en CS de parametros -> Restear micro
	if((Param.CRC != processCodeRunCRC ((uint8_t *) &Param, sizeof(Param)-2)) && (faultFlags.Err_EEPROM == 0))
	{
		faultFlags.Err_CRCParam++;
		
		// Resetear micro
		SFRPAGE = LEGACY_PAGE; 
		RSTSRC |= 0x10;
	}
}	


//---------------------------------- incrContErrores -------------------------------------
//	
//----------------------------------------------------------------------------------------
void incrContErrores (char tipo)
{
	// Acumulador de errores con reset
	if(faultPartialCounter.CRC == processCodeRunCRC ((uint8_t *) &faultPartialCounter, (sizeof(faultPartialCounter)-2)))
	{
		switch(tipo)
		{
		case ERR_DI:
			faultPartialCounter.DI++;
			break;
		case ERR_AI:
			faultPartialCounter.AI++;
			break;
		case ERR_CAN:
			faultPartialCounter.CAN++;
			break;
		case ERR_CRC_PARAM:
			faultPartialCounter.CRCParam++;
			break;
		case ERR_MAQ_EST:
			faultPartialCounter.MaqEst++;
			break;
		case ERR_ADC:
			faultPartialCounter.ADC++;
			break;
		case ERR_DAC:
			faultPartialCounter.DAC++;
			break;
		}

		if((tipo != ERR_DISCREP) && (tipo != ERR_V_NIM)) faultPartialCounter.Totales++;

		faultPartialCounter.CRC = processCodeRunCRC ((uint8_t *) &faultPartialCounter, (sizeof(faultPartialCounter)-2));

                if((tipo != ERR_DISCREP) && (tipo != ERR_V_NIM))
		{
			i2cWriteDataArray( EEPROM_FAILURES , 0x00U , 0x00U , (uint8_t *) &faultPartialCounter , sizeof(faultPartialCounter) );
 	        }


	} else
	{ // Si hay error de CS resetear todo
		faultResetAllData ();
	}

	// Acumulador de errores (sin reset)
	if(faultTotalCounter.CRC == processCodeRunCRC ((uint8_t *) &faultTotalCounter, (sizeof(faultTotalCounter)-2)))
	{
		if((tipo != ERR_DISCREP) && (tipo != ERR_V_NIM)) faultTotalCounter.Totales++;

		switch(tipo)
		{
		case ERR_DISCREP:
			faultTotalCounter.Discrep++;
			break;
		case ERR_V_NIM:
			faultTotalCounter.V_NIM++;
			break;
		case ERR_DI:
			faultTotalCounter.DI++;
			break;
		case ERR_AI:
			faultTotalCounter.AI++;
			break;
		case ERR_CAN:
			faultTotalCounter.CAN++;
			break;
		case ERR_CRC_PARAM:
			faultTotalCounter.CRCParam++;
			break;
		case ERR_MAQ_EST:
			faultTotalCounter.MaqEst++;
			break;
		case ERR_ADC:
			faultTotalCounter.ADC++;
			break;
		case ERR_DAC:
			faultTotalCounter.DAC++;
			break;
		}

		faultTotalCounter.CRC = processCodeRunCRC ((uint8_t *) &faultTotalCounter, (sizeof(faultTotalCounter)-2));

		if((tipo != ERR_DISCREP) && (tipo != ERR_V_NIM)) 
                {
                  i2cWriteDataArray( EEPROM_FAILURES , 0x01U , 0x00U , (uint8_t *) &faultTotalCounter , sizeof(faultTotalCounter) );
                }
        } else
	{ // Si hay error de CS resetear todo
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
		
		faultTotalCounter.CRC = processCodeRunCRC ((uint8_t *) &faultTotalCounter, (sizeof(faultTotalCounter)-2));
		i2cWriteDataArray( EEPROM_FAILURES , 0x01U , 0x00U , (uint8_t *) &faultTotalCounter , sizeof(faultTotalCounter) );
	}
}


//-------------------------------------- Leer_DI -----------------------------------------
//	Leer entrada digital con filtro
//----------------------------------------------------------------------------------------
bool Leer_DI (bool port, char *estado, char *temp)
{
	switch (*estado)
	{
	case 0: // Entrada en 1
//    *temp = 0;
		if (port == 0) *estado = 1;
		return 1;
	case 1: // Validando entrada de 1 -> 0 
//		(*temp)++;
//		if (*temp >= TEMP_PULSADO)
		{
			if (port == 0)
			{
				*estado = 2;
				return 0;
			} else
			{
				*estado = 0;
//				*temp = 0;
			}
		}
		return 1;
	case 2: // Entrada en 0
//		*temp = 0;
		if (port) *estado = 3;
		return 0;
	case 3: // Validando entrada de 0 -> 1
//		(*temp)++;
//		if (*temp >= TEMP_PULSADO)
		{
			if (port)
			{
				*estado = 0;
				return 1;
			} else 
			{
				*estado = 2;
//				*temp = 0;
			}
		}
		return 0;
        default:
                // En caso de caer en un estado no valido -> Reset micro
                incrContErrores (ERR_MAQ_EST);

                // Resetear micro
                SFRPAGE = LEGACY_PAGE; 
                RSTSRC |= 0x10;
                break;
	}
        return 0;
}