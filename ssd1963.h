/** 
 *  @defgroup SSD1963 SSD1963 Display Controller
 *  @ingroup LIB
 *  @brief SSD1963 Display Controller
 *  @details SSD1963 Display Controller High Level API
 *  @warning Command/Data Read/Write commands are hardcoded to DSY-3 Display pinout.
 *  @addtogroup SSD1963
 *  @{
 */

#ifndef SSD1963_H
#define SSD1963_H
#include <stdint.h>
#include <rgb.h>


#define SSD1963_WIDTH  480          /**< Display width 800*/
#define SSD1963_HEIGHT 128          /**< Display height 480 */
#define SSD1963_CHAR_WIDTH 6/**< Text character width */
#define SSD1963_CHAR_HEIGHT 8       /**< Text character height */
#define SSD1963_CHAR_WIDTH2 6    // para numeros especiales
#define SSD1963_CHAR_HEIGHT2 8       

#define        PORT         P7		// #define        Data_BUS         P1		  // D15 ~ D8
#define        Data_BUS_16bits  P1


#define  CD P0_bit.P04	// DispDC	//sbit           IC_A0            =P2^6;    // Data/command select L:Command,H:Data
#define  WR P4_bit.P47 			//sbit           IC_WR            =P0^6; 	  // L: Write, H: Read
#define  RD P4_bit.P46 			//sbit           IC_RD            =P0^7;	  // Data Enable H-->L
#define  CE P0_bit.P05			//sbit           IC_CS            =P2^4;    // L: Chip select
#define  RST P0_bit.P03			//sbit           IC_RST           =P2^1;    // L: RESET  

/**
 * @brief Initialize display controller
 * @details Initializes the display controller and internal structures. Must
            be called once at application startup.
 */
void ssd1963_init();
void ssd1963_cmd_set_display_on();
void ssd1963_cmd_set_display_off();

void ssd1963_rect(uint32_t HEIGHT, uint32_t WIDTH, uint32_t w, uint32_t h, const rgb* color);
void ssd1963_box(uint32_t HEIGHT, uint32_t WIDTH, uint32_t w, uint32_t h, const rgb* color);


void ssd1963_set_text_color(rgb color);
void ssd1963_set_text_bgcolor(rgb color);
void ssd1963_set_text_scale(uint8_t scale);
void ssd1963_set_text_pos(uint16_t HEIGHT, uint16_t WIDTH);
void ssd1963_set_text_origin_position(uint16_t HEIGHT);
void ssd1963_write_text(const char* text);
void ssd1963_delay(uint32_t interval);

void SSD1963_Show_RGB(unsigned char dat1,unsigned char dat2,unsigned char dat3, unsigned int HEIGHT0, unsigned int HEIGHT1, unsigned int WIDTH0, unsigned int WIDTH1);
void cmdwrite_2data(unsigned char dl,unsigned char dh,unsigned char cmd);

void ssd1963_set_pixel(uint32_t HEIGHT, uint32_t WIDTH, const rgb* color);
void ssd1963_draw_bitmap(uint32_t HEIGHT, uint32_t WIDTH, uint32_t w, uint32_t h, int const *fondo);

#endif
/**@}*/
