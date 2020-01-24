/*
 * Based on an example from the libopencm3 project. 
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 *
 */


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>

/*
* set up timer to fire every x miliseconds
* it's 24 bit range, can be done between 1 and 2796 ms
*/

uint8_t channel_array[] = { 1, 1, ADC_CHANNEL_TEMP};

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA); //enable clock for GPIO port A
    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1); //trimpot on PA1
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4); //LED on PA4
}

static void adc_setup(void) {

    rcc_periph_clock_enable(RCC_ADC); //enable clock for ADC

    adc_power_off(ADC1);
    adc_set_clk_source(ADC1,ADC_CLKSOURCE_ADC);
    adc_calibrate(ADC1);
    adc_set_operation_mode(ADC1, ADC_MODE_SCAN);
    adc_disable_external_trigger_regular(ADC1);
    adc_set_right_aligned(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_071DOT5);
    adc_set_regular_sequence(ADC1, 1, channel_array);
    adc_set_resolution(ADC1, ADC_RESOLUTION_6BIT);
    adc_disable_analog_watchdog(ADC1);
    adc_power_on(ADC1);

    /* wait for ADC start up */

   int i;
	for (i = 0; i < 800000; i++) {    /* Wait a bit. */
		__asm__("nop");
	}

}

/* set MCU to clock by 48MHZ from HSI oscillator */
static void clock_setup(void) {
    rcc_clock_setup_in_hsi_out_48mhz(); //Set System Clock PLL at 48MHz from HSI.
        
}


int main(void) {
    
    clock_setup();
    gpio_setup();
    adc_setup();


    int i;
    int reading;

	while(1) {
		
        adc_start_conversion_regular(ADC1);
        while(!(adc_eoc(ADC1)));

        reading = (adc_read_regular(ADC1) + 1) * 100000; //make sure we're not using 0
        
        gpio_clear(GPIOA,GPIO4);	/* LED on */
		for (i = 0; i < reading; i++)	/* Wait a bit. */
			__asm__("nop");

		gpio_set(GPIOA,GPIO4);		/* LED off */
		for (i = 0; i < reading; i++)	/* Wait a bit. */
			__asm__("nop");
	}

	return 0;


}
