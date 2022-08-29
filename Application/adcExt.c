/******************************************************/
/**                                                   */
/**    ARCHIVO FUENTE "adcExt.c"                      */
/**    BLOQUE DE PROGRAMACION "ADC Externo"           */
/**    CAPA: DRIVERS                                  */
/**    FECHA: MAY-2018                                */
/**    VERSION: v1.0                                  */
/**    AUTOR: MAP                                     */
/**                                                   */
/******************************************************/

/*************/
/** INCLUDE **/
/*************/
/* Archivos de cabecera */
#include <ioC8051f040.h>                          // SFR declarations
#include "page_defs.h"
#include <spi.h>
#include <adcExt.h>


/***************/
/** VARIABLES **/
/***************/

signed long AcumCalib;
char Calib = 0;
char ContCalib = 0, CalibOK = 0;

unsigned char SPI_DatoTX[2];
unsigned char SPI_DatoRX[2];

/***************/
/** FUNCIONES **/
/***************/
/**********************************************/
/**    BLOQUE DE PROGRAMACION "ADC EXTERNOS"  */
/**********************************************/
//-----------------------------------------------------------------------------
// adc_Init
//-----------------------------------------------------------------------------
/**
* Inicializa el driver SPI y configura los diferentes ADC
* Argumentos
*    CS: Chip select del ADC que se desea inicializar
*    Ref: Valor de rango de tensión en los cuales se configura el ADC
* Retorno
*    void
* Ejemplo de Uso
*    adc_Init(2,REF2_5);
**/

void adc_Init (unsigned char CS, unsigned char Ref)
{
	int i;

	SPI0_Init();
	
	// Range Register
	SPI_DatoTX[0]= 0xA0 | (Ref<<3) | (Ref<<1) | (Ref>>1); 	// Write = 1, RegSelec = 01 (Range Reg)
	SPI_DatoTX[1]= 0x00 | (Ref<<7) | (Ref<<5); 		// VinX = Select combination for +-10V (00) or +-2.5V (10) 
								// or +-5V (01) or 0 to 10V (11)
	
	vEnviarDatosSPI(CS);

	for(i=0;i<100;i++);

	// Sequence Register
	SPI_DatoTX[0]= 0xE0; // Write = 1, RegSelec = 11 (Sequence Reg)
	SPI_DatoTX[1]= 0x00; 
	
	vEnviarDatosSPI(CS);

	for(i=0;i<100;i++);

	// Control Register
	SPI_DatoTX[0]= 0x80; // Write = 1, RegSelec = 00 (Control Reg), Select CH0, Mode = 00 (Single Ended)
	SPI_DatoTX[1]= 0x10; // PM = 00 (Normal Mode), Code = 0 (two complement), Ref = 1 (Internal Reference)
						 // Seq = 00 (Sequencer is not used)
	vEnviarDatosSPI(CS);

}

//-----------------------------------------------------------------------------
// lectura
//-----------------------------------------------------------------------------
/**
* Solicita el valor de conversion al ADC correspondiente y devuelve el valor obtenido
* Argumentos
*    CS: Chip select del ADC que se desea leer
*    CH: Canal del ADC del cual se realizará la conversión A/D
* Retorno
*    int: retorna el valor de cuenta del ADC y del canal seleccionado
* Ejemplo de Uso
*    cuenta = lectura(2,1);  //Valor de ADC con el CS numero 2, canal 1
**/
int lectura (unsigned char CS, unsigned char CH)
{

        // Control Register
	SPI_DatoTX[0]= 0x80 + (CH << 2); // Write = 1, RegSelec = 00 (Control Reg), Select CH del canal selccionado, Mode = 00 (Single Ended)
	SPI_DatoTX[1]= 0x10; // PM = 00 (Normal Mode), Code = 0 (two complement), Ref = 1 (Internal Reference)
						 // Seq = 00 (Sequencer is not used)
	vEnviarDatosSPI( CS );
      //  vEnviarDatosSPI( CS );
     
	CH = SPI_DatoRX[0] >> 6;		//Los 8 bits recibidos del ADC estan desplazados un bit a la izquierda por tema de sincronización
	
	return (((SPI_DatoRX[0] & 0x3F) << 7) + (SPI_DatoRX[1] >> 1));	//Los 8 bits recibidos del ADC estan desplazados un bit a la izquierda por tema de sincronización
}



//------------------------------------- InitAnalog ---------------------------------------
// 
//----------------------------------------------------------------------------------------
void InitAnalog(struct Analog_ *pAIn)
{
	pAIn->Dato = -1;
	pAIn->Acum = 0;
	pAIn->Prom = 0;
	pAIn->Cont = 0;
        pAIn->valorCalib = 0;
	pAIn->Cuentas_Ant = 0;
}



//------------------------------------- ProcAnalog ---------------------------------------
// 
//----------------------------------------------------------------------------------------
void ProcAnalog(struct Analog_ *pAIn, int NEnt, uint16_t *ord, uint16_t *pend)
{
	int16_t Res, Cuentas_Ant;
	signed char Cont = 0x00;
	
	// Leer ADC
        
        Res = lectura ((NEnt/4)+1, NEnt%4); // CS, CH
        
        // En las salidas se miden tensiones negativas
        if((Res > 4095)) 
        {
                Res |= 0xE000;
                Res = 0 - Res;//Complemento a 2 del valor Res para cambiar el signo
        }
		
	// Filtrado y acondicionamiento (descarta los valores que varien respecto al valor anterior)
	
        Cuentas_Ant = pAIn->Cuentas_Ant;
	Cont = pAIn->Cont;
	descarte (&Res, &Cuentas_Ant, &Cont);
	pAIn->Cuentas_Ant = Cuentas_Ant;
	pAIn->Cont = Cont;
	
	//Acumula el valor del ADC para realizar el promedio
	pAIn->Acum += (long) Res;
	//Cantidad de valores para realizar el promedio
	pAIn->Prom++;
	
	if(pAIn->Prom == 16)
	{
            //Ajusto el valor del dato en base a la ordenada y a la pendiente 
            pAIn->valorCalib = pAIn->Acum >> 4; 
            pAIn->Dato = Calibrar (pAIn->Acum, *ord, *pend);
	    pAIn->Acum = 0;
	    pAIn->Prom = 0;
	}
}

//-------------------------------------- descarte ----------------------------------------
//	  Descarte de mediciones para conversor A/D
//----------------------------------------------------------------------------------------
#define UMBRAL		5 		// Ventana de comparacion +/- 5 cuentas
#define DESC		5		// Cantidad de cuentas a descartar

void descarte (int16_t *Res, int16_t *Cuentas_Ant, signed char *Cont)
{
	if((*Res) & 0x1000) (*Res) |= 0xF000;		//Si el valor es negativo, entonces coloco en 1 los que no son dato

	if(((*Res) < ((*Cuentas_Ant) - UMBRAL)) && ((*Cont) > -DESC)) //Verifica que el valor no sea menos de 5 cuentas de el valor anterior
	{
		(*Res) = (*Cuentas_Ant);		//Reseteo el valor y tomo el anterior
		(*Cont)--;				
    	
	        if((*Cont) > 0) (*Cont) = 0;
    	return;
	}
  
	if (((*Res) > ((*Cuentas_Ant) + UMBRAL)) && ((*Cont) < DESC)) //Verifica que el valor no sea mas de 5 cuentas de el valor anterior
	{
    	    (*Res) = (*Cuentas_Ant);			//Reseteo el valor y tomo el anterior
	    (*Cont)++;
    
    	    if((*Cont) < 0) (*Cont) = 0;
	return;
	}
  
	(*Cont) = 0;
	(*Cuentas_Ant) = (*Res);
        return;
}



//------------------------------------- Calibrar -----------------------------------------
//	  Calibracion periodica
//----------------------------------------------------------------------------------------

uint16_t Calibrar(uint16_t Acum, int16_t ord, int16_t pend)
{
        float temp_ord, temp_pend;
        temp_pend = ((float)pend/(float)10000);
        temp_ord = ((float)(ord & 0x7FFF)/(float)1000);
        Acum = Acum >> 4;
        if(ord & 0x8000) 
        {
	   Acum = (uint16_t)((Acum * temp_pend) - temp_ord + (float) 0.5);
	}
        else 
        {
          Acum = (uint16_t)((Acum * temp_pend) + temp_ord +(float) 0.5);
	}
        
      //  if (Acum < 0) Acum = -1;
      //  if (Acum > CANT_CUENTAS) Acum = CANT_CUENTAS + 1;

	return Acum;
}