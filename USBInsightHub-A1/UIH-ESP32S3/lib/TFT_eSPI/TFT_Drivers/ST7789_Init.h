
// This is the command sequence that initialises the ST7789 driver
//
// This setup information uses simple 8 bit SPI writecommand() and writedata() functions
//
// See ST7735_Setup.h file for an alternative format

#ifndef INIT_SEQUENCE_3
{
  writecommand(ST7789_SLPOUT);   // Sleep out
  delay(120);

  writecommand(ST7789_NORON);    // Normal display mode on

  //------------------------------display and color format setting--------------------------------//
  writecommand(ST7789_MADCTL);
  //writedata(0x00);
  writedata(TFT_MAD_COLOR_ORDER);

  // JLX240 display datasheet
  writecommand(0xB6);
  writedata(0x0A);
  writedata(0x82);

  writecommand(ST7789_RAMCTRL);
  writedata(0x00);
  writedata(0xE0); // 5 to 6 bit conversion: r0 = r5, b0 = b5

  writecommand(ST7789_COLMOD);
  writedata(0x55);
  delay(10);

  //--------------------------------ST7789V Frame rate setting----------------------------------//
  writecommand(ST7789_PORCTRL); //PORCH Control 0xB2
  writedata(0x1F); //Orig 0x0c 
  writedata(0x1F); //Orig 0x0c
  writedata(0x00); //Orig 0x00
  writedata(0x33); //Orig 0x33
  writedata(0x33); //Orig 0x33

  writecommand(ST7789_GCTRL);      //GATE CONTROL Control
  writedata(0x00); //Orig 0x35    //VGH=12.2V,VGL=-7.16V

  //---------------------------------ST7789V Power setting--------------------------------------//
  writecommand(ST7789_VCOMS);
  writedata(0x3F); //orig 0x28  //1.675V

  writecommand(ST7789_LCMCTRL);
  writedata(0x0C);

  writecommand(ST7789_VDVVRHEN);
  writedata(0x01);
  writedata(0xFF);

  writecommand(ST7789_VRHS); //VRH Set 0xC3 //change
  writedata(0x0F); //4.3V org 0x10

  writecommand(ST7789_VDVSET);
  writedata(0x20);

  writecommand(ST7789_FRCTR2); //Frame Rate control in Normal Mode 0xC6
  writedata(0x13); //org 0x0f  //53Hz

  writecommand(ST7789_PWCTRL1);
  writedata(0xa4);
  writedata(0xa1);

  //--------------------------------ST7789V gamma setting---------------------------------------//
  writecommand(ST7789_PVGAMCTRL); //Positive Voltage Gamma Control 0xE0
  writedata(0xF0); //org 0xD0
  writedata(0x06); //org 0x00
  writedata(0x0D); //org 0x02
  writedata(0x0B); //org 0x07
  writedata(0x0A); //org 0x0a
  writedata(0x07); //org 0x028
  writedata(0x2E); //org 0x32
  writedata(0x43); //org 0x44
  writedata(0x45); //org 0x42
  writedata(0x38); //org 0x06
  writedata(0x14); //org 0x0e
  writedata(0x13); //org 0x12
  writedata(0x25); //org 0x14
  writedata(0x29); //org 0x17

  writecommand(ST7789_NVGAMCTRL); //Negative Voltage Gamma Control 0xE0
  writedata(0xF0); //org 0xd0
  writedata(0x07); //org 0x00
  writedata(0x0A); //org 0x02
  writedata(0x08); //org 0x07
  writedata(0x07); //org 0x0a
  writedata(0x23); //org 0x28
  writedata(0x2E); //org 0x31
  writedata(0x33); //org 0x54
  writedata(0x44); //org 0x47
  writedata(0x3A); //org 0x0e
  writedata(0x16); //org 0x1c
  writedata(0x17); //org 0x17
  writedata(0x26); //org 0x1b
  writedata(0x2C); //org 0x1e

  writecommand(ST7789_INVON);

  writecommand(ST7789_CASET);    // Column address set
  writedata(0x00);
  writedata(0x00);
  writedata(0x00);
  writedata(0xEF);    // 239

  writecommand(ST7789_RASET);    // Row address set
  writedata(0x00);
  writedata(0x00);
  writedata(0x01);
  writedata(0x3F);    // 319

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  end_tft_write();
  delay(120);
  begin_tft_write();

  writecommand(ST7789_DISPON);    //Display on
  delay(120);

#ifdef TFT_BL
  // Turn on the back-light LED
  digitalWrite(TFT_BL, HIGH);
  pinMode(TFT_BL, OUTPUT);
#endif
}


#else
// TTGO ESP32 S3 T-Display
{
  writecommand(ST7789_SLPOUT);   // Sleep out
  delay(120);

  writecommand(ST7789_NORON);    // Normal display mode on

  //------------------------------display and color format setting--------------------------------//
  writecommand(ST7789_MADCTL);
  writedata(TFT_MAD_COLOR_ORDER);

 // writecommand(ST7789_RAMCTRL);
 // writedata(0x00);
 // writedata(0xE0); // 5 to 6 bit conversion: r0 = r5, b0 = b5

  writecommand(ST7789_COLMOD);
  writedata(0x55);
  delay(10);

  //--------------------------------ST7789V Frame rate setting----------------------------------//
  writecommand(ST7789_PORCTRL);
  writedata(0x0b);
  writedata(0x0b);
  writedata(0x00);
  writedata(0x33);
  writedata(0x33);

  writecommand(ST7789_GCTRL);      // Voltages: VGH / VGL
  writedata(0x75);
  
  //---------------------------------ST7789V Power setting--------------------------------------//
  writecommand(ST7789_VCOMS);
  writedata(0x28);		// JLX240 display datasheet

  writecommand(ST7789_LCMCTRL);
  writedata(0x2C);

  writecommand(ST7789_VDVVRHEN);
  writedata(0x01);

  writecommand(ST7789_VRHS);       // voltage VRHS
  writedata(0x1F);

  writecommand(ST7789_FRCTR2);
  writedata(0x13);

  writecommand(ST7789_PWCTRL1);
  writedata(0xa7);

  writecommand(ST7789_PWCTRL1);
  writedata(0xa4);
  writedata(0xa1);

  writecommand(0xD6);
  writedata(0xa1);

  //--------------------------------ST7789V gamma setting---------------------------------------//
  writecommand(ST7789_PVGAMCTRL);
  writedata(0xf0);
  writedata(0x05);
  writedata(0x0a);
  writedata(0x06);
  writedata(0x06);
  writedata(0x03);
  writedata(0x2b);
  writedata(0x32);
  writedata(0x43);
  writedata(0x36);
  writedata(0x11);
  writedata(0x10);
  writedata(0x2b);
  writedata(0x32);

  writecommand(ST7789_NVGAMCTRL);
  writedata(0xf0);
  writedata(0x08);
  writedata(0x0c);
  writedata(0x0b);
  writedata(0x09);
  writedata(0x24);
  writedata(0x2b);
  writedata(0x22);
  writedata(0x43);
  writedata(0x38);
  writedata(0x15);
  writedata(0x16);
  writedata(0x2f);
  writedata(0x37);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  end_tft_write();
  delay(120);
  begin_tft_write();

  writecommand(ST7789_DISPON);    //Display on
  delay(120);

#ifdef TFT_BL
  // Turn on the back-light LED
  digitalWrite(TFT_BL, HIGH);
  pinMode(TFT_BL, OUTPUT);
#endif
}
#endif