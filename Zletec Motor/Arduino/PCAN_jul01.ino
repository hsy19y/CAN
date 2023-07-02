//master code(데이터 처리)
//#include <MsTimer2.h>
#include <Wire.h>
typedef char BYTE_t;
#define Slave   174 //서버 주소
#define RING_BUFFER_SIZE 8
#define TEMP_BUFFER_SIZE 8

static char ring_buff[RING_BUFFER_SIZE]={0,};
//char processing_data[8]={0,}; //
//byte Receive[8] = {0,};       //
byte Receive[8]; 
static int cnt = 0;

BYTE_t sending_crc_check[8]={0,}; //

struct{
  byte Motor_mod = 0;
  long Value = 0;
}pc_data;

union{
  unsigned char hex[2];
  int dec;
}hex2dec;

union
{
  unsigned short check_sum;
  unsigned char b[2];
} crc_16_val;

int CRC16_MODBUS(const uint8_t *nData, uint16_t wLength)
{

  static const uint16_t wCRCTable[] = { 0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241, 0XC601, 0X06C0, 0X0780,
    0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440, 0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1,
    0XCE81, 0X0E40, 0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841, 0XD801,
    0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40, 0X1E00, 0XDEC1, 0XDF81, 0X1F40,
    0XDD01, 0X1DC0, 0X1C80, 0XDC41, 0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680,
    0XD641, 0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040, 0XF001, 0X30C0,
    0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240, 0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501,
    0X35C0, 0X3480, 0XF441, 0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840, 0X2800, 0XE8C1, 0XE981,
    0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41, 0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1,
    0XEC81, 0X2C40, 0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640, 0X2200,
    0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041, 0XA001, 0X60C0, 0X6180, 0XA141,
    0X6300, 0XA3C1, 0XA281, 0X6240, 0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480,
    0XA441, 0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41, 0XAA01, 0X6AC0,
    0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840, 0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01,
    0X7BC0, 0X7A80, 0XBA41, 0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640, 0X7200, 0XB2C1, 0XB381,
    0X7340, 0XB101, 0X71C0, 0X7080, 0XB041, 0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0,
    0X5280, 0X9241, 0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440, 0X9C01,
    0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40, 0X5A00, 0X9AC1, 0X9B81, 0X5B40,
    0X9901, 0X59C0, 0X5880, 0X9841, 0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81,
    0X4A40, 0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41, 0X4400, 0X84C1,
    0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641, 0X8201, 0X42C0, 0X4380, 0X8341, 0X4100,
    0X81C1, 0X8081, 0X4040 };

  uint8_t nTemp;
  uint16_t wCRCWord = 0xFFFF;

  while (wLength--)
  {
    nTemp = *nData++ ^ wCRCWord;
    wCRCWord >>= 8;
    wCRCWord ^= wCRCTable[nTemp];
  }

  return wCRCWord;
}
/*
void Ring_buff(){
    //Wire.requestFrom(Slave, 9);
    char read_temp[TEMP_BUFFER_SIZE];
    byte leng = Serial1.readBytes(read_temp,TEMP_BUFFER_SIZE);
    //delay(2);
    for(int i=0; i<RING_BUFFER_SIZE; i++){
     ring_buff[i]=0;
     }
    for(int i=0; i<leng; i++){
      ring_buff[cnt] = read_temp[i];
      cnt++;
      if(cnt>=RING_BUFFER_SIZE){
        cnt = 0;
       }
    }
  byte data_check[8]={0,};
  for(int j=0;j<8;j++){
    data_check[j] = ring_buff[(cnt + RING_BUFFER_SIZE - 8+j) % RING_BUFFER_SIZE];
    //Serial.print(data_check[j]);Serial.print(" ");
  }
  //Serial.println(" ");
  if(data_check[0]==184 && data_check[1]==174 && data_check[2]=='I'){
    hex2dec.hex[1]=data_check[4];
    hex2dec.hex[0]=data_check[5];
    pc_data.Value = hex2dec.dec;
    pc_data.Motor_mod = data_check[3];
    //Serial.print(hex2dec.dec);Serial.println(" ");
    //Serial.print(hex2dec.hex[0]);Serial.print(" ");Serial.print(hex2dec.hex[1]);Serial.print(" ");Serial.print(hex2dec.hex[2]);Serial.print(" ");Serial.print(hex2dec.hex[3]);Serial.println(" ");
    Serial.print(" Mod : ");Serial.print(pc_data.Motor_mod);Serial.print(" Value : ");Serial.print(pc_data.Value);
    Serial.println(" ");
  }
}
*/
void Ring_buff(){
  char read_temp[TEMP_BUFFER_SIZE];
  byte leng = Serial1.readBytes(read_temp, TEMP_BUFFER_SIZE);
  if(leng <=RING_BUFFER_SIZE){
    for(int i=0; i<leng; i++){
      ring_buff[cnt]=read_temp[i];
      cnt++;
    if(cnt >= RING_BUFFER_SIZE){ cnt= 0;}
    }
  }
  else{
     for(int i=0; i<RING_BUFFER_SIZE; i++){
        ring_buff[i]=0;
     }
     cnt=0;
  }
  byte data_check[8]={0,};
  for(int j=0;j<8;j++){
    data_check[j] = ring_buff[(cnt + RING_BUFFER_SIZE - 8+j) % RING_BUFFER_SIZE];
    //Serial.print(data_check[j]);Serial.print(" ");
  }
  //Serial.println(" ");
  if(data_check[0]==184 && data_check[1]==174 && data_check[2]=='I'){
    hex2dec.hex[1]=data_check[4];
    hex2dec.hex[0]=data_check[5];
    pc_data.Value=hex2dec.dec;
    pc_data.Motor_mod=data_check[3];
    //Serial.print(hex2dec.dec);Serial.println(" ");
    //Serial.print(hex2dec.hex[0]);Serial.print(" ");Serial.print(hex2dec.hex[1]);Serial.print(" ");Serial.print(hex2dec.hex[2]);Serial.print(" ");Serial.print(hex2dec.hex[3]);Serial.println(" ");
    Serial.print(" Mod : ");Serial.print(pc_data.Motor_mod);Serial.print(" Value : ");Serial.print(pc_data.Value);
    Serial.println(" ");
  }
     
}

void send_data_I2C(int data){ //I2C 데이터를 서버로 보내는 함수
  Receive[0]=174;
  Receive[1]=164;
  Receive[2]='I';
  Receive[3]=pc_data.Motor_mod;
  Receive[4]=hex2dec.hex[0];
  Receive[5]=hex2dec.hex[1];
  crc_16_val.check_sum = CRC16_MODBUS(sending_crc_check, 6);
  Receive[6]=crc_16_val.b[0];
  Receive[7]=crc_16_val.b[1];
  
 // Wire.beginTransmission(Slave);
  for(int i=0; i<8; i++){
   // Wire.write(sending_crc_check[i]);
  }
 // Wire.endTransmission();
  Wire.write(Receive,8);
  Serial.println("Send I2C Message!");
}

/*  
void Send_to_M(){

  for(int i=0; i<8; i++){
    Wire.write(Receive[i]);
  }
}


void read_processing(){
  char processing = 0;
  processing = Serial.read();
  for(int i=0; i<8; i++){
    processing_data[i]=(int)((processing>>i)&0x01);
  }
}
*/

void setup() {
  Wire.begin(Slave);
  Wire.onRequest(send_data_I2C); // 마스터의 데이터 전송 요구 시 처리할 함수 : send_data_I2C
  Serial.begin(115200);
  Serial1.begin(115200);
  
}

void loop() {
  if(Serial1.available()){
    Ring_buff();
  }
  /*
  if(Serial.available()){
    for(int i = 0; i<8; i++){
      processing_data[i] = processing_data[i+1];
    }
    processing_data[7] = Serial.read();
    cnt++;
    if(cnt%8==0){
    crc_16_val.check_sum = CRC16_MODBUS(Receive, 6);
    Receive[6] = crc_16_val.b[0];
    Receive[7] = crc_16_val.b[1];
    
    for(int i = 0; i<8; i++){
      Receive[i] = processing_data[i];
      Serial.print((int)Receive[i]);
    }
    Serial.println(" ");
    cnt = 0;
   }
  }
  Serial.print(" Mod : "); Serial.print(pc_data.Motor_mod); Serial.print(" Value : "); Serial.println(pc_data.Value);
  */
}
