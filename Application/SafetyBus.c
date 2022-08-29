/******************************************************/
/**                                                   */
/**    ARCHIVO FUENTE "SafetyBus.c"                   */
/**    BLOQUE DE PROGRAMACION "SAFETYBUS"             */
/**    CAPA: APPLICATION                              */
/**    FECHA: MAY-2018                                */
/**    VERSION: v1.0                                  */
/**                                                   */
/******************************************************/

/*************/
/** INCLUDE **/
/*************/
/* Archivo de cabecera */
#include <SafetyBus.h>
#include <adcExt.h>
#include <configuration.h>

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
/**    BLOQUE DE PROGRAMACION "SAFETYBUS"             */
/******************************************************/
void safetyBusInitMaster ( void )
{
  
        canClearAllMessageObject();
        canInitTxMessageObject( 0x01 );
        canInitRxMessageObject( 0x02 );
        canInitRxMessageObject( 0x03 );
        canInitRxMessageObject( 0x04 );
        canInitRxMessageObject( 0x05 );
        canInit();
	dirmod = 0;
	EIE2=0x20;
       
                
}

void safetyBusInitSlave ( void )
{
        
	
        canClearAllMessageObject();
        canInitRxMessageObject( 0x01 );
        canInitTxMessageObject( 0x02 );
        canInitTxMessageObject( 0x03 );
        canInitTxMessageObject( 0x04 );
        canInitTxMessageObject( 0x05 );
        canInit();
	dirmod = canGetModuleAddress();
        EIE2=0x20;
                
}




//------------------------------------ TXCanDatos ----------------------------------------
// Enviar valores actuales de las varibles del módulo (uno o dos paquetes)
//----------------------------------------------------------------------------------------
void safetyBusTxFrameData(uint8_t* Tx, uint8_t msgNum)  
{
	// Armar Respuesta CAN
        switch (msgNum)
        {   
	case 1:
          CAN_DatoTX[0] = dirmod & 0x3F;
          break;
        case 2:
          CAN_DatoTX[0] = dirmod & 0x3F;
          break;
        case 3:
          CAN_DatoTX[0] = (dirmod & 0x3F) | 0x40;
          break;
        case 4:
          CAN_DatoTX[0] = (dirmod & 0x3F) | 0xC0;
          break;
	}
        
	CAN_DatoTX[1] = verificador + (Param.Tipo & 0x1F);

	if ( faultFlags.Err_Discrep || faultFlags.Err_V_NIM || faultFlags.Err_EEPROM || faultFlags.Err_DI ) 
	{
	        CAN_DatoTX[1] |= 0x80;
	}
	
	CAN_DatoTX[2] = 0;
	CAN_DatoTX[2] = Tx[2]; 
	
	CAN_DatoTX[3] = 0;
	CAN_DatoTX[3] = Tx[3];
	
	CAN_DatoTX[4] = 0;
	CAN_DatoTX[4] = Tx[4]; 
	
	CAN_DatoTX[5] = 0;
	CAN_DatoTX[5] = Tx[5]; 
	
	CAN_DatoTX[6] = 0;
	CAN_DatoTX[6] = Tx[6];
        
	CAN_DatoTX[7] = 0;
	CAN_DatoTX[7] = Tx[7];
	
	// Enviar Respuesta CAN
        canTxData( msgNum );
}

//---------------------------------- TXCanDiagnostico ------------------------------------
// Enviar valores de diagnostico del módulo
//----------------------------------------------------------------------------------------

// ***  AUTO DIAGNOSTIC TEST  ***//
void TXCanDiagnostico(void)
{
	// Armar Respuesta CAN
  	CAN_DatoTX[0] = 0;
	CAN_DatoTX[0] = dirmod;  //aca tiene que ir 0 o 5 para el tipo de msj???
	
       	CAN_DatoTX[1] = 0;
        
    	// CAN_DatoTX[1] = verificador + (Param.Tipo & 0x1F);
        // --- JEZ ---
        CAN_DatoTX[1] = (verificador & 0x60) | (Param.Tipo & 0x1F);

        if (faultFlags.Err_Discrep || faultFlags.Err_V_NIM || faultFlags.Err_EEPROM || faultFlags.Err_DI) 
        {
            CAN_DatoTX[1] |= 0x80;
        }

	// Errores Comunes a todos los Módu                                                                                                                                                                 los
	CAN_DatoTX[2] = 0;
	if(ContErr_Discrep > 3) CAN_DatoTX[2] |= 0x03;    // DISC
	else CAN_DatoTX[2] |= ContErr_Discrep;
	if(V_NIM_OK_Cont > 3) CAN_DatoTX[2] |= 0x0C;	  // NIM
	else CAN_DatoTX[2] |= (V_NIM_OK_Cont & 0x03) << 2;
	if(ContErr_EEPROM > 3) CAN_DatoTX[2] |= 0x30;	  // EEPR
	else CAN_DatoTX[2] |= (ContErr_EEPROM & 0x03) << 4;
	
	CAN_DatoTX[3] = 0;
	if(faultPartialCounter.DI > 3) CAN_DatoTX[3] |= 0x0C;	// DI
	else CAN_DatoTX[3] |= (faultPartialCounter.DI & 0x03) << 2;
	if(faultPartialCounter.AI > 3) CAN_DatoTX[3] |= 0x30;	// tADC
	else CAN_DatoTX[3] |= (faultPartialCounter.AI & 0x03) << 4;
	if(faultPartialCounter.CAN > 3) CAN_DatoTX[3] |= 0xC0;	// CAN
	else CAN_DatoTX[3] |= (faultPartialCounter.CAN & 0x03) << 6;

	CAN_DatoTX[4] = 0;
	if((faultPartialCounter.CRCParam + faultPartialCounter.MaqEst) > 3) CAN_DatoTX[4] |= 0x03;	//RAM
	else CAN_DatoTX[4] |= (faultPartialCounter.CRCParam + faultPartialCounter.MaqEst);

                // Fin calibracion OK
	if (CalibOK != 0)
	{
		CAN_DatoTX[4] |= CalibOK << 2;  //Aviso de que se calibro correctamente el canal X
		CalibOK = 0;
	}        
        
        //Faltan los errores propios del modulo que se desean mandar, si así lo requiere en CAN_DatoTX[5], CAN_DatoTX[6] y CAN_DatoTX[7]
        // Errores Específicos del Módulo
	CAN_DatoTX[7] = 0;

	// Enviar Primera Respuesta CAN
        canTxData( 0x02 );
}



//---------------------------------- procesarCAN ------------------------------------
// Enviar valores de diagnostico del módulo
//----------------------------------------------------------------------------------------
  void procesarCAN( uint8_t* Tx, uint8_t cantPaquetes)  // *** SAFETYBUS ***
  {
    	  uint16_t CRC_Rx;
          uint8_t i;
          
          IE_bit.EA = 0;
    
	    // CMD desde CAN
	  
                  CRC_Rx = (unsigned int) (CAN_DatoRX[6] << 8) + CAN_DatoRX[7];
            //     if( processCodeRunCRC( (uint8_t *) CAN_DatoRX, 6) == CRC_Rx )
            //      {
		    switch ( CAN_DatoRX[2] )
		    {
                    case 0:
			  {
			      if (cantPaquetes == 1)
			      {
                                  Tx = TxBuild ();
				  safetyBusTxFrameData(Tx,2);
			      }    
			      if (cantPaquetes == 2)
			      {
                                  Tx = TxBuild ();
				  safetyBusTxFrameData(Tx,3);
				  safetyBusTxFrameData(Tx+8,4); 
			      }  
                              
			  break;
			  }
		    case RESET:
                          {
                                  ResetSup = 1;
                                  Tx = TxBuild ();
                                  safetyBusTxFrameData(Tx,2);
                          break;
                          }
		    case CALIBRAR:
                          {
                                  if( ((CAN_DatoRX[3] >> 4) & 0x0F) == 0x0A )
                                  {
                                      
                                      
                                      
                                      Tx = TxBuildCalib ( CAN_DatoRX[3] & 0x0F , 1 );
                                      safetyBusTxFrameData( Tx , 3 ); //envio los datos
                                      
                                      
                                      Tx = TxBuildCalib ( CAN_DatoRX[3] & 0x0F , 2 );
                                      safetyBusTxFrameData( Tx+8 , 4 ); //envio los datos 
                                      
                                    } 
                                  
                     
                                  if( ((CAN_DatoRX[3] >> 4) & 0x0F) == 0x0B )
                                  {
                                    
                                     ConfigCalib();
                                    
                                      //if( estadoCalibracion == 1 )
                                      //{
                                              // Confirmación de calibración
                                              Tx = TxBuildCalibResponse();
                                              safetyBusTxFrameData( Tx , 3 ); //envio los datos
                                              safetyBusTxFrameData( Tx , 4 ); //envio los datos 
                                      //}
			    	  	    //  safetyBusConfigCalib();
			    	  	
                                  }   
                                  
                          break;
                                  
                          }
		    case SET_LOGIC:
                          {
                            ConfigCalib();
   //                               safetyBusTxFrameData();
   //                               safetyBusConfiguration();
			    break;
                          }
		    case SET_ID:
                          {
                            ConfigCalib();
   //                               safetyBusTxFrameData();
    //                              safetyBusGetModuleType();
                          	break;
			  }
		    case DIAGNOSTICO:
                          {
                                  TXCanDiagnostico();
                          	break;
			  }
		    
                          
              //    }
	  	}
          
          for(i=0;i<16;i++) CAN_DatoRX[i] = 0;
          CAN_tx = 0;
          IE_bit.EA = 1;
  }
