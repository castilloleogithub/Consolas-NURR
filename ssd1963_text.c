#include "ssd1963.h"
#include <ioC8051f040.h>                          // SFR declarations
#include "page_defs.h"
#include "font.h"
#include "font2.h"
#include "font3.h"
#include "font4.h"
#include "font5.h"
#include "font6.h"
#include "font7.h"
#include "font8.h"
#include "font9.h"
#include "font10.h"
#include "font11.h"
#include <stdint.h>
#include <rgb.h>
static struct {
  /**
   * @brief Text-related internal variables
   */
   struct {      
      uint16_t origin_x; /**< Horizontal position in pixels */
      uint16_t HEIGHT;       /**< Vertical position in pixels */
      uint16_t WIDTH;       /**< Horizontal position in pixels */
      uint16_t HEIGHT2;       /**< Vertical position in pixels */
      uint16_t WIDTH2;       /**< Horizontal position in pixels */
      rgb color;     /**< Text foreground color */
      rgb bgcolor;   /**< Text background color */
      uint8_t scale;    /**< Text scale */
  } text;
}
 _ssd1963 = {
   {  0,
      0,
      0,
      0,
      0,
      {0xff,0xff,0xff},
      {0x00,0x00,0x00},
      1
  }  
};

/* --- Internal variables --------------------------------------------------- */
/**
 * @brief Internal structure to hold display state variables
 */


/* --- Private functions & Macros ------------------------------------------- */
void ssd1963_delay(uint32_t interval)
{
  uint32_t i;  
  for(i=0;i<interval;++i);
}

/**
 * @brief Write command
 * @param cmd D[0:8]
 * @pre dsy3_data_port_set_output_direction
 * @note This is DSY-3 implementation specific
 */
inline void _ssd1963_write_command(uint8_t cmd)
{
    RD = 1;
    CD = 0;
    WR = 0;
    CE = 0;
    PORT=cmd;
    CE = 1;
    WR = 1;
}

/**
 * @brief Write data (8bit)
 * @param data D[0:8]
 * @pre dsy3_data_port_set_output_direction
 * @note This is DSY-3 implementation specific
 */
inline void _ssd1963_write_data8(uint8_t data)
{  P2_bit.P27 = 1;

    RD = 1;
    CD = 1;
    WR = 0;
    CE = 0;    
    PORT=(data);
    CE = 1;
    WR = 1;
    P2_bit.P27 = 0;
 
}

/**
 * @brief Write data (16bit)
 * @param data D[0:8]
 * @pre dsy3_data_port_set_output_direction
 * @note This is DSY-3 implementation specific
 */
inline void _ssd1963_write_data16(uint16_t data)
{
    RD = 1;
    CD = 1;
    WR = 0;
    CE = 0;    
    Data_BUS_16bits=(data);
    CE = 1;
    WR = 1;
}


/**
 * @brief Set RAM display Address
 * @param x0 start column (in pixels)
 * @param x1 end column (in pixels)
 * @param y0 start page (in pixels)
 * @param y1 end page (in pixels)
 * @note page is equal to 'row' or 'horizontal position'
 */
void _ssd1963_ram_address( uint16_t y0, uint16_t y1, uint16_t x0, uint16_t x1 )
{
    /* 1. Set Column Address */
    _ssd1963_write_command(0x2a);  /**< set start column address=x0 */
    _ssd1963_write_data8((y0)>>8);
    _ssd1963_write_data8(y0);
    _ssd1963_write_data8((y1)>>8);	/**< set end column address=x1 */
    _ssd1963_write_data8(y1);
    
    /* 2. Set Page Address */
    _ssd1963_write_command(0x2b);  /**< set start page address=y0 */
    _ssd1963_write_data8((x0)>>8);
    _ssd1963_write_data8(x0);
    _ssd1963_write_data8((x1)>>8);	/**< set end page address=y1 */
    _ssd1963_write_data8(x1);
       
}

/**
 * @brief Display a character
 * @param c ASCII character (only the 96 characters from ' ' are supported).
 * @todo Optimizar, unroll for() loops?
 */
void _ssd1963_char(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char2(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font2[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char3(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font3[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char4(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font4[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char5(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font5[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char6(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font6[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char7(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font7[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char8(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font8[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char9(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font9[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char10(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font10[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}
void _ssd1963_char11(uint8_t c)
{   
  const int h = SSD1963_CHAR_WIDTH*_ssd1963.text.scale;
  const int w = SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;

  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
      c = 0;
  } else {
      c -= ' ';
  }
  
  /* Set position */
  _ssd1963_ram_address(
    _ssd1963.text.HEIGHT,
    _ssd1963.text.HEIGHT+w-1,
    _ssd1963.text.WIDTH,
    _ssd1963.text.WIDTH+h-1
  );
  
  /* Write Memory Start */
  _ssd1963_write_command(0x2c);
  
  /* Copy pixels */  
  for(int iy=0;iy<SSD1963_CHAR_HEIGHT;iy++)
  {
    for(int i=0;i<_ssd1963.text.scale;i++)
    {
      for(int ix=0;ix<SSD1963_CHAR_WIDTH;ix++)
      {
        uint32_t state = font11[c][ix]&(1<<iy); 
        //uint32_t state = font[c][ix]&(1<<(SSD1963_CHAR_HEIGHT-iy));

        for(int j=0;j<_ssd1963.text.scale;j++)
        {
          _ssd1963_write_data8( state ?
            _ssd1963.text.color.r : _ssd1963.text.bgcolor.r );
           _ssd1963_write_data8( state ?
            _ssd1963.text.color.g : _ssd1963.text.bgcolor.g );
            _ssd1963_write_data8( state ?
            _ssd1963.text.color.b : _ssd1963.text.bgcolor.b );
        }      
      }      
    }
  }  
}

/* --- Public functions (API) ----------------------------------------------- */

void ssd1963_init()
{
  /* 1. Hardware reset */
  RST =0;
  ssd1963_delay(100);
  RST =1;
  ssd1963_delay(100);
  
  /* 2. Software Reset */
  _ssd1963_write_command(0x01);
 
  ssd1963_delay(100);
  ssd1963_delay(100);
  
  /* 3. Start PLL */
  /* 4. Lock PLL */
  _ssd1963_write_command(0xe0);//start pll
  _ssd1963_write_data8(0x03);
  
  ssd1963_delay(100);

  /* 5. LCD & TFT mode config */
  _ssd1963_write_command(0xb0); /* Set LCD mode / set TFT 18bits mode. */
  _ssd1963_write_data8(0x20);   /* Set TFT mode & hsync+hsync+DEN MODE *///24 bits
  _ssd1963_write_data8(0x80);// RGB
  
  /* 6.1 Set horizontal resolution. Note: 0x031F = 799 *///479
  _ssd1963_write_data8(0x01);  //01 /* HB03 */
  _ssd1963_write_data8(0xdf);  //df /* LB 1f*/
        
  /* 6.2 Set vertical resolution. Note: 0x01DF = 479 */ //255
  _ssd1963_write_data8(0x00);//00   /* HB01 */
  _ssd1963_write_data8(0xff); //ff  /* LBdf */
  
  /* 6.3 Set even/odd line RGB seq.=RGB */
  _ssd1963_write_data8(0x00);
  //----------------------------
  /* 7. Pixel format RGB565 */
  _ssd1963_write_command(0xf0);
  _ssd1963_write_data8(0x00); /*03*///00 8bits  03 16 bits
   
  
  /* 8. Set Address Mode: Top to bottom */
  _ssd1963_write_command(0x36);
  _ssd1963_write_data8(0x60);/*00*/ ///0060

  _ssd1963_write_command(0x0b);
  _ssd1963_write_data8(0x40);
   _ssd1963_write_command(0x0d);
  _ssd1963_write_data8(0xA0);
  
  /* 9. Set PLL Multiplier */ // for 10mhz clk display
  _ssd1963_write_command(0xe2);
  _ssd1963_write_data8(0x1d); /* N = 29 */
  _ssd1963_write_data8(0x02); /* M = 2 */
  _ssd1963_write_data8(0x54);

  /* 10. Set pixel clock frequency */
  _ssd1963_write_command(0xe6); /* Set PCLK freq=4.94MHz (*/
  _ssd1963_write_data8(0x01);   //01 //04
  _ssd1963_write_data8(0x99);   //99 //6f
  _ssd1963_write_data8(0x9a);  //9A//47
//--------------------------------------
  /* 11. Set HSYNC and VSYNC */
  _ssd1963_write_command(0xb4); /* _SET HBP */
  _ssd1963_write_data8(0x04); //02//04  /* SET HSYNC Tatol = 440 */
  _ssd1963_write_data8(0x13); //13//20
  _ssd1963_write_data8(0x00);       /* SET HBP = 68 */
  _ssd1963_write_data8(0x2b); //2b//2e
  _ssd1963_write_data8(0x05); //05//d2 /* SET VBP 16 = 15 + 1 */
  _ssd1963_write_data8(0x00); /* SET Hsync pulse start position */
  _ssd1963_write_data8(0x00);
  _ssd1963_write_data8(0x00); /* SET Hsync pulse subpixel start position */
  
  _ssd1963_write_command(0xb6);/* SET VBP */
  _ssd1963_write_data8(0x01);//01 //02/* SET Vsync total 265 = 264 + 1 */
  _ssd1963_write_data8(0x24);//24//0d
  _ssd1963_write_data8(0x00); /* SET VBP = 19 */
  _ssd1963_write_data8(0x10);//10//17
  _ssd1963_write_data8(0x08);//08//16 /* SET Vsync pulse 8 = 7 + 1 */
  _ssd1963_write_data8(0x00); /* SET Vsync pulse start position */
  _ssd1963_write_data8(0x00);
  
  //---LEO-------
	_ssd1963_write_command(0x2a);  //SET column address
	_ssd1963_write_data8(0x00);   //SET start column address=0
	_ssd1963_write_data8(0x00);
	_ssd1963_write_data8(0x01);//01   //SET end column address=479
	_ssd1963_write_data8(0xdf);//df
        
        _ssd1963_write_command(0x2b);//set page adderss
        _ssd1963_write_data8(0x00);//start page 0
        _ssd1963_write_data8(0x00);
        _ssd1963_write_data8(0x00);// set page 127
        _ssd1963_write_data8(0x7f);
//---------------------------------
 
  /* Finally, turn on display */
  ssd1963_cmd_set_display_on();
  SSD1963_Show_RGB(0x00, 0x00, 0x00, 0, 480, 68, 200);  //negro
}


void ssd1963_cmd_set_display_off()
{
    _ssd1963_write_command(0x28);
}
  
    
void ssd1963_cmd_set_display_on()
{
    _ssd1963_write_command(0x29);
}

void ssd1963_set_pixel(uint32_t HEIGHT, uint32_t WIDTH,  const rgb* color)
{
  _ssd1963_ram_address(HEIGHT,HEIGHT,WIDTH+68,WIDTH+68);
  _ssd1963_write_command(0x2c);  /* Write Memory Start */
  _ssd1963_write_data8(color->r);
  _ssd1963_write_data8(color->g);
  _ssd1963_write_data8(color->b);
}

void ssd1963_rect(uint32_t HEIGHT, uint32_t WIDTH, uint32_t w, uint32_t h,
                     const rgb* color)
{
  uint32_t n = w*h;  
  if (!n)
    return;
  _ssd1963_ram_address(HEIGHT, HEIGHT+w-1,WIDTH+68, WIDTH+68+h-1);
  _ssd1963_write_command(0x2c); /* Write Memory Start */
  do {    
     _ssd1963_write_data8(color->r);
     _ssd1963_write_data8(color->g);
     _ssd1963_write_data8(color->b);
  } while(--n);  
}

void ssd1963_box(uint32_t HEIGHT, uint32_t WIDTH, uint32_t w, uint32_t h,
                     const rgb* color)
{  
  ssd1963_rect(HEIGHT,WIDTH,w,1,color);
  ssd1963_rect(HEIGHT,WIDTH+h-1,w,1,color);
  ssd1963_rect(HEIGHT,WIDTH,1,h,color);
  ssd1963_rect(HEIGHT+w-1,WIDTH,1,h,color);
}

void cmdwrite_rgb(unsigned char dl)     // escribe un comando de dos bits
{    
        _ssd1963_write_data8((dl));
	_ssd1963_write_data8(0xff);
	_ssd1963_write_data8(0xff);
}

void ssd1963_set_text_color(rgb color)
{
  _ssd1963.text.color = color;
}

void ssd1963_set_text_bgcolor(rgb color)
{
  _ssd1963.text.bgcolor = color;
}

void ssd1963_set_text_scale(uint8_t scale)
{
  _ssd1963.text.scale = scale;
}

void ssd1963_set_text_pos(uint16_t HEIGHT, uint16_t WIDTH)
{  /* if (HEIGHT =0)
     {HEIGHT = 1;}
   else if (WIDTH =0)
      {WIDTH=1;}*/
  _ssd1963.text.HEIGHT = HEIGHT*SSD1963_CHAR_HEIGHT*_ssd1963.text.scale;
  _ssd1963.text.WIDTH = (WIDTH*SSD1963_CHAR_WIDTH*_ssd1963.text.scale)+68;
}
void ssd1963_set_text_pos2(uint16_t HEIGHT, uint16_t WIDTH)
{  /* if (HEIGHT =0)
     {HEIGHT = 1;}
   else if (WIDTH =0)
      {WIDTH=1;}*/
  _ssd1963.text.HEIGHT = HEIGHT;
  _ssd1963.text.WIDTH = WIDTH+68;
}


void ssd1963_set_text_origin_position(uint16_t  WIDTH)
{
    _ssd1963.text.origin_x = WIDTH*SSD1963_CHAR_WIDTH*
                                 _ssd1963.text.scale;

}
/*
in: test
*/
void ssd1963_write_text(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}
void ssd1963_write_text2(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char2(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}void ssd1963_write_text3(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char3(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}void ssd1963_write_text4(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char4(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}void ssd1963_write_text5(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char5(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}void ssd1963_write_text6(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char6(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}void ssd1963_write_text7(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char7(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}void ssd1963_write_text8(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char8(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}void ssd1963_write_text9(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char9(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}void ssd1963_write_text10(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char10(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}
void ssd1963_write_text11(const char* text)
{
    uint8_t c;
    while(c=*text++)
    {
      if('\n'==c)
      {
        _ssd1963.text.HEIGHT=_ssd1963.text.HEIGHT-(SSD1963_CHAR_HEIGHT*_ssd1963.text.scale);
        _ssd1963.text.WIDTH=_ssd1963.text.origin_x+68;
       }
      else
      {
        _ssd1963_char11(c);
        _ssd1963.text.WIDTH+=(SSD1963_CHAR_WIDTH*_ssd1963.text.scale);
      }
    }  
}
/*
in: r,g,b,x0,x1,y0,y1
*/
void SSD1963_Show_RGB(unsigned char r,unsigned char g,unsigned char b, unsigned int HEIGHT0, unsigned int HEIGHT1, unsigned int WIDTH0, unsigned int WIDTH1)
{
	 unsigned int i,j;

	_ssd1963_ram_address(HEIGHT0, HEIGHT1, WIDTH0, WIDTH1);
	_ssd1963_write_command(0x2c); 
	for(i=0 ; i <= (WIDTH1 - WIDTH0) ; i++)
	{
		for(j=0 ; j <= (HEIGHT1 - HEIGHT0) ; j++)
		{
			
				//8bit
				_ssd1963_write_data8(r);
				_ssd1963_write_data8(g);
				_ssd1963_write_data8(b);
			
		}
	}
}


//---------------------------------- cmdwrite_2data --------------------------------------
//	 Escribe un comando de 2 datos
//----------------------------------------------------------------------------------------
void cmdwrite_2data(unsigned char dl,unsigned char dh,unsigned char cmd)     // escribe un comando de dos bits
{    
        _ssd1963_write_command(cmd);
	_ssd1963_write_data8(dl);
	_ssd1963_write_data8(dh);
}
