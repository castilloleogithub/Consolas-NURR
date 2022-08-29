
/* Inclusión del Módulo ARI-1 */
#include <configuration.h>

#define PENDIENTE   0
#define OK          1

#define ON  1
#define OFF 0

#define IDLE        00
#define MSJDATOS    01
#define MSJPREGUNTA 02
#define MSJENVIAR   03

#define MSJID_01 1234  // 11 bits ID 0 a 2048
#define MSJID_02 1234  // 11 bits ID 0 a 2048

#define PAQUET_BYTE_SIZE 576
#define PAQUETE_SPI 9 //noe  8

#define SPI_CLOCK       1000000//2000000

extern unsigned char SPI_DatoTX[2];
extern unsigned char SPI_DatoRX[2];

void SPI0_Init(void);
void vEnviarDatosSPI(unsigned char CS);