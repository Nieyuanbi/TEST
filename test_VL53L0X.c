//VL53L0X ���β�������ģʽ����
//dev:�豸 I2C �����ṹ��
//mode: 0:Ĭ��;1:�߾���;2:������;3:����
VL53L0X_Error vl53l0x_set_mode(VL53L0X_Dev_t *dev,u8 mode)
{
		VL53L0X_Error status = VL53L0X_ERROR_NONE;
	uint8_t VhvSettings;
	uint8_t PhaseCal;
	uint32_t refSpadCount;
	uint8_t isApertureSpads;
	vl53l0x_reset(dev);//��λ vl53l0x(Ƶ���л�����ģʽ���׵��²ɼ��������ݲ�׼���������һ����)
	status = VL53L0X_StaticInit(dev);//�����ָ�Ĭ��ֵ
	if(AjustOK!=0)//��У׼����,д��У׼ֵ
	{
		status= VL53L0X_SetReferenceSpads(dev,Vl53l0x_data.refSpadCount,Vl53l0x_data.isApertureSpads);//�趨 Spads У׼ֵ
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status= VL53L0X_SetRefCalibration(dev,Vl53l0x_data.VhvSettings,Vl53l0x_data.PhaseCal);//�趨 Ref У׼ֵ
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status=VL53L0X_SetOffsetCalibrationDataMicroMeter(dev,Vl53l0x_data.OffsetMicroMeter);//�趨ƫ��У׼ֵ
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status=VL53L0X_SetXTalkCompensationRateMegaCps(dev,Vl53l0x_data.XTalkCompensationRateMegaCps);//�趨����У׼ֵ
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
	}
	else//ûУ׼
	{
		status = VL53L0X_PerformRefCalibration(dev, &VhvSettings,&PhaseCal);//Ref �ο�У׼
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_PerformRefSpadManagement(dev, &refSpadCount,&isApertureSpads);//ִ�вο� SPAD ����
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
	}
	status = VL53L0X_SetDeviceMode(dev,VL53L0X_DEVICEMODE_SINGLE_RANGING);//ʹ�ܵ��β���ģʽ
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,1);//ʹ�� SIGMA ��Χ���
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,1);//ʹ���ź����ʷ�Χ���
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,Mode_data[mode].sigmaLimit);
	//�趨 SIGMA ��Χ
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status= VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,Mode_data[mode].signalLimit);//�趨�ź����ʷ�Χ��Χ
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status= VL53L0X_SetMeasurementTimingBudgetMicroSeconds(dev,Mode_data[mode].timingBudget);//�趨��������ʱ��
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_PRE_RANGE,Mode_data[mode].preRangeVcselPeriod);//�趨 VCSEL ��������
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE,Mode_data[mode].finalRangeVcselPeriod);//�趨 VCSEL �������ڷ�Χ
	error://������Ϣ
	if(status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(status);
		LCD_Fill(30,140+20,300,300,WHITE);
		return status;
	}
	return status;
}





