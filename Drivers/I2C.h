/********************************************/
/**                                         */
/**    ARCHIVO DE CABECERA "I2C.h"          */
/**    BLOQUE DE PROGRAMACION "I2C"         */
/**    CAPA: DRIVERS                        */
/**    FECHA: FEB-2018                      */
/**    VERSION: v1.0                        */
/**                                         */
/********************************************/

/* Include Guards */
#ifndef I2C_H
#define I2C_H

        /*************/
        /** INCLUDE **/
        /*************/
    	/* Inclusión del Módulo ARI-1 */
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
        /********************************************/
        /**    BLOQUE DE PROGRAMACION "I2C"         */
        /********************************************/
        
        bool i2cInit( void );
        uint8_t i2cReadData( uint8_t );
        uint8_t i2cWriteData( uint8_t );
        bool i2cReadDataArray( uint8_t , uint8_t , uint8_t , uint8_t * , uint8_t );
        bool i2cWriteDataArray( uint8_t , uint8_t , uint8_t , uint8_t * , uint8_t );
 
        
/* End Include Guards */
#endif