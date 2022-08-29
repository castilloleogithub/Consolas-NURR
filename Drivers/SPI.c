
#include <SPI.h>
#include <adcExt.h>


//-----------------------------------------------------------------------------
// SPI0_Init
//-----------------------------------------------------------------------------
void SPI0_Init(void)
{
	unsigned char SFRPAGE_SAVE = SFRPAGE; // Save the current SFRPAGE
	
	SFRPAGE = CONFIG_PAGE;

	CS1 = 0;
	CS2 = 0;
	CS3 = 0;
        CS4 = 0;

	EN1 = 1;
	EN2 = 1;
	EN3 = 1;
        EN4 = 1;

	SFRPAGE = SPI0_PAGE;                  // Switch to the necessary SFRPAGE

	SPI0CFG = 0x40;                      // Operate in MASTER mode
                                         // CKPHA = '0', CKPOL = '0'
	// SPI clock frequency equation from the datasheet
	SPI0CKR   = (SYSCLK/(2*SPI_CLOCK))-1;

	SPI0CN = 0x01;                        // 3-wire , SPI enabled

	//EIE1 |= 0x01;                       // Enable SPI interrupts

	//EIP1 |= 0x01;                       // High priority SPI interrupts

	SFRPAGE = SFRPAGE_SAVE;               // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// vEnviarDatosSPI
//-----------------------------------------------------------------------------
void vEnviarDatosSPI(unsigned char CS){
	// Demora total 26 us
	// Tiempo de hold = 6 us (Entre q baja CS y aparece el primer pulso de CLK)

	unsigned char save_sfrpage = SFRPAGE;
	unsigned char i, j;
 	
	SFRPAGE = CONFIG_PAGE;
	switch (CS)
	{
		case 1:
                        EN1 = 0;
			CS1 = 1;
        	break;
		case 2:
			EN2 = 0;
			CS2 = 1;
		break;
		case 3:
			EN3 = 0;
			CS3 = 1;
		break;
                case 4:
			EN4 = 0;
			CS4 = 1;
		break;
	}
	
	SFRPAGE   = SPI0_PAGE;
	for (i=0; i<10; i++); // Tiempo de hold

	// Envia por SPI
	for ( i = 0 ; i < 2 ; i++ )
	{
		SPI0CN_bit.SPIF = 0;   //  Clear the SPI intr. flag

		SPI0DAT  = ~SPI_DatoTX[i];

		for (j=0; j<40; j++); // Wait for end of transfer
	
		if (SPI0CN_bit.WCOL == 1)
   		{
    		// Write collision occurred
			SPI0CN_bit.WCOL = 0;                        // Clear the Write collision flag
		} else if (SPI0CN_bit.RXOVRN == 1)
   		{
			// Receive overrun occurred
	   		SPI0CN_bit.RXOVRN = 0;                      // Clear the Receive Overrun flag
   		} else
   		{
    		// SPIF caused the interrupt

		SPI_DatoRX[i] = ~SPI0DAT;

	}
	    SPI0CN_bit.SPIF = 0;   //  Clear the SPI intr. flag
	}
	
	SFRPAGE = CONFIG_PAGE;
	switch (CS)
	{
		case 1:
			EN1 = 1;
			CS1 = 0;
		break;
		case 2:
			EN2 = 1;
			CS2 = 0;
		break;
		case 3:
			EN3 = 1;
			CS3 = 0;
		break;
                case 4:
			EN4 = 1;
			CS4 = 0;
		break;
	}

	
	SFRPAGE  = save_sfrpage;
}