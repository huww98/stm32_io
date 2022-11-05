#include "oled.h"

#define OLED_I2C_ADDR (0b111100 << 1)

void oled_command_1(I2C_HandleTypeDef *i2c, uint8_t cmd0) {
  uint8_t data[2];
  data[0] = 0;
  data[1] = cmd0;
  HAL_I2C_Master_Transmit(i2c, OLED_I2C_ADDR, data, 2, 1);
}

void oled_command_2(I2C_HandleTypeDef *i2c, uint8_t cmd0, uint8_t cmd1) {
  uint8_t data[3];
  data[0] = 0;
  data[1] = cmd0;
  data[2] = cmd1;
  HAL_I2C_Master_Transmit(i2c, OLED_I2C_ADDR, data, 3, 1);
}

void oled_command_3(I2C_HandleTypeDef *i2c, uint8_t cmd0, uint8_t cmd1, uint8_t cmd2) {
  uint8_t data[4];
  data[0] = 0;
  data[1] = cmd0;
  data[2] = cmd1;
  data[3] = cmd2;
  HAL_I2C_Master_Transmit(i2c, OLED_I2C_ADDR, data, 4, 1);
}

void oled_clear(I2C_HandleTypeDef *i2c) {
  uint8_t data[2];
  data[0] = 0x40;
  data[1] = 0;
  for(int i=0; i<8; i++)
	{
    oled_command_3(i2c, 0xb0 + i, 0x00, 0x10); // set address
    for(int j=0; j<128; j++)
      HAL_I2C_Master_Transmit(i2c, OLED_I2C_ADDR, data, 2, 1);
	}
}

void oled_test_seq(I2C_HandleTypeDef *i2c) {
  oled_command_2(i2c, 0x20, 0x00); // horizontal addressing mode
  oled_command_3(i2c, 0x22, 0, 7); // set page start/end
  uint8_t data[2];
  data[0] = 0x40;
  for (int m = 0; m < 2; m++) {
    if (m == 0)
      data[1] = 0xff;
    else
      data[1] = 0x00;

    for(int i=0; i<8; i++)
    {
      for(int j=1; j<129; j++) {
        HAL_I2C_Master_Transmit(i2c, OLED_I2C_ADDR, data, 2, 1);
        HAL_Delay(2);
      }
    }
  }

}

void oled_init(struct oled_handle *handle) {
  HAL_GPIO_WritePin(handle->rst_port, handle->rst_pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(handle->pwr_port, handle->pwr_pin, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(handle->rst_port, handle->rst_pin, GPIO_PIN_SET);
  HAL_Delay(1);

  // set osc frequency
  // oled_command_1(handle->i2c, 0xD5);
  // oled_command_1(handle->i2c, 0x80);

  // Enable charge pump
  oled_command_2(handle->i2c, 0x8D, 0x14);

  // display on
  oled_command_1(handle->i2c, 0xAF);

  // entire display on
  oled_command_1(handle->i2c, 0xA5);
  HAL_Delay(1000);

  // resume to RAM content display
  oled_command_1(handle->i2c, 0xA4);
  HAL_Delay(1000);

  oled_clear(handle->i2c);
}
