#include <I2C.h>
#include <module_DSY.h>
#include <MemoryBank.h>
#include <SafetyBus.h>
#include <adc.h>
#include <can.h>
#include <Supervisor.h>
#include <intrinsics.h> 

uint8_t* trama = Tx_trama; //Variable donde se carga la trama a ser enviada por CAN
bool ErrorRW;
int16_t Dato_old,  Res;
int main( void )
{	 
  	// Disable watchdog timer
        
        WDTCN=0xDE;
        WDTCN=0xAD;
	SFRPAGE = CONFIG_PAGE; 
        //Configuration
   
        external_osc();			// Configuro el oscilador externo
        modulePortInit();		// Inicializo la configuración de puertos de entrada salida
        memoryBankInit();		// Inicializo el protocolo I2C
        safetyBusInitSlave ();		//Inicializo la comunicación CAN
        //configurationSaveData();
       
         ssd1963_init();
       	__no_operation(); __no_operation(); __no_operation(); __no_operation(); __no_operation();
	__no_operation(); __no_operation(); __no_operation(); __no_operation(); __no_operation();
	__no_operation(); __no_operation(); __no_operation(); __no_operation(); __no_operation();
        __no_operation(); __no_operation(); __no_operation(); __no_operation(); __no_operation();
	__no_operation(); __no_operation(); __no_operation(); __no_operation(); __no_operation();
	__no_operation(); __no_operation(); __no_operation(); __no_operation(); __no_operation();

        
       
        
        //fondo_Test();
        
        // LIN
        //fondo_LIN_0_100();
        //fondo_LIN_0_10();            
        //fondo_LIN_0_800();//fondo_LIN_0_600();
        //fondo_LIN_0_15();            //
        
        //fondo_LIN_0_100_C();           //Medicion 1   
        //fondo_LIN_0_100_CCC();  //fondo_LIN_0_100_CC();        //Medicion 11
        
        //diferentes_fuentes();
        //fondo_LOG_1_100();
        //fondo_LOG_0_5();

        //fondo_RATE_3_7();
        
        //fondo_dosis();
        
        //test();
        microOK = 0xFF;
       
         configurationGetData();
         // Iniciar Timer2
//	  wait_ms (15);
          ad_init (); 
          InitAnalog(&In1);
        
          IE_bit.EA = 1;		//habilito interrupcion global
          Inicio_uC = 0;
              {
        if ( P4_bit.P45 == 0  && P4_bit.P44 == 0 && P4_bit.P43 == 0 && P4_bit.P42 == 0)
        {
fondo_LIN_0_100();
        }
        if ( P4_bit.P45 == 1  && P4_bit.P44 == 0 && P4_bit.P43 == 0 && P4_bit.P42 == 0)
        {
fondo_LIN_0_10(); 
        }
        if ( P4_bit.P45 == 0  && P4_bit.P44 == 1 && P4_bit.P43 == 0 && P4_bit.P42 == 0)
        {
fondo_LIN_0_800();
        }
        if ( P4_bit.P45 == 1  && P4_bit.P44 == 1 && P4_bit.P43 == 0 && P4_bit.P42 == 0)
        {
fondo_LIN_0_15();
        }
        if ( P4_bit.P45 == 0  && P4_bit.P44 == 0 && P4_bit.P43 == 1 && P4_bit.P42 == 0)
        {
fondo_LIN_0_100_C();
        }
        if ( P4_bit.P45 == 1  && P4_bit.P44 == 0 && P4_bit.P43 == 1 && P4_bit.P42 == 0)
        { 
fondo_LIN_0_100_CCC();
        }
        if ( P4_bit.P45 == 0  && P4_bit.P44 == 1 && P4_bit.P43 == 1 && P4_bit.P42 == 0)
        {
fondo_LOG_1_100();
        }
        if ( P4_bit.P45 == 1  && P4_bit.P44 == 1 && P4_bit.P43 == 1 && P4_bit.P42 == 0)
        {
fondo_LOG_0_5();
        }
        if ( P4_bit.P45 == 0  && P4_bit.P44 == 0 && P4_bit.P43 == 0 && P4_bit.P42 == 1)
        {
fondo_RATE_3_7();
        }
        if ( P4_bit.P45 == 1  && P4_bit.P44 == 0 && P4_bit.P43 == 0 && P4_bit.P42 == 1)
        {
fondo_dosis();
        }
        if ( P4_bit.P45 == 0  && P4_bit.P44 == 1 && P4_bit.P43 == 0 && P4_bit.P42 == 1)
        {
fondo_Test();
        }
        }
         
         
        while (1)
	{ 
          // Alimentar WD
		SFRPAGE = CONFIG_PAGE;
                
               microOK = 0xFF;
               feed_WD(15);	// 250ms
	
		SFRPAGE = TMR2_PAGE;
//		if (TMR2CN_bit.TF2 == 1)
//		{
//			TMR2CN_bit.TOG2 = 0;	//TR2
//			
//                        wait_ms(30);//15

			// Leer ADC
                        SFRPAGE = CONFIG_PAGE;
			Res = lectura (0);
			descarte (&Res, &In1.Cuentas_Ant, &In1.Cont);

			In1.Acum += (long) Res;
			In1.Prom ++;
			
			if(In1.Prom == 16)
			{
				In1.valorCalib = In1.Acum >> 4;                                            
				In1.Dato = Calibrar (In1.Acum,  Param.Ch1_In_ord, Param.Ch1_In_pend);
				In1.Acum = 0;
				In1.Prom = 0;
                                
				 //dato_inge = Valor_ing(In1.Dato);
                                //In1.Dato = 10.0;
                              
                              if ( P4_bit.P40 == 1)
                              { 
                                
                                {
if ( P4_bit.P45 == 0 && P4_bit.P44 == 0 && P4_bit.P43 == 0 && P4_bit.P42 == 0)
{
medicion2(In1.Dato);//0-100c  y consola uA
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 1  && P4_bit.P44 == 0 && P4_bit.P43 == 0 && P4_bit.P42 == 0)
{
medicion3(In1.Dato);//0-10c
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 0   && P4_bit.P44 == 1 && P4_bit.P43 == 0 && P4_bit.P42 == 0)
{
medicion4(In1.Dato);//0-800m3h//medicion5(In1.Dato);//0-600m3h
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 1  && P4_bit.P44 == 1 && P4_bit.P43 == 0 && P4_bit.P42 == 0)
{
medicion6(In1.Dato);//0-15kpa
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 0  && P4_bit.P44 == 0 && P4_bit.P43 == 1 && P4_bit.P42 == 0)
{
medicion2(In1.Dato);//0-100c  y consola uA
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 1  && P4_bit.P44 == 0 && P4_bit.P43 == 1 && P4_bit.P42 == 0)
{
medicion11(In1.Dato);//0-100ccc consola cps aranque
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 0  && P4_bit.P44 == 1 && P4_bit.P43 == 1 && P4_bit.P42 == 0)
{
medicion8(In1.Dato);//1-100uA
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 1  && P4_bit.P44 == 1 && P4_bit.P43 == 1 && P4_bit.P42 == 0)
{
medicion7(In1.Dato);//1e0-1e5cps
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 0  && P4_bit.P44 == 0 && P4_bit.P43 == 0 && P4_bit.P42 == 1)
{
medicion9(In1.Dato);//  OJO Cambiar a barra4
barra__4(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 1  && P4_bit.P44 == 0 && P4_bit.P43 == 0 && P4_bit.P42 == 1)
{
medicion10(In1.Dato);//
 barra__3(In1.Dato,Dato_old);
}
if ( P4_bit.P45 == 0  && P4_bit.P44 == 1 && P4_bit.P43 == 0 && P4_bit.P42 == 1)
{
medicion(In1.Dato);//4-20mA calibration
 barra__3(In1.Dato,Dato_old);
}
}
                                
                                
                                
                                
                                
                                
                                //medicion(In1.Dato);//4-20mA calibration
                                
                                //Lin
                                //medicion2(In1.Dato);//0-100c  y consola uA
                                //medicion3(In1.Dato);//0-10c
                                //medicion4(In1.Dato);//0-800m3h//medicion5(In1.Dato);//0-600m3h
                                //medicion6(In1.Dato);//0-15kpa
                                //medicion11(In1.Dato);//0-100ccc consola cps aranque
                                
                                //Log
                                //medicion8(In1.Dato);//1-100uA
                                //medicion7(In1.Dato);//1e0-1e5cps
                               
                                //Rate
                                //medicion9(In1.Dato);//  OJO Cambiar a barra4
                                
                                //Dosis
                                //medicion10(In1.Dato);//
                               
                                                        
                                //barra__3(In1.Dato,Dato_old);
                                //barra__4(In1.Dato,Dato_old);
                                Dato_old=In1.Dato;
                              }    
                               
                              
                        
			}
			
//			// Flag de micro OK para timer 2 (DIn)
			microOK |= 0x01;
//		}

		// Nuevo Paquete CAN
		if ((CAN_tx == 1))
		{
			// Flag de micro OK para CAN
			microOK |= 0x04;
			
			if(Inicio_uC == 0) procesarCAN(trama,1);
		}
	}
          
          
 }
