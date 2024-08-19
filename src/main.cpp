#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
//#include <mySD.h>
//File myFile;

// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 4;

#define RGBLED_PIN 48 // Pin where NeoPixels are connected
#define MAX 32
Adafruit_NeoPixel strip(1, RGBLED_PIN, NEO_GRB + NEO_KHZ400);
int j;

#include "SX1262.h"
#define PNSS  15	//10  
#define PRST  14  
#define PBUSY_IRQ  21	
#define PDIO0 -1  
#define PDIO1 9 	//9
#define PDIO2 -1  
#define PDIO3 -1  
SX1262 LoRa1262(PNSS, PRST, PBUSY_IRQ, PDIO1);	//define a object of class SX1262.
loRa_Para_t	lora_para;	//define a struct to hold lora parameters.
int8_t power_table[32]={-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
uint8_t sf_table[8]={LORA_SF5,LORA_SF6,LORA_SF7,LORA_SF8,LORA_SF9,LORA_SF10,LORA_SF11,LORA_SF12};
uint8_t bw_table[10]={LORA_BW_7,LORA_BW_10,LORA_BW_15,LORA_BW_20,LORA_BW_31,LORA_BW_41,LORA_BW_62,LORA_BW_125,LORA_BW_250,LORA_BW_500};
uint8_t cr_table[4]={LORA_CR_4_5,LORA_CR_4_6,LORA_CR_4_7,LORA_CR_4_8};
uint8_t tx_buf[]={"LoRa Message Ok"};
uint16_t tx_cnt = 0;
uint8_t rx_buf[20]={"LoRa Message OK"};
uint16_t rx_cnt = 0;
uint16_t rx_size = 0;
uint8_t val;
uint8_t state;
bool temp;

#include <FS.h>
#include <SD.h>
#define SCK 12
#define MISO  11
#define MOSI  13
#define SS  PNSS
#define SS2 16
//#include <SPI.h>


#include "RTClib.h"
RTC_PCF8563 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
uint8_t dD, dM, dY;
uint8_t dh, dm, ds;
uint8_t dow;
uint8_t finsleep = 0;

//WiFi
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <EEPROM.h>
#include <esp_task_wdt.h>
#define WDT_TIMEOUT WDTO_8S
//#include <WiFiClientSecure.h>
#define WAITWIFI  60  //60 = 30s
String mac4 = "";
String mqtt_id = "0001";
String mqtt_server = "20.231.17.181";
String mqtt_port = "1883";
String mqtt_user = "ttmq";
String mqtt_pass = "1122";
String mqtt_topic = "/device1/vibration1";

#define VIP1  192
#define VIP2  168
#define VIP3  1
#define VIP4  100
//0x42,0x49,0x54,0xf3,0xab,0x9e
#define VMAC1 0x42
#define VMAC2 0x49
#define VMAC3 0x54
#define VMAC4 0xf3
#define VMAC5 0xab
#define VMAC6 0x9e
HTTPClient http;
WiFiMulti wifiMulti;
uint8_t isWiFi = 0;
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

// SSID and PW for Config Portal
char host[] = "duet";
String ssid = host + String(ESP_getChipId(), HEX).substring(String(ESP_getChipId(), HEX).length()-4);
const char* password = "1234567890";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

//declare function
IPAddress myip(VIP1, VIP2, VIP3, VIP4); //ESP static ip
IPAddress mygw(VIP1, VIP2, VIP3, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress mysn(255, 255, 255, 0);  //Subnet mask
IPAddress mydns(VIP1, VIP2, VIP3, 1);  //DNS
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional
IPAddress apip(192,168,4,1);
IPAddress apgw(192,168,4,1);
IPAddress apsn(255,255,255,0);
byte isDHCP = 1;

char html[12000];
char shtml[2000];
char fns[40];
ulong wifistarttime = 0;

uint8_t pwd_c;
String wifissid = "LambdaNu";
String wifipassword = "0818090157";
char pwd_ch;
uint8_t cretry = 0;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

#define MIN_AP_PASSWORD_SIZE    8

#define SSID_MAX_LEN            32
//From v1.0.10, WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN            64

WebServer server(80);
//WiFiServer serverAP(80);

// Variable to store the HTTP request
String header;

/* Style */
String style =
"<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
"input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
"#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
"#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
"form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
".btn{background:#3498db;color:#fff;cursor:pointer}</style>";

/*
 * Login page
 */

 const char* loginIndex1 = 
"";

void handleCMD()
{
  //char html[200];
  uint8_t dd;
  uint8_t dm2;
  uint8_t dy;
  uint8_t hh;
  uint8_t mm;
  uint8_t ss;

  finsleep = 0;

  sprintf(shtml,"<br><br>");
  
  if (server.hasArg("dt") && server.hasArg("tm")) {
    if (server.arg("dt").length() == 10 && server.arg("tm").length() == 8) {
      dd = atoi(server.arg("dt").substring(8,10).c_str());
      dm2 = atoi(server.arg("dt").substring(5,7).c_str());
      dy = atoi(server.arg("dt").substring(2,4).c_str());
      dD = dd; dM = dm2; dY = dy;
      hh = atoi(server.arg("tm").substring(0,2).c_str());
      mm = atoi(server.arg("tm").substring(3,5).c_str());
      ss = atoi(server.arg("tm").substring(6,8).c_str());
      dh = hh; dm = mm; ds = ss;
      sprintf(shtml,"Request Set Date Time = %02d/%02d/%04d %02d:%02d:%02d<br>Current Date Time = %02d/%02d/%04d %02d:%02d:%02d",dd,dm2,dy+2000,hh,mm,ss,dD,dM,2000+dY,dh,dm,ds);
      rtc.adjust(DateTime(dY+2000,dM,dD,dh,dm,ds));
    }
    else {
      sprintf(shtml,"invalid format, plese Usage http://x.x.x.x/cmd?dt=yyyy-mm-dd&tm=hh:mm:ss");
    }
  }
  else {
    strcpy(html,"unknown cmd");
  }

  DateTime now = rtc.now();

  sprintf(html,
  "<html><head><title>Duet Configuration</title>"
  "</head><body text='#%02x%02x%02x'>"
  "<span id=\"headpage\">"
  "<h3>Lambda Nu Duet Demo (ทดสอบบอร์ด)</h3>"
  "%s<br><br>"
  "</span>"
  "Date : <input value='%04d-%02d-%02d' name=dt id=dt> (yyyy-mm-dd)<br>"
  "Time : <input value='%02d:%02d:%02d' name=tm id=tm> (HH:mm:ss)<br><br>"
  "<input type=button value=\" Set Date Time \" onclick=\"window.open('?dt='+dt.value+'&tm='+tm.value,'_top');\"><br>"
  "<br><br>Now Time %02d/%02d/%04d %02d:%02d:%02d น. (%s)"
  "<br><br>(auto refresh every 10 seconds)"
  "<script>setTimeout(function(){ window.open('/','_self'); }, 10000);</script>"
  "</body></html>\n",
    now.hour()*11,now.minute()*4,now.second()*4,
    shtml,
    now.year(),now.month(),now.day(),now.hour(),now.minute(),now.second(),
    now.day(),now.month(),now.year()+543,now.hour(),now.minute(),now.second(),    
    daysOfTheWeek[now.dayOfTheWeek()]
  );

  server.sendHeader("Connection", "close");
  server.send(200, "text/html; charset=utf-8", html); 

}


void enable_wifi()
{

  Serial.printf("default wifi-ssid=%s\r\n",wifissid.c_str());    
  Serial.printf("default wifi-pass=%s\r\n",wifipassword.c_str());    
  isDHCP = 1;
  Serial.printf("\r\n\r\nSaved IP Info:\r\nIP = %d.%d.%d.%d\r\nSub Netmask = %d.%d.%d.%d\r\nGateway IP = %d.%d.%d.%d\r\nDNS IP = %d.%d.%d.%d\r\nDHCP = %s\r\n",
    myip[0],myip[1],myip[2],myip[3],
    mysn[0],mysn[1],mysn[2],mysn[3],
    mygw[0],mygw[1],mygw[2],mygw[3],
    mydns[0],mydns[1],mydns[2],mydns[3],
    isDHCP==1?"Yes":"No"
  );

  isWiFi = 0;
  mac4 = WiFi.macAddress().substring(12,14)+WiFi.macAddress().substring(15,17);
  Serial.print("MAC :");
  Serial.print(WiFi.macAddress());  
  Serial.print(" ("+mac4+")");
  ssid = host + mac4;  

  if (isDHCP == 1) {
      Serial.println("\r\nStation uses DHCP");
  }
  else {
    if (!WiFi.config(myip, mygw, mysn, mydns, primaryDNS)) {
      Serial.println("STA Failed to configure");
    }
    else {
      Serial.printf("\r\nManual Station Info:\r\nIP = %d.%d.%d.%d\r\nSub Netmask = %d.%d.%d.%d\r\nGateway IP = %d.%d.%d.%d\r\nDNS IP = %d.%d.%d.%d\r\n",
        myip[0],myip[1],myip[2],myip[3],
        mysn[0],mysn[1],mysn[2],mysn[3],
        mygw[0],mygw[1],mygw[2],mygw[3],
        mydns[0],mydns[1],mydns[2],mydns[3]
      );
    }
  }

  // Configures static IP address
  WiFi.softAPConfig(apip,apgw,apsn);

  //wifiMulti.addAP(ssid.c_str(),password);
  WiFi.softAP(ssid.c_str(),password);

  WiFi.mode(WIFI_AP_STA);

  Serial.printf("\r\n#AP SSID = %s\r\n",ssid.c_str());
  Serial.printf("#AP Pass = %s\r\n",password);
  Serial.print("#AP IP = ");
  Serial.println(WiFi.softAPIP());

  WiFi.setHostname(host);
  WiFi.begin(wifissid.c_str(), wifipassword.c_str());

  Serial.printf("#WiFi SSID = %s\r\n",wifissid.c_str());
  Serial.printf("#WiFi Pass = %s\r\n",wifipassword.c_str());

  int wc = 0;
  while (WiFi.status() != WL_CONNECTED) {
    esp_task_wdt_reset();
    delay(500);
    Serial.print(".");
    if (++wc >= WAITWIFI) {
      Serial.printf("\r\nBreak.. Can't connect WIFI SSID=%s with password '%s', please re-config via AP.\r\n",wifissid.c_str(),wifipassword.c_str());
      delay(5000);
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    isWiFi = 1;
    wifistarttime = millis();
    Serial.println("");
    Serial.println("#WiFi connected");
    Serial.print("#Client  IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("#Client  SN address: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("#Client  GW address: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("#Client DNS address: ");
    Serial.println(WiFi.dnsIP());
  }

  server.on("/", HTTP_GET, []() {
    handleCMD();
  });
    
  //server.onNotFound(handleCMD);

  server.begin();

}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void setup() {
  delay(2000);
  // put your setup code here, to run once:
  EEPROM.begin(512);
  Serial.begin(115200);
  esp_task_wdt_init(WDT_TIMEOUT, true);  // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);  
  enable_wifi();

 	SPI.setFrequency(2000000);

  pinMode(SS,OUTPUT);
  pinMode(SS2,OUTPUT);

	SPI.begin(SCK, MISO, MOSI, PNSS); //sck, miso, mosi, ss
  Wire.begin(4,3);
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP

	lora_para.rf_freq    = 920000000; //434500000
	lora_para.tx_power   = 10;	//-9~22
	lora_para.lora_sf    = LORA_SF7;
	lora_para.band_width = LORA_BW_125;
	lora_para.code_rate  = LORA_CR_4_5;
	lora_para.payload_size = sizeof(tx_buf);

  temp = LoRa1262.Init(&lora_para);
	if(0 == temp)
	{
		Serial.println("#LoRa1262 init fail!");
	}	
  else {
    Serial.println("#LoRa1262 init Ok.");
  }
  digitalWrite(SS,HIGH);

  if (! rtc.begin()) {
    Serial.println("#Couldn't find RTC.");
  }
  else {
    Serial.println("#RTC init Ok.");
  }
  Serial.flush();

  if (rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.start();
  pinMode(17,OUTPUT);
  pinMode(18,OUTPUT);
  pinMode(19,OUTPUT);
  pinMode(45,OUTPUT);
  pinMode(46,OUTPUT);
  pinMode(47,OUTPUT);
  pinMode(35,OUTPUT);
  pinMode(36,OUTPUT);
  pinMode(37,OUTPUT);
  pinMode(38,OUTPUT);
  pinMode(39,OUTPUT);
  pinMode(40,OUTPUT);
  pinMode(41,OUTPUT);
  pinMode(42,OUTPUT);
  pinMode(1,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);

  digitalWrite(SS2,LOW);
  Serial.println("#Init SDCard...");
 	//SPI.setFrequency(2000000);
	//SPI.begin(SCK, MISO, MOSI, PNSS); //sck, miso, mosi, ss
  if(!SD.begin()){
      Serial.println("Card Mount Failed");
  }
  else {
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("#No SD card attached");
    }
    else {
      Serial.print("SD Card Type: ");
      if(cardType == CARD_MMC){
          Serial.println("MMC");
      } else if(cardType == CARD_SD){
          Serial.println("SDSC");
      } else if(cardType == CARD_SDHC){
          Serial.println("SDHC");
      } else {
          Serial.println("UNKNOWN");
      }
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

#ifdef TESTSD
    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    //testFileIO(SD, "/test.txt");
#endif  
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

    Serial.println("Reading config file: /setup.txt");

    File file = SD.open("/setup.txt");
    if(!file){
        Serial.println("#Failed to open config-file, use default!!");
    }
    else {
      while(file.available()){
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() > 5) {
          if(line.charAt(0) != '#' && line.charAt(0) != ';') {
            Serial.println(line);
          }
        }
      }
    }
    file.close();
  }
  digitalWrite(SS2,HIGH);

}

void loop() {
  esp_task_wdt_reset();
  server.handleClient();
  digitalWrite(15,digitalRead(15)^1);
  digitalWrite(16,digitalRead(16)^1);
  digitalWrite(17,digitalRead(17)^1);
  digitalWrite(18,digitalRead(18)^1);
  digitalWrite(19,digitalRead(19)^1);
  digitalWrite(20,digitalRead(20)^1);
  digitalWrite(45,digitalRead(45)^1);
  digitalWrite(46,digitalRead(46)^1);
  digitalWrite(47,digitalRead(47)^1);
  digitalWrite(35,digitalRead(35)^1);
  digitalWrite(36,digitalRead(36)^1);
  digitalWrite(37,digitalRead(37)^1);
  digitalWrite(38,digitalRead(38)^1);
  digitalWrite(39,digitalRead(39)^1);
  digitalWrite(40,digitalRead(40)^1);
  digitalWrite(41,digitalRead(41)^1);
  digitalWrite(42,digitalRead(42)^1);
  digitalWrite(1,digitalRead(1)^1);
  digitalWrite(2,digitalRead(2)^1);
  digitalWrite(5,digitalRead(5)^1);
  digitalWrite(6,digitalRead(6)^1);
  digitalWrite(7,digitalRead(7)^1);
  digitalWrite(8,digitalRead(8)^1);
  // put your main code here, to run repeatedly:
  j = (j%8)==0?j+1:j;
  Serial.printf("#Hello %d, Num LED=%d, color=%d (%c,%c,%c)\r\n",j,strip.numPixels(),j%8,(((j%8)&(1<<0))==0?'-':'R'), (((j%8)&(1<<1))==0?'-':'G'), (((j%8)&(1<<2))==0?'-':'B'));
  //colorWipe(strip.Color((((i%8)&(1<<0))==0?0:1)*128, (((i%8)&(1<<1))==0?0:1)*128, (((i%8)&(1<<2))==0?0:1)*128), 0);  
  for(int i = 0; i<(j==2?20:MAX); ++i) {
    strip.setPixelColor(0,strip.Color((((j%8)&(1<<0))==0?0:1)*i, (((j%8)&(1<<1))==0?0:1)*i, (((j%8)&(1<<2))==0?0:1)*i));
    strip.show();
    server.handleClient();
    delay((j==2?80:40));
    if(digitalRead(0)==0) break;
  }
  for(int i = MAX-1; i>=0; --i) {
    strip.setPixelColor(0,strip.Color((((j%8)&(1<<0))==0?0:1)*i, (((j%8)&(1<<1))==0?0:1)*i, (((j%8)&(1<<2))==0?0:1)*i));
    strip.show();
    delay((j==2?80:40));
    server.handleClient();
    if(digitalRead(0)==0) break;
  }
  //delay(500);
  ++j;

  DateTime now = rtc.now();
  Serial.printf(PSTR("%04d-%02d-%02d %02d:%02d:%02d (%s)\r\n\r\n"),
    now.year(),
    now.month(),
    now.day(),
    now.hour(),
    now.minute(),
    now.second(),
    daysOfTheWeek[now.dayOfTheWeek()]
  );
}

