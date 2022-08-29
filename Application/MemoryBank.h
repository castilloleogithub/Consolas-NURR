/******************************************************/
/**                                                   */
/**    ARCHIVO DE CABECERA "MemoryBank.h"             */
/**    BLOQUE DE PROGRAMACION "CONFIGURATION"         */
/**    BLOQUE DE PROGRAMACION "FAULT"                 */
/**    CAPA: APPLICATION                              */
/**    FECHA: FEB-2018                                */
/**    VERSION: v1.0                                  */
/**                                                   */
/******************************************************/

/* Include Guards */
#ifndef MEMORYBANK_H
#define MEMORYBANK_H

        /*************/
        /** INCLUDE **/
        /*************/
        #include <I2C.h>
       	#include <ProcessCode.h>
                
        /************/
        /** DEFINE **/
        /************/
        /* Dirección hardware memoria de fallas */
        /* 1010 (esclavo) + CS: A0 A1 A2 + R/W  Read=1 Write=0 */
        /* 000 (A2-A1-A0) ==> 0xA0 = 1010 "000"0 */
        #define  EEPROM_FAILURES        0xA0
        
        /* Dirección hardware memoria de configuración */
        /* 1010 (esclavo) + CS: A0 A1 A2 + R/W  Read=1 Write=0 */
        /* 101 (A2-A1-A0) ==> 0xAA = 1010 "101"0 */
        #define  EEPROM_CALIBRATIONS    0xAA
        
        
 /*     #define CALIB_FIN           0
        #define CALIB_CERO_CH1      1
        #define CALIB_PEND_CH1      2
        #define CALIB_CERO_CH2      3
        #define CALIB_PEND_CH2      4
        #define CALIB_CERO_CH3      5
        #define CALIB_PEND_CH3      6
        #define CALIB_CERO_CH4      7
        #define CALIB_PEND_CH4      8
        #define CALIB_CERO_CH5      9
        #define CALIB_PEND_CH5      10
        #define CALIB_CERO_CH6      11
        #define CALIB_PEND_CH6      12     ¡¡¡¡¡¡ VEEER segun lo que requiere el ARI !!!!!!!!!!*/
        
        #define ERR_TOTALES		0
        #define ERR_DISCREP		1
        #define ERR_V_NIM		2
        #define ERR_DI			3
        #define ERR_AI			4
        #define ERR_CAN			5
        #define ERR_CRC_PARAM	        6
        #define ERR_MAQ_EST		7
        #define ERR_ADC			8
        #define ERR_DAC			9

	     
        
        /***************/
        /** VARIABLES **/
        /***************/
    
        //++// char __xdata V_NIM_OK_Cont = 0, Inicio_uC = 1, ContErr_EEPROM = 0, ContErr_Discrep = 0;
        extern uint8_t V_NIM_OK_Cont, Inicio_uC, ContErr_EEPROM, ContErr_Discrep;
        
	//Estructura de bandera de errores en memoria externa de falla
        struct FaultFlags			
        {
                uint8_t Err_Discrep     : 1;
                uint8_t Err_EEPROM      : 1;
                uint8_t Err_V_NIM       : 1;
                uint8_t Err_DI          : 1;
                uint8_t Err_AI          : 1;
                uint8_t Err_CAN         : 1;
                uint8_t Err_CRCParam    : 1;
                uint8_t Err_MaqEst      : 1;
		uint8_t Err_ADC      	: 1;
		uint8_t Err_DAC      	: 1;
	};
        
        extern struct FaultFlags faultFlags;
        
	struct memoryBankFaultCounters {
                uint8_t Totales;	// Falla discrepancia en Capa Dura
                uint8_t Discrep;	// Falla discrepancia en Capa Dura
                uint8_t V_NIM;	        // Falla Tensiones NIM
                uint8_t DI;		// Falla en timer 2
                uint8_t AI;		// Falla en timer 3
                uint8_t CAN;		// No se estan recibiendo paquetes CAN en XX mseg
                uint8_t CRCParam;	// Error en CRC de parametros de calibracion
                uint8_t MaqEst;	        // Estado de maquinas de estado no valido
                uint8_t ADC;		// Falla en lectura de referencia externa
                uint8_t DAC;		// Falla en lectura de retorno DAC
                uint16_t CRC;
        };
        
        
        extern struct memoryBankFaultCounters faultTotalCounter, faultPartialCounter;
         
        
        extern uint8_t microOK;
       
        
      
        
        
        /***************/
        /** FUNCIONES **/
        /***************/
        void memoryBankInit( void );
        
        /******************************************************/
        /**    BLOQUE DE PROGRAMACION "CONFIGURATION"         */
        /******************************************************/
        
        // void LeerCal(void)
        bool configurationGetData( void );
        bool configurationSaveData( void );
        bool configurationTestData( void );
        
        /******************************************************/
        /**    BLOQUE DE PROGRAMACION "FAULT"                 */
        /******************************************************/
        
        // void incrContErrores (char tipo)
        bool faultSaveAddData( uint8_t );
        bool faultResetAllData( void );
        bool faultGetData( void );
        
/* End Include Guards */
#endif
