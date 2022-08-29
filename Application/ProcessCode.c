/********************************************/
/**                                         */
/**    ARCHIVO FUENTE "ProcessCode.c"       */
/**    BLOQUE DE PROGRAMACION "PROCESSCODE" */
/**    CAPA: Application                    */
/**    FECHA: FEB-2018                      */
/**    VERSION: v1.0                        */
/**                                         */
/********************************************/

/*************/
/** INCLUDE **/
/*************/
/* Archivo de cabecera */
#include <ProcessCode.h>



/************/
/** DEFINE **/
/************/


/***************/
/** VARIABLES **/
/***************/


/***************/
/** FUNCIONES **/
/***************/

/********************************************/
/*    BLOQUE DE PROGRAMACION "PROCESSCODE"  */
/********************************************/
uint16_t processCodeRunCRC( uint8_t * buff , uint16_t lenght )  
{
	//++// unsigned int CRCRegFull;
	//++// char index;
	//++// char x;
	//++// unsigned char temp;
        
        uint16_t CRCRegFull;
        uint8_t index;
        uint8_t x;
        uint8_t temp;  // VER ERROR 8 -> 16
        
        // Load the register with 0xFFFF  
        CRCRegFull = 0xFFFF;
        
        for ( index = 0 ; index < lenght ; index++ )
        {
                temp = * buff;
                buff++;
                //++// CRCRegFull ^= (unsigned int)temp;
                CRCRegFull ^= (uint16_t) temp;
                   
                for ( x = 0 ; x < 8 ; x++ )
                {
                        // Evaluate LSB. If it == 1, XOR with 0xA001 
                        if ( ( 1 ) & ( CRCRegFull ) )
                        {
                        // Bit-shift once to the right 
                        CRCRegFull >>= 1;
                        CRCRegFull ^= 0xA001;
                        }
                        else
                        {
                                CRCRegFull >>= 1;
                        }
                }
        }
          
          // Swap the bytes for insertion into the message 
          return ( ( CRCRegFull & 0xFF ) * 256 ) + ( CRCRegFull >> 8 );
}


//-------------------------------------- Leer_DI -----------------------------------------
//	Leer entrada digital con filtro
//----------------------------------------------------------------------------------------
bool processCodeGetDIFilter( bool port , uint8_t * estado , uint8_t * temp )
{
	switch ( * estado )
	{
	        case 0: // Entrada en 1
                {
//    *temp = 0;
		        if ( port == false )
                        {
                                * estado = 1;
                        }
		        
                        return true;
                }
                
	        case 1: // Validando entrada de 1 -> 0 
                {
                  //		(*temp)++;
//		if (*temp >= TEMP_PULSADO)
		
			if ( port == false )
			{
				* estado = 2;
                                
				return false;          
			}
                        else
			{
				* estado = 0;
//				*temp = 0;
                                
			}
		
                
		        return true;
                }
                
                case 2: // Entrada en 0
                {
        //		*temp = 0;
                        if ( port )
                        {
                                * estado = 3;
                        }
                        
                        return false;
                }
                
                case 3: // Validando entrada de 0 -> 1
                {
        //		(*temp)++;
        //		if (*temp >= TEMP_PULSADO)
                        
                        if ( port )
                        {
                                * estado = 0;
                                
                                return true;
                        }
                        else 
                        {
                                * estado = 2;
//				*temp = 0;
                        }
                
                
                        return false;                        
                }
                
                default:
                {
                  /*   CAPA AUTODIAGNOSTICTEST   */
                                // En caso de caer en un estado no valido -> Reset micro
                                
                        //++// incrContErrores (ERR_MAQ_EST);
                        faultSaveAddData( ERR_MAQ_EST );

                        // COMENTADO ver que debería estar en la capa MemoryBank -> FAULT
                        // O CAPA AUTODIAGNOSTICTEST

                        /*
                        // Resetear micro
                        SFRPAGE = LEGACY_PAGE; 
                        RSTSRC |= 0x10;
                        */
                        
                        /* Bucle infinito - Técnica reset watch dog externo */
                        /***** RESET MICRO ****/
                        while( 1 );
                        /***** RESET MICRO ****/
                        
                        
                        
                }
	}
}