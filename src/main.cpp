#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

const int chipSelect = 4;

#define RGBLED_PIN 48 // Pin where NeoPixels are connected
#define MAX 32
Adafruit_NeoPixel strip(1, RGBLED_PIN, NEO_GRB + NEO_KHZ400);
int j;

#define PNSS  10	//10 (DUET) 
#define PRST  14  
#define PBUSY_IRQ  21	
#define PDIO0 -1  
#define PDIO1 9 	//9
#define PDIO2 -1  
#define PDIO3 -1
#define PSCK  12
#define PMOSI 13
#define PMISO 11  

/*
#include "SX1262.h"
SX1262 LoRa1262(PNSS, PRST, PBUSY_IRQ, PDIO1);	//define a object of class SX1262.
loRa_Para_t	lora_para;	//define a struct to hold lora parameters.
int8_t power_table[32]={-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
uint8_t sf_table[8]={LORA_SF5,LORA_SF6,LORA_SF7,LORA_SF8,LORA_SF9,LORA_SF10,LORA_SF11,LORA_SF12};
uint8_t bw_table[10]={LORA_BW_7,LORA_BW_10,LORA_BW_15,LORA_BW_20,LORA_BW_31,LORA_BW_41,LORA_BW_62,LORA_BW_125,LORA_BW_250,LORA_BW_500};
uint8_t cr_table[4]={LORA_CR_4_5,LORA_CR_4_6,LORA_CR_4_7,LORA_CR_4_8};
uint8_t tx_buf[200]={"LoRa Message Ok"};
uint16_t tx_cnt = 0;
uint8_t rx_buf[200]={"LoRa Message OK"};
uint16_t rx_cnt = 0;
uint16_t rx_size = 0;
uint8_t val;
uint8_t state;
bool temp;

*/

char databuff[200];

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
String mqtt_server = "202.231.217.181";
String mqtt_port = "1883";
String mqtt_user = "aaaa";
String mqtt_pass = "bbbb";
String mqtt_topic = "/device1/ch1";

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
      sprintf(shtml,"invalid format, plese Usage http://x.x.x.x/?dt=yyyy-mm-dd&tm=hh:mm:ss");
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

	SPI.begin(SCK, MISO, MOSI, PNSS); //sck, miso, mosi, ss
  Wire.begin(4,3);
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP
/*
	lora_para.rf_freq    = 920000000; //434500000
	lora_para.tx_power   = 10;	//-9~22
	lora_para.lora_sf    = LORA_SF7;
	lora_para.band_width = LORA_BW_125;
	lora_para.code_rate  = LORA_CR_4_5;
	lora_para.payload_size = sizeof(tx_buf);
  LoRa1262.RxInit();

  temp = LoRa1262.Init(&lora_para);
	if(0 == temp)
	{
		Serial.println("#LoRa1262 init fail!");
	}	
  else {
    Serial.println("#LoRa1262 init Ok.");
  }
*/
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
  sprintf(databuff,"#Hello %d, Num LED=%d, color=%d (%c,%c,%c)",j,strip.numPixels(),j%8,(((j%8)&(1<<0))==0?'-':'R'), (((j%8)&(1<<1))==0?'-':'G'), (((j%8)&(1<<2))==0?'-':'B'));
  Serial.println(databuff);
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

