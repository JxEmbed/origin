#include "power.h"

/*
    brief      Configure the ADC peripheral
    param[in]  adc_typedef_enum adc_id
    param[out] none
    retval     none
*/
void power_48v_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOD);
	//48V_EN
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	Power_48V_EN=0;

}

void power_adc_init2(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOF);

    /* enable ADC0 clock */
    rcu_periph_clock_enable(RCU_ADC2);

    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);

    /* config the GPIO as analog mode */
    gpio_init(GPIOF, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
	
    /* ADC continuous mode function disableADC0, ADC_SCAN_MODE, DISABLE */
    adc_special_function_config(ADC2, ADC_SCAN_MODE, DISABLE);

    /* ADC data alignment config */
    adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC2, ADC_REGULAR_CHANNEL, 1);

    /* ADC regular channel config */
    adc_regular_channel_config(ADC2, 0, ADC_CHANNEL_15, ADC_SAMPLETIME_1POINT5);

    /* ADC trigger config ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE*/
    adc_external_trigger_source_config(ADC2, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC2, ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable(ADC2);
    delay_ms(1);

    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC2);
}

void power_adc_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	rcu_periph_clock_enable(RCU_ADC0);
	/* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
    /* 复位ADC0*/
    adc_deinit(ADC0);
    /* 模式 */
    adc_mode_config(ADC_MODE_FREE);
    /* 连续测量使能 */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);

    /* 右对齐 */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* 通道长度 */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);

    /*ADC常规通道配置 */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_12, ADC_SAMPLETIME_1POINT5);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
	

}
/*
14.6/12.2*2.2=测量电压值



*/
float Power_ADC_Read(void)
{
	 float adc_convertedValueLocal;  
	float adc_calcResult;
    uint32_t adc_convertedValue;
	adc_software_trigger_enable(ADC2, ADC_REGULAR_CHANNEL);

	while(!adc_flag_get(ADC2,ADC_FLAG_EOC));//检查转换标志
	adc_flag_clear(ADC2, ADC_FLAG_EOC); // 清除结束标志
		
	adc_convertedValue=adc_regular_data_read(ADC2);//ADC转换结果

	adc_convertedValueLocal =(float) adc_convertedValue/4096*3.3; // 读取转换的电压值
	adc_calcResult=adc_convertedValueLocal*12.2/2.2;
	Statlog_Par_new.voltage=adc_calcResult;
	Statlog_Par.voltage_flag=1;
	return adc_calcResult;
}



