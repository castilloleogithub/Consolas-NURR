/********************************************/
/**                                         */
/**    ARCHIVO DE CEBECERA "ProcessCode.h"  */
/**    BLOQUE DE PROGRAMACION "PROCESSCODE" */
/**    CAPA: Application                    */
/**    FECHA: FEB-2018                      */
/**    VERSION: v1.0                        */
/**                                         */
/********************************************/
/* Include Guards */
#ifndef CRC_H
#define CRC_H


        /*************/
        /** INCLUDE **/
        /*************/
        /* typedef */
        /* uint8_t - uint16_t - uint32_t - uint64_t */
        #include <stdint.h>
        
        /* typedef */
        /* bool - true - false */
        #include <stdbool.h>
        
        #include <MemoryBank.h>
        
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
        
        uint16_t processCodeRunCRC( uint8_t * , uint16_t );

//      bool Leer_DI (bool port, char __xdata *estado, char  __xdata *temp);
        bool processCodeGetDIFilter( bool , uint8_t * , uint8_t * );

        
/* End Include Guards */
#endif