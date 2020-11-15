//VL53L0X 单次测量精度模式配置
//dev:设备 I2C 参数结构体
//mode: 0:默认;1:高精度;2:长距离;3:高速
VL53L0X_Error vl53l0x_set_mode(VL53L0X_Dev_t *dev,u8 mode)
{
		VL53L0X_Error status = VL53L0X_ERROR_NONE;
	uint8_t VhvSettings;
	uint8_t PhaseCal;
	uint32_t refSpadCount;
	uint8_t isApertureSpads;
	vl53l0x_reset(dev);//复位 vl53l0x(频繁切换精度模式容易导致采集距离数据不准，需加上这一代码)
	status = VL53L0X_StaticInit(dev);//参数恢复默认值
	if(AjustOK!=0)//已校准好了,写入校准值
	{
		status= VL53L0X_SetReferenceSpads(dev,Vl53l0x_data.refSpadCount,Vl53l0x_data.isApertureSpads);//设定 Spads 校准值
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status= VL53L0X_SetRefCalibration(dev,Vl53l0x_data.VhvSettings,Vl53l0x_data.PhaseCal);//设定 Ref 校准值
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status=VL53L0X_SetOffsetCalibrationDataMicroMeter(dev,Vl53l0x_data.OffsetMicroMeter);//设定偏移校准值
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status=VL53L0X_SetXTalkCompensationRateMegaCps(dev,Vl53l0x_data.XTalkCompensationRateMegaCps);//设定串扰校准值
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
	}
	else//没校准
	{
		status = VL53L0X_PerformRefCalibration(dev, &VhvSettings,&PhaseCal);//Ref 参考校准
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_PerformRefSpadManagement(dev, &refSpadCount,&isApertureSpads);//执行参考 SPAD 管理
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
	}
	status = VL53L0X_SetDeviceMode(dev,VL53L0X_DEVICEMODE_SINGLE_RANGING);//使能单次测量模式
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,1);//使能 SIGMA 范围检查
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,1);//使能信号速率范围检查
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,Mode_data[mode].sigmaLimit);
	//设定 SIGMA 范围
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status= VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,Mode_data[mode].signalLimit);//设定信号速率范围范围
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status= VL53L0X_SetMeasurementTimingBudgetMicroSeconds(dev,Mode_data[mode].timingBudget);//设定完整测距最长时间
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_PRE_RANGE,Mode_data[mode].preRangeVcselPeriod);//设定 VCSEL 脉冲周期
	if(status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE,Mode_data[mode].finalRangeVcselPeriod);//设定 VCSEL 脉冲周期范围
	error://错误信息
	if(status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(status);
		LCD_Fill(30,140+20,300,300,WHITE);
		return status;
	}
	return status;
}





