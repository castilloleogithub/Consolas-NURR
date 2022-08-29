/********************************************/
/**                                         */
/**    ARCHIVO FUENTE "I2C.c"               */
/**    BLOQUE DE PROGRAMACION "I2C"         */
/**    CAPA: DRIVERS                        */
/**    FECHA: ABR-2018                      */
/**    VERSION: v1.0                        */
/**                                         */
/********************************************/

/*************/
/** INCLUDE **/
/*************/
/* Archivo de cabecera */
#include <I2C.h>


/************/
/** DEFINE **/
/************/
#define         WRITE          0x00             // WRITE direction bit
#define         READ           0x01             // READ direction bit
		
#define         MY_ADDR        0x00             // Address of this SMBus device
                                                // (dummy value since this device does
                                                // not have any defined slave states)
#define         T_BIT           20              // Bit time 10 useg
#define         T_BIT_2         (T_BIT/2)
#define         ACK             0
#define         NACK            1

#define         TIMEOUT_EEPROM  600             // 1ms waiting


/***************/
/** VARIABLES **/
/***************/


/***************/
/** FUNCIONES **/
/***************/

/********************************************/
/**    BLOQUE DE PROGRAMACION "I2C"         */
/********************************************/
/**
* Inicializa el driver I2C y configura los registros internos
* Argumentos
*    void
* Retorno
*    bool: retorna si se falló la incialización del driver deI2C
* Ejemplo de Uso
*    i2cInit();
**/
bool i2cInit(void)   
{
          uint8_t pulses = 0;
          uint16_t waiting;
          
          uint8_t SFRPAGE_SAVE = SFRPAGE;               // Save Current SFR page
          SFRPAGE = CONFIG_PAGE;
            
          // If slave is holding SDA low because of an improper SMBus reset or error
          while( (!SDA) && (pulses < 15) )
          {
                    // Provide clock pulses to allow the slave to advance out
                    // of its current state. This will allow it to release SDA.
                  
                    SCL = 0;                                            // Drive the clock low
                    
                    for( uint8_t i = 0 ; i < 255 ; i++ );               // Hold the clock low
                    
                    SCL = 1;                                            // Release the clock
                      
                    waiting = TIMEOUT_EEPROM;
                    
    
                    while( (!SCL) && (waiting) ) waiting--;   // Wait for open-drain
                    
                    if( waiting == 0 )
                    {
                           
                            return true;
                    }                  
                                                       // clock output to rise
                    for( uint8_t i = 0 ; i < 10 ; i++ );         // Hold the clock high
                    
                    pulses++;
          }
        
          if( pulses == 15 )
          {
            
                    return true;
          }
           
          SCL = 1;
          SDA = 1;
        
          SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
          
          return false;
          
}

/**
* Inicia la comunicación por la interfaz I2C hacia el dispositivo esclavo
* Argumentos
*    uint8_t : Confirmación acknowledge (ACK) que controla la comunicación
* Retorno
*    uint8_t : Dato recibido
* Ejemplo de Uso
*    uint8_t dato;
*    dato = i2cReadData( ack );
**/
uint8_t i2cReadData( uint8_t AssertAck )   // unsigned char I2CRead(unsigned char AssertAck)
{
          //++// unsigned char i, j, Data = 0;
          uint8_t i, j;
          uint8_t Data = 0;
           
          for( i = 0 ; i < 8 ; i++ )
          {
                    SCL = 0;
                    // Data Allowed to Change 
                    for( j = 0 ; j < T_BIT_2 ; j++ ); // 8
                    SDA = 1; // Free line
                    for( j = 0 ; j < T_BIT_2 ; j++ ); // 9
                        
                    SCL = 1;
                    // Read Data Valid from EEPROM
                    if(SDA) Data |= 1;
                    if(i<7) Data <<= 1;
                    for( j = 0 ; j < T_BIT ; j++ ); // 2
          }
            
          SCL = 0;
          // Data Allowed to Change
          // Write Acknowledge - Not Acknowledge
          for( j = 0 ; j < T_BIT_2 ; j++ ); // 8
          
          if( AssertAck == ACK )
          {
                SDA = 0;
          }
          else
          {
                SDA = 1;
          }
          
          for( j = 0 ; j < T_BIT_2 ; j++ ); // 9
          
          // Wait for EEPROM read data
          SCL = 1;
          for( j = 0 ; j < T_BIT ; j++ ); // 2 - 
          SCL = 0;
            
          return Data;
}

uint8_t i2cWriteData( uint8_t Data )    // unsigned char I2CSend(unsigned char Data)
{
          uint8_t j, i, ack_bit;
          
          for( i = 0 ; i < 8 ; i++ )
          {
                    SCL = 0;
                    // Data Allowed to Change - uC changes data
                    for(j=0 ; j<T_BIT_2 ; j++); // 8
                    
                    if ( (Data & 0x80) == 0 )
                    {
                        SDA = 0;
                    }
                    else
                    {
                        SDA = 1;
                    }
                    
                    for(j=0 ; j<T_BIT_2 ; j++); // 9
                      
                    SCL = 1;
                    // Data Valid
                    Data <<= 1;
                    for(j=0 ; j<T_BIT ; j++); // 2 - Wait for EEPROM read data
          }
            
          SCL = 0;
          for(j=0 ; j<T_BIT_2 ; j++); // 8
          SDA = 1;
          for(j=0 ; j<T_BIT_2 ; j++); // 9
            
          // Read Acknowledge
          SCL = 1;
          ack_bit = SDA;
          for(j=0 ; j<T_BIT ; j++); // 2
            
          SCL = 0;
            
          return ack_bit;
}

bool i2cReadDataArray( uint8_t EEPROM_ADDR , uint8_t src_addr_H , uint8_t src_addr_L , uint8_t * dest_addr , uint8_t len )

{
          
          uint8_t SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
          uint8_t i;
          SFRPAGE = CONFIG_PAGE;
         
            
          SCL = 1;
          SDA = 1;
          for(i=0 ; i<T_BIT ; i++); // 7
            
          // START
          SDA = 0;
          for(i=0 ; i<T_BIT ; i++); // 6 - Wait for EEPROM read START
          SCL = 0;
            
          // Control Byte
          if( i2cWriteData( (EEPROM_ADDR) + (MY_ADDR<<1) + (WRITE) ) == 1 )
          {
                return true;
          }
          // Address High Byte
          if( i2cWriteData( src_addr_H ) == 1)
          {
                return true;
            
          }
          // Address Low Byte
          if( i2cWriteData( src_addr_L ) == 1)
          {
                return true;
            
          }
            
          // START
          SCL = 0;
          // Data Allowed to Change
          for(i=0 ; i<T_BIT_2 ; i++); // 8
          SDA = 1;
          for(i=0 ; i<T_BIT_2 ; i++); // 9
          SCL = 1;
          for(i=0 ; i<T_BIT ; i++); // 7
          SDA = 0;
          for(i=0 ; i<T_BIT ; i++); // 6 - Wait for EEPROM read START
          SCL = 0;
            
          // Control Byte
          if( i2cWriteData( (EEPROM_ADDR) + (MY_ADDR<<1) + (READ) ) == 1)
          {
                return true;
          }
            
          for( i = 0 ; i < ( len - 1 ) ; i++ )
          {
            (*dest_addr) = i2cReadData( ACK );
            dest_addr++;
          }
            
          (*dest_addr) = i2cReadData( NACK );
           
          // STOP
          SCL = 0;
          // Data Allowed to Change
          for(i=0 ; i<T_BIT_2 ; i++); // 8
          SDA = 0;
          for(i=0 ; i<T_BIT_2 ; i++); // 9
          SCL = 1;
          for(i=0 ; i<T_BIT ; i++); // 10
          SDA = 1;
          for(i=0 ; i<T_BIT ; i++); // 14
            
          SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
          
          return false;
          
}

bool i2cWriteDataArray( uint8_t EEPROM_ADDR , uint8_t dest_addr_H , uint8_t dest_addr_L , uint8_t * src_addr, uint8_t len )
{
                  
          uint8_t SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
          uint8_t i;
          uint16_t aux;
            
          SFRPAGE = CONFIG_PAGE;
          SCL = 1;
          SDA = 1;
          for( i = 0 ; i < T_BIT ; i++ ); // 7
            
          // START
          SDA = 0;
          for( i = 0 ; i < T_BIT ; i++ ); // 6 - Wait for EEPROM read START
          SCL = 0;
            
          // Control Byte
          if( i2cWriteData( (EEPROM_ADDR) + (MY_ADDR<<1) + (WRITE) ) == 1 )
          {
                return true;
          }
          // Address High Byte
          if( i2cWriteData( dest_addr_H ) == 1 )
          {
                return true;
          }
          // Address Low Byte
          if( i2cWriteData( dest_addr_L ) == 1 )
          {
                return true;
          }
          
          for( i = 0 ; i < len ; i++ )
          {
                i2cWriteData( *src_addr );
                src_addr++;
          }
          
          // STOP
          SCL = 0;
          // Data Allowed to Change
          for( i = 0 ; i < T_BIT_2 ; i++ ); // 8
          SDA = 0;
          for( i = 0 ; i < T_BIT_2 ; i++ ); // 9
          SCL = 1;
          for( i = 0 ; i < T_BIT ; i++ ); // 10
          SDA = 1;
          for( i = 0 ; i < T_BIT ; i++ ); // 14
            
          // Wait for write last data in EEPROM
          for( aux = 0 ; aux < 9000 ; aux++ ); // 3 ms medido con OSC
           
          SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector

  
          return false;
                  
}











