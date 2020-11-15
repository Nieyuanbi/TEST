#include <rtthread.h>
#include <rthw.h>
#include <ls1c.h>
#include <stdlib.h> 

#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_pwm.h"
#include "../libraries/vl53l0x.h" //��������ATK-VL53L0X���⴫�����Ŀ⺯��

static rt_thread_t tid1 = RT_NULL;
int tong=520;// ֻ��һ��������Ϊ��ʾ��
int tong1=1923; // ���� us
int i=1;
unsigned int gpio, duty;
unsigned int led_gpio= 53;  //����LED��gpio��



void test_beep_thread(void* parameter)
{
	
    pwm_info_t pwm_info;
    pwm_info.gpio = gpio;                           // pwm����λgpio
    pwm_info.mode = PWM_MODE_NORMAL;                // ����ģʽ--�������pwm����
    pwm_info.duty = (float)duty/100;                           // pwmռ�ձ�
	gpio_init(led_gpio, gpio_mode_output);// ��ʼ��led

while(1)
{
    
    
        pwm_info.period_ns = tong1*200;               // pwm����5ms
        pwm_init(&pwm_info);   // pwm��ʼ������ʼ������������pwm����   
        pwm_enable(&pwm_info); // ʹ��pwm=1
		gpio_set(led_gpio, gpio_level_low);
       // rt_thread_delay(RT_TICK_PER_SECOND*1)   ;	
		delay_s(10);
		pwm_disable(&pwm_info);//pwm=0
		gpio_set(led_gpio, gpio_level_high);//����
		delay_s(10);
		//	rt_thread_delay(RT_TICK_PER_SECOND*1) ;//�����ӳٿ��ƺ�����������RT-thread���е��߳��ӳ٣�Ҳ������delay.h����ӳٺ���
}

    return ;
}
#define gpio_sda    (2)
#define gpio_scl    (3)
void test_warning(void *parameter)
{
	ls1c_i2c_gpio_init(gpio_scl);
	ls1c_set_scl(gpio_scl);
	ls1c_i2c_gpio_init(gpio_sda);	//��ʼ��I2C����
	ls1c_set_sda(gpio_sda);
	vl53l0x_init(ls1c_i2c_gpio_sda,ls1c_i2c_gpio_scl);//��ʼ�����⴫����ģ��
	while(1)
	{
		vl53l0x_scan(ls1c_i2c_gpio);
		if(vl53l0x_get(ls1c_i2c_gpio_sda,ls1c_i2c_gpio_scl)<=1500)//�趨1.5��Ϊ������ֵ
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