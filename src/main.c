#include "include/board.h"
#include "lib/io.h"
#include "lib/uart.h"
#include "lib/util.h"
#include "lib/i2c.h"
#include "lib/timer.h"
//#include "libshield/sht4x.h"
//#include "libshield/tcs34725.h"
//#include "libshield/gesture.h"
#include "libshield/sunlight.h"

#define MAIN_GLOBAL_INIT

#define TEST_IHM

#ifdef MAIN_GLOBAL_INIT

#define DELAY_1_SECOND      1000000
#define DELAY_10_SECONDS    10000000

#define delay_us(us)        timer_wait_us(_TIM3, us)

volatile char command;

static void on_command_received(char c) {
    command = c;
}

int main(void) {
    uart_init(_USART2, 115200, UART_8N1, on_command_received);
     
    i2c_master_init(_I2C1);
    init();
    SI114X_DeInit();
    SI114X_Begin();
    

    while (1) {

        //print_gesture();
   

    // Envoi de la ligne de séparation
    uart_puts(_USART2, "//--------------------------------------//\r\n");

    // Envoi de la valeur visible
    uart_puts(_USART2, "Vis: ");
    uart_printf(_USART2, "%d\r\n", SI114X_ReadVisible());

    // Envoi de la valeur infrarouge
    uart_puts(_USART2, "IR: ");
    uart_printf(_USART2, "%d\r\n", SI114X_ReadIR());

    // Lecture de la valeur UV (elle doit être divisée par 100)
    uint16_t uv_value = SI114X_ReadUV();
    float uv_float = (float) uv_value;

    int partie_entiere = (int)uv_float;
    int partie_fractionnaire = fabsf(uv_float - partie_entiere) * 1000;

    // Envoi de la valeur UV
    uart_puts(_USART2, "UV: ");
    uart_printf(_USART2, "%d"".""%d\r\n", partie_entiere,partie_fractionnaire);

    delay_us(DELAY_10_SECONDS);
		   	
 
		}
              
    return 0;
}

#endif
