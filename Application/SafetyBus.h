/******************************************************/
/**                                                   */
/**    ARCHIVO DE CABECERA "SafetyBus.h"              */
/**    BLOQUE DE PROGRAMACION "SAFETYBUS"             */
/**    CAPA: APPLICATION                              */
/**    FECHA: FEB-2018                                */
/**    VERSION: v1.0                                  */
/**                                                   */
/******************************************************/

/* Include Guards */
#ifndef SAFETYBUS_H
#define SAFETYBUS_H

        /*************/
        /** INCLUDE **/
        /*************/
        #include <CAN.h>
        #include <MemoryBank.h>
 
        
        
        
        /************/
        /** DEFINE **/
        /************/


        // Tipo de equipo
        #ifndef MCM_uC1 // Para MCM no se utiliza ninguna opcion
                #define ESCLAVO  	// Para todos los esclavos (CMP, PLO, VLO, TMR, DIR, BLL, etc)
                // y para CAUX-2 usado con MCM-1
                #ifdef DSY_1
                        #undef ESCLAVO
                        #define DISPLAY	// Solo para display DSY-1 (CAUX-2_Display)
                #endif
        
                #ifdef CAUX_1
                        #undef ESCLAVO
                        #define MAESTRO 	// CAUX-1
                #endif
        
                #ifdef CAUX_2
                        #undef ESCLAVO
                        #define CAUX2_ESCLAVO // Para CAUX-2 usado con MCM-1
                #endif
        #endif


        
        /***************/
        /** VARIABLES **/
        /***************/
        
        
        /***************/
        /** FUNCIONES **/
        /***************/
        /******************************************************/
        /**    BLOQUE DE PROGRAMACION "SAFETYBUS"             */
        /******************************************************/
        void safetyBusInitMaster( void );
        void safetyBusInitSlave( void );
        void safetyBusGetModuleType( void );  
        void safetyBusTxFrameData(uint8_t * Tx, uint8_t msgNum);  //ver si esta bien esto (es posible pasar un puntero de un arreglo y despues variar el offsetr dentro?)
        uint8_t safetyBusRxFrameData( void );
        void procesarCAN(uint8_t* Tx, uint8_t cantPaquetes );
  

/* End Include Guards */
#endif
