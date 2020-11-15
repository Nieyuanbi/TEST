#include <rtthread.h>
#include <rthw.h>
#include <ls1c.h>
#include <stdlib.h> 

#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_pwm.h"
#include "../libraries/vl53l0x.h" //调用驱动ATK-VL53L0X激光传感器的库函数

static rt_thread_t tid1 = RT_NULL;
int tong=520;// 只给一个中音作为提示音
int tong1=1923; // 周期 us
int i=1;
unsigned int gpio, duty;
unsigned int led_gpio= 53;  //配置LED的gpio口



void test_beep_thread(void* parameter)
{
	
    pwm_info_t pwm_info;
    pwm_info.gpio = gpio;                           // pwm引脚位gpio
    pwm_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
    pwm_info.duty = (float)duty/100;                           // pwm占空比
	gpio_init(led_gpio, gpio_mode_output);// 初始化led

while(1)
{
    
    
        pwm_info.period_ns = tong1*200;               // pwm周期5ms
        pwm_init(&pwm_info);   // pwm初始化，初始化后立即产生pwm波形   
        pwm_enable(&pwm_info); // 使能pwm=1
		gpio_set(led_gpio, gpio_level_low);
       // rt_thread_delay(RT_TICK_PER_SECOND*1)   ;	
		delay_s(10);
		pwm_disable(&pwm_info);//pwm=0
		gpio_set(led_gpio, gpio_level_high);//灯灭
		delay_s(10);
		//	rt_thread_delay(RT_TICK_PER_SECOND*1) ;//关于延迟控制函数，可以用RT-thread独有的线程延迟，也可以用delay.h里的延迟函数
}

    return ;
}
#define gpio_sda    (2)
#define gpio_scl    (3)
void test_warning(void *parameter)
{
	ls1c_i2c_gpio_init(gpio_scl);
	ls1c_set_scl(gpio_scl);
	ls1c_i2c_gpio_init(gpio_sda);	//初始化I2C总线
	ls1c_set_sda(gpio_sda);
	vl53l0x_init(ls1c_i2c_gpio_sda,ls1c_i2c_gpio_scl);//初始化激光传感器模块
	while(1)
	{
		vl53l0x_scan(ls1c_i2c_gpio);
		if(vl53l0x_get(ls1c_i2c_gpio_sda,ls1c_i2c_gpio_scl)<=1500)//设定1.5米为报警阈值
			test_beep_thread(vl53l0x_get(ls1c_i2c_gpio_sda,ls1c_i2c_gpio_scl));
		

	
	}



}



#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5
#include  <finsh.h> 

void test_beep(int argc, char** argv)
{
    if(argc != 3)
    return;

	gpio = strtoul(argv[1], NULL, 0);
	duty = strtoul(argv[2], NULL, 0);

	tid1 = rt_thread_create("thread1",
			test_beep_thread, RT_NULL,       
			THREAD_STACK_SIZE, THREAD_PRIORITY  , THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
			rt_thread_startup(tid1);
}

void test_warning(void)
{
	tid1 = rt_thread_create("t1",
		thread1_entry, RT_NULL,       
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
	if (tid1 != RT_NULL)
		rt_thread_startup(tid1);

	tid2 = rt_thread_create("t2",
		thread2_entry,   RT_NULL,       
		THREAD_STACK_SIZE, THREAD_PRIORITY-1, THREAD_TIMESLICE);
	if (tid2 != RT_NULL)
		rt_thread_startup(tid2);
}


MSH_CMD_EXPORT(test_beep, test_beep e.g.test_beep 46 50);
MSH_CMD_EXPORT(test_warning, test_warning gpio_2 gpio_3);