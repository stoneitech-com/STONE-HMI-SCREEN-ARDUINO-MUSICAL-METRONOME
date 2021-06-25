    int ledPin =  13;// the number of the LED pin
    int  pinBuzzer=6;//设置控制蜂鸣器的数字6脚
    unsigned char Uart_Packet_IN=0;
    unsigned char Uart_Packet_Flag=0;
    unsigned char Uart_RX_Buf[9];
    unsigned char Uart_Current_Order=0;
    unsigned char Uart_Packet_Header=0xA5;
    unsigned char speed0=100;
    unsigned char metr0=4;
    unsigned char Temp25[8] = {0x00,0x06,0x0F,0xA2,0x00,0x19,0xEB,0x27};
    unsigned char Play=0x00;
    unsigned int  on=250;
    unsigned int  off=0;
    
    void setup()
    {
      pinMode(ledPin, OUTPUT);
      digitalWrite(ledPin,LOW);
      pinMode(pinBuzzer,OUTPUT);//设置数字IO脚模式，OUTPUT为输出
      Serial.begin(115200);
    }
    void Beep(long fre,unsigned int on,unsigned int off)
    { 
      long frequency = fre;//频率, 单位Hz
      //用tone()函数发出频率为frequency的波形
      tone(pinBuzzer, frequency );
      delay(on); //等待1000毫秒
      noTone(pinBuzzer);//停止发声
      delay(off); //等待2000毫秒
    }
    void metronome(unsigned char metr,unsigned char speed)
    {
      unsigned char i;
      unsigned int  on=250;
      unsigned int  off=60000/speed-250;
      for(i=0;i<metr;i++)
      {
        Beep(300,on,off);
      }
       Beep(600,on,off);
    }
    void serialEvent()
    {
      while(Serial.available()>0)
      {
        unsigned char tmp = (unsigned char)Serial.read();
        
        //-判断是否正在接收数据报文中
        if(Uart_Packet_IN)
        {
          //--Serial.write(0x33);  
          Uart_RX_Buf[Uart_Current_Order++] = tmp;
          //--接收到的数据长度 < 该数据包的设定长度
          if(Uart_Current_Order >=9)
          {
            //--判断是否为包尾  &&  校验和正确
            Uart_Packet_Flag = 1;  //--接收到有效数据包
            Uart_Packet_IN    = 0; //--不再处于接收数据包状态
            Uart_Current_Order= 0; //--对序号清零 
            Serial.write(Uart_RX_Buf,9);
          }
        }
        //-判断是否为数据包头
        else if(tmp==0xA5)
        {
          Uart_Packet_IN  = 1;
          Uart_RX_Buf[0] = tmp;
          Uart_Current_Order=1;
          //--Serial.write(0xAA);
        }
        //--Serial.write(Uart_RX_Buf,9);
      }
    }
    void process(void)
    {
      Uart_Packet_Flag=0;
      switch(Uart_RX_Buf[5])
      { 
        //--判断是操作速度还是节拍
        case 0x06: 
        {
          speed0=Uart_RX_Buf[8];
          Serial.write(speed0);
          break;
        }
        //--判断是操作速度还是节拍
        case 0x07:
        {
          metr0=Uart_RX_Buf[8];
          Serial.write(metr0);
          break;
        }
        //--判断播放或是暂停
        case 0x08:
        {
          Play=Uart_RX_Buf[8];
          off=60000/speed0-250;
          Serial.write(Play);
          break;
        }
        default:break;
      }
      
    }
    void loop()
    {
      if(Uart_Packet_Flag==1)
      {
        process();
        Serial.write(0x88);
      }
      if(Play)
      {
        for(unsigned char i=1;i<metr0;i++)
        {
          Beep(300,on,off);
          digitalWrite(ledPin,HIGH);
          //--Serial.write(0xFF);
        }
        Beep(600,on,off); 
        digitalWrite(ledPin,LOW);
        //--Serial.write(0x00);
      }
    }
