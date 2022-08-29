/******************************************************/
/**                                                   */
/**    ARCHIVO DE CABECERA "Supervisor.h"             */
/**    BLOQUE DE PROGRAMACION "SUPERVISOR"            */
/**    CAPA: APPLICATION                              */
/**    FECHA: JUN-2018                                */
/**    VERSION: v1.0                                  */
/**                                                   */
/******************************************************/

/* Include Guards */
#ifndef SUPERVISOR_H
#define SUPERVISOR_H

        /*************/
        /** INCLUDE **/
        /*************/
        #include <module_ARI.h>
        #include <MemoryBank.h>
        #include <ProcessCode.h>
        #include <CAN.h>
        #include <SafetyBus.h>
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
        /**    BLOQUE DE PROGRAMACION "SUPERVISOR"             */
        /******************************************************/
bool feed_WD (bool WD, unsigned int temp);
void VerifmicroOK (void);
void incrContErrores (char tipo);
  

/* End Include Guards */
#endif
