import processing.serial.*;
Serial port; 
char[] Data_pkg={184,174,'I',0,0,0,0,0};
int flag = 0;
int board_late = 115200;
String comport_result = "COM16";


int slider1X = 100;        // 슬라이드 버튼의 x 좌표
int slider1Y = 200;  // 슬라이드 버튼의 y 좌표
int slider2X = 100;
int slider2Y = 400;
int sliderWidth = 450;  // 슬라이드 버튼의 너비
int sliderHeight = 50;  // 슬라이드 버튼의 높이
int sliderMin = 0;  // 슬라이드 범위의 최소값
int sliderMax = 600;  // 슬라이드 범위의 최대값
int sliderValue1 = 0;  // 슬라이드 버튼1의 현재 값
int sliderValue2 = 0;  // 슬라이드 버튼2의 현재 값
int Velocity = 0;
int Position = 0;
boolean sliderDragging1 = false;  // 슬라이드 버튼 드래그 상태 확인용 변수
boolean sliderDragging2 = false;
int sliderOffset = 0;  // 슬라이드 버튼 드래그 시 위치 보정을 위한 변수



void setup() {
  size(1000, 700);
  flag = 2;
  String portName ;
  println(Serial.list());
  port = new Serial(this,"COM9",115200);
  port.bufferUntil('\n');
}


void draw() {
 background(230);
 textSize(30);
 text("Velocity",100,150);
 text("Position",100,370);
 Text();
 Button();
 Stop();
 send_Velocity();
 send_Position();

  
  
}

void Data_Control(){
  
  }
  
////////////////////////////////////////////////////////  
void Stop(){

   if(Velocity == 0) flag = 2; delay(10);
   print("flag : ");println(flag);
   if(flag == 2){
       Data_pkg[3]=5;
       Data_pkg[4]=0;
       Data_pkg[5]=0;
       Data_pkg[6]=0;
       Data_pkg[7]=0;
       for(int i=0; i<8; i++){
         port.write(Data_pkg[i]);
       }
       delay(10);
    }
 }
 
void send_Velocity(){
  if(flag != 2){
    if(Velocity > 0)      Data_pkg[3]=1;
    else if(Velocity < 0) Data_pkg[3]=2;
   
    Data_pkg[4]=(char)(Velocity>>8);
    Data_pkg[5]=(char)(Velocity%256);
    Data_pkg[6]=0;
    Data_pkg[7]=0;
    for(int i=0; i<8; i++){
      port.write(Data_pkg[i]);
    }
    //print("speed : ");println(Data_pkg);
    delay(10);
   }
}

void send_Position(){
  if(Position >= 0){
    Data_pkg[3]=3;
    Data_pkg[4]=(char)(Position>>8);
    Data_pkg[5]=(char)(Position%256);
  }
  else{
    Data_pkg[3]=4;
    Data_pkg[4]=(char)((Position*-1)>>8);
    Data_pkg[5]=(char)((Position*-1)%256);
  }
  Data_pkg[6]=0;
  Data_pkg[7]=0;
  for(int i=0; i<8; i++){
    port.write(Data_pkg[i]);
  }
  delay(10);
}
/////////////////////////////////////////////////////////////////


void Text(){
   if(!sliderDragging1)text(Velocity, 230,150);
   if(!sliderDragging2)text(Position, 230,370);
   if(sliderDragging1) text(Velocity, 230,150);
   if(sliderDragging2) text(Position, 230,370);
   
   text("RPM", 300, 150);
   text("Flag", 400, 80);
   text(flag, 460,80);
  }

void Button(){
    // 슬라이드 바 그리기
    fill(200);
    rect(slider1X, slider1Y, sliderWidth, sliderHeight);
    // 슬라이드 버튼 그리기
    fill(150);
    rect(slider1X + sliderValue1, slider1Y, sliderHeight, sliderHeight);
    
    fill(200);
    rect(slider2X, slider2Y, sliderWidth, sliderHeight);
    fill(150);
    rect(slider2X + sliderValue2, slider2Y, sliderHeight, sliderHeight);
    
    fill(150);
    rect(710, 180, 140, 80);
    fill(255,0,0);
    textSize(20);
    text("EMERGENCY \n       STOP", 730, 210);
    
    fill(200);
    rect(600, 50, 130, 70);
    fill(0);
    textSize(20);
    text("Comport", 630, 75);
    text(comport_result, 640, 108);
    
    fill(200);
    rect(800, 50, 130, 70);
    fill(0);
    textSize(20);
    text("Board late", 825, 75);
    text(board_late, 835, 108);
    
  }
  
void mousePressed() {
  // 슬라이드 버튼을 클릭한 경우
  if (mouseX > slider1X + sliderValue1 && mouseX < slider1X + sliderValue1 + sliderHeight &&
      mouseY > slider1Y && mouseY < slider1Y + sliderHeight) {
    sliderDragging1 = true;
    sliderDragging2 = false;
    flag = 0;
    print("flag : ");println(flag);
    sliderOffset = mouseX - slider1X - sliderValue1;
    
  }
  
  if (mouseX > slider2X + sliderValue2 && mouseX < slider2X + sliderValue2 + sliderHeight &&
       mouseY > slider2Y && mouseY < slider2Y + sliderHeight) {
    sliderDragging2 = true;
    sliderDragging1 = false;
    flag = 1;
    print("flag : ");println(flag);
    sliderOffset = mouseX - slider2X - sliderValue2;
    
  }
  
  if(mouseX > 710 && mouseX < 850 &&
     mouseY > 180 && mouseY < 260) {
    flag = 2;
    print("flag : ");println(flag);
    fill(50);
    rect(710, 180, 140, 80);
    
  }
}

void mouseDragged() {
  // 슬라이드 버튼이 드래그 상태일 때 위치 업데이트
  if (sliderDragging1) {
    sliderValue1 = constrain(mouseX - slider1X - sliderOffset, 0, sliderWidth - sliderHeight);
    Velocity = sliderValue1 - 200;
  }
  
  if (sliderDragging2) {
    sliderValue2 = constrain(mouseX - slider2X - sliderOffset, 0, sliderWidth - sliderHeight);
    Position = sliderValue2;
  }
}

void mouseReleased() {
  // 슬라이드 버튼의 드래그 상태를 해제
  sliderDragging1 = false;
  sliderDragging2 = false;
}
