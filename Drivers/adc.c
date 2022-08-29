////////////// ADC //////////////////////////////////////
#include <ioC8051f040.h>                          // SFR declarations
#include "page_defs.h"
//#include <General.h> 
#include <adc.h> 
#include <configuration.h>

signed long __xdata AcumCalib;
char Calib = 0;
char __xdata ContCalib = 0, CalibOK = 0;
//-------------------------------------- ad_init -----------------------------------------
//	  INICIO EL ADC
//----------------------------------------------------------------------------------------
void ad_init (void)
{
   	char SFRPAGE_SAVE = SFRPAGE;        
   	SFRPAGE = ADC0_PAGE;

   	ADC0CN = 0x00;                      //inicio conversion con AD0BUSY
	AMX0CF = 0x00;                     	// Selecciono entradas simples
   	
	ADC0CF = 0xF8;     // ADC conversion clock = 2.5MHz
   					                    // PGA ganancia = 1
  
	ADC0CN_bit.AD0EN = 1;

	REF0CN = 0x03;      // Internal Reference
   	SFRPAGE = SFRPAGE_SAVE;
}


//-------------------------------------- lectura -----------------------------------------
//	  LECTURA DEL ADC
//----------------------------------------------------------------------------------------
int lectura (char channel)
{
	uint16_t aux_dato,i;
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = ADC0_PAGE;			                        ///conversiones AD
		
	AMX0SL= channel;//aux_amx2sl;					//selecciono AIN2.0
	
	ADC0CN_bit.AD0INT=0;						//limpio el flag de fin de conversion
	IE_bit.EA=0;
	ADC0CN_bit.AD0BUSY=1;						//inicio la conversion
	while (ADC0CN_bit.AD0INT==0)				        //me quedo esperando hasta que termina la conversion
	{ 
		i+=1;	  
	}

/*	aux_datoh = ADC0H;
	aux_datol = ADC0L;
*/	aux_dato = ADC0H << 8 | ADC0L;

	IE_bit.EA=1;
	
	SFRPAGE = SFRPAGE_SAVE;

	return aux_dato;
}


//------------------------------------- Calibrar -----------------------------------------
//	  Calibracion periodica
//----------------------------------------------------------------------------------------
#define CANT_CUENTAS 4095

signed int Calibrar(signed long Acum, signed int ord, signed int pend)
{
	/*Acum = Acum >> 4;
	Acum = ((Acum - (long) ord)*CANT_CUENTAS)/ (long) pend;
	
	if (Acum < 0) Acum = 0;
	if (Acum > CANT_CUENTAS) Acum = CANT_CUENTAS;

	return Acum;
        */
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

//------------------------------------- CalibInit ----------------------------------------
//	  Iniciar calibracion
//----------------------------------------------------------------------------------------
void CalibInit(char Cal)
{
	ContCalib = 0;
	AcumCalib = 0;
	Calib = 0x0F & Cal;
}

//------------------------------------ Calibracion ---------------------------------------
//	  Calibracion: obtener pendiente y ordenada
//----------------------------------------------------------------------------------------
signed long Calibracion(signed int Valor, signed int ord)
{
	// Promediacion
	AcumCalib += Valor;
	ContCalib++;
	if(ContCalib == 16) 
	{
		Calib = 0;
		AcumCalib = AcumCalib >> 4;
		return (AcumCalib - (long) ord);
	}
	return (0);
}

//-------------------------------------- descarte ----------------------------------------
//	  Descarte de mediones para conversor A/D
//----------------------------------------------------------------------------------------
#define UMBRAL		5 		// Ventana de comparacion +/- 5 cuentas
#define DESC		5		// Cantidad de cuentas a descartar

void descarte (signed int *Res, signed int *Cuentas_Ant, signed char *Cont)
{
	if((*Res) & 0x1000) (*Res) |= 0xF000;

	if(((*Res) < ((*Cuentas_Ant) - UMBRAL)) && ((*Cont) > -DESC))
	{
		(*Res) = (*Cuentas_Ant);
		(*Cont)--;
    	
	    if((*Cont) > 0) (*Cont) = 0;
    	return;
	}
  
	if (((*Res) > ((*Cuentas_Ant) + UMBRAL)) && ((*Cont) < DESC))
	{
    		(*Res) = (*Cuentas_Ant);
	    	(*Cont)++;
    
    		if((*Cont) < 0) (*Cont) = 0;
	    	return;
	}
  
	(*Cont) = 0;
	(*Cuentas_Ant) = (*Res);
}

void InitAnalog(struct Analog_ *pAIn)
{
	pAIn->Dato = -1;
	pAIn->Acum = 0;
	pAIn->Prom = 0;
	pAIn->Cont = 0;
        pAIn->valorCalib = 0;
	pAIn->Cuentas_Ant = 0;
}