#include <Arduino.h>
#include <SPI.h>
#include "SdFat.h"
//CS腳位，預設是SS，以Uno來說就是腳位10，如果有多個SPI設備，請設不同的腳位
#define SD_CS_PIN SS

SdFat SD;
int count = 1;
int ecgdata = 0;
int x = 0;
int Numberoffiles = 0;
String fname = "";
int GSRsensorValue=0;
const int GSR = A1;
File myFile;

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  //判斷SD模組初始化是否成功
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  pinMode(6, INPUT); // Setup for leads off detection LO +
  pinMode(7, INPUT); // Setup for leads off detection LO -

}
void loop() {
  GSRsensorValue = analogRead(GSR);
  if ((digitalRead(6) == 1) || (digitalRead(7) == 1)) {
    // Serial.println(String(fname)+String(",")+'!'+String(",")+String(GSRsensorValue));
    ecgdata = 9999;
  }
  else {
    // send the value of analog input 0:
    // Serial.println(String(fname)+String(",")+analogRead(A0)+String(",")+String(GSRsensorValue));
    ecgdata = analogRead(A0);
  }
  if (count%600 == 0) //設定單個檔案中有幾筆數據
  {
    x = x+1;
  }
  fname = "data" + String(x) + ".txt";
  myFile = SD.open(fname, FILE_WRITE);
  // 如果成功打開，就寫入文字
  if (myFile) {
    myFile.println(String(ecgdata)+String(",")+String(GSRsensorValue));
    // 關閉檔案
    myFile.close();
  } else{
    // 如果無法開啟檔案，就在監控視窗顯示訊息
    Serial.println("error opening " + fname);
  }
  Serial.println(String(ecgdata)+","+String(GSRsensorValue));
  // Serial.println(String(count)+","+fname+","+String(ecgdata)+","+String(GSRsensorValue));
  // delay(1000);
  count++;
}

void serialEvent() {
  while (Serial.available()) {
    int incomingValue = Serial.parseInt(); 
    // Serial.println(incomingValue);
    if (incomingValue == -1) {                 //輸出TXT檔案內數據
      while (true){
        fname = "data" + String(x) + ".txt" ;
        myFile = SD.open(fname, FILE_READ);
        if (myFile) {
          Serial.println(fname);
          //一直讀取檔案內容，直到沒有為止
          while (myFile.available()) {
            Serial.write(myFile.read()); //把讀到的內容顯示在監控視窗
          }
          // 關閉檔案
          myFile.close();
        } else {
          // 如果無法開啟檔案，就在監控視窗顯示訊息
          // Serial.println("error opening " + fname);
          break;
        }
        x++;
      }
      Serial.println(x);
      Serial.println("end");
      delay(10000);
    }
    if (incomingValue == -2){               //輸出檔案數量
      while (true){
        fname = "data" + String(Numberoffiles) + ".txt" ;
        myFile = SD.open(fname, FILE_READ);
        if (myFile) {
          Serial.println(fname);
          myFile.close();
        } else {
          // 如果無法開啟檔案，就在監控視窗顯示訊息
          // Serial.println("error opening " + fname);
          break;
        }
        Numberoffiles++;
      }
      Serial.println(Numberoffiles-1);
      Serial.println("end");
      delay(1000);
    }
  }
}