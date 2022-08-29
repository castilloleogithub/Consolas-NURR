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
        #include <configuration.h>
        #include <SafetyBus.h>

        /************/
        /** DEFINE **/
        /************/
        #define SP_RESET_CONT_ERRORES 10800 // Tiempo = (3*3600*1) seg
        
        /***************/
        /** VARIABLES **/
        /***************/
        extern bool VP12_OK, VN12_OK, VP24_OK, VN24_OK;
        extern bool OutOfRange_CH1, OutOfRange_CH2, OutOfRange_CH3, OutOfRange_CH4;
        extern bool V_NIM_OK;
        extern char Est_VP12_OK , Est_VN12_OK , Est_VP24_OK , Est_VN24_OK;
        extern char Temp_VP12_OK , Temp_VN12_OK , Temp_VP24_OK , Temp_VN24_OK;
        

       
        /***************/
        /** FUNCIONES **/
        /***************/
        /******************************************************/
        /**    BLOQUE DE PROGRAMACION "SUPERVISOR"             */
        /******************************************************/
        void feed_WD ( unsigned int temp);
        void VerifmicroOK (void);
        void incrContErrores (char tipo);
        bool Leer_DI (bool port, char *estado, char *temp);
        void Supervisor_Fault(void);

  

/* End Include Guards */
#endif
