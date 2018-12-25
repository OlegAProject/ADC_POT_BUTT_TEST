#include <ch.h>
#include <hal.h>

#include <chprintf.h>

/////////////////////////////////////////////////////////////////////////////////
//ADC 
/////////////////////////////////////////////////////////////////////////////////
int k = 0;

#define ADC1_NUM_CHANNELS   1
#define ADC1_BUF_DEPTH      1

static adcsample_t adc_buffer[ADC1_NUM_CHANNELS * ADC1_BUF_DEPTH];


static const GPTConfig gpt4cfg1 = {
  .frequency =  100000,
  .callback  =  NULL,
  .cr2       =  TIM_CR2_MMS_1,  // MMS = 010 = TRGO on Update Event.
  .dier      =  0U
 };

static void adccallback(ADCDriver *adcp, adcsample_t *buffer, size_t n)
{
	adcp = adcp; n = n;
    k = buffer[0];
}

static const ADCConversionGroup adcgrpcfg1 = {
  .circular     = true,
  .num_channels = ADC1_NUM_CHANNELS,
  .end_cb       = adccallback,
  .error_cb     = 0,
  .cr1          = 0,
  .cr2          = ADC_CR2_EXTEN_RISING | ADC_CR2_EXTSEL_SRC(12),
  .smpr1        = ADC_SMPR1_SMP_AN10(ADC_SAMPLE_144),
  .smpr2        = 0,
  .sqr1         = ADC_SQR1_NUM_CH(ADC1_NUM_CHANNELS),
  .sqr2         = 0,
  .sqr3         = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN10)
};

void adc_set_and_start(void)
{
    adcStart(&ADCD1, NULL);
    adcStartConversion(&ADCD1, &adcgrpcfg1, adc_buffer, ADC1_BUF_DEPTH);
    palSetLineMode( LINE_ADC123_IN10, PAL_MODE_INPUT_ANALOG );  // PC0

    gptStart(&GPTD4, &gpt4cfg1);
    gptStartContinuous(&GPTD4, gpt4cfg1.frequency/5);
}

/////////////////////////////////////////////////////////////////////////////////
//SERIAL PORT
/////////////////////////////////////////////////////////////////////////////////
static const SerialConfig sdcfg = {
    .speed  = 9600,
    .cr1    = 0,
    .cr2    = 0,
    .cr3    = 0
};

void sd_set(void)
{
    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );    // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );    // RX
}

/////////////////////////////////////////////////////////////////////////////////
// BUTTON EXT
/////////////////////////////////////////////////////////////////////////////////
char flag = 0;
static void extcb( EXTDriver *extp, expchannel_t channel)
{
	extp = extp;
    channel = channel;
    flag = 1;
}

static const EXTConfig extcfg = {
  .channels =
  {
    [0]  = {EXT_CH_MODE_DISABLED, NULL},
    [1]  = {EXT_CH_MODE_DISABLED, NULL},
    [2]  = {EXT_CH_MODE_DISABLED, NULL},
    [3]  = {EXT_CH_MODE_DISABLED, NULL},
    [4]  = {EXT_CH_MODE_DISABLED, NULL},
    [5]  = {EXT_CH_MODE_DISABLED, NULL},
    [6]  = {EXT_CH_MODE_DISABLED, NULL},
    [7]  = {EXT_CH_MODE_DISABLED, NULL},
    [8]  = {EXT_CH_MODE_DISABLED, NULL},
    [9]  = {EXT_CH_MODE_DISABLED, NULL},
    [10] = {EXT_CH_MODE_DISABLED, NULL},
    [11] = {EXT_CH_MODE_DISABLED, NULL},
    [12] = {EXT_CH_MODE_DISABLED, NULL},
    [13] = {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb}, //PC13 = Button
    [14] = {EXT_CH_MODE_DISABLED, NULL},
    [15] = {EXT_CH_MODE_DISABLED, NULL},
  }
};

void ext_set_and_start(void)
{
	extStart( &EXTD1, &extcfg );
}

/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	chSysInit();
    halInit();
    sd_set();
    adc_set_and_start();
    ext_set_and_start();
    while (true)
    {
    	if (flag == 1)
    	{
    		chprintf( (BaseSequentialStream *)&SD7, " %d \n\r", k/23 , 4 );
    		flag = 0;
    	}
    	chThdSleepMilliseconds(50);
    }
}
