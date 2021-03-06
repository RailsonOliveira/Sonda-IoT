//_______________________Radiômetro (BH1750)
#include <Wire.h>
#include <ErriezBH1750.h>
BH1750 sensor(LOW);
uint16_t LuxRad;
float WmRad=0;

//_______________________Pressão atmosférica (BMP280)
#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h> 
Adafruit_BMP280 bmp; 
const int ALTITUDE = 551; //definicao do valor da altitudeuepb cct 535
int cont2;
float absoluta=0; 
float relativaM=0; 
float absolutaM=0; 

//_______________________Temperatura_umidade
#include <DHT_U.h>
#define DHTTYPE    DHT22                          
#define DHTPIN 4                                  
DHT_Unified dht(DHTPIN, DHTTYPE);                  
uint32_t delayMS;  
int cont3;
float umidade;
float temperatura;
float umidadeM;
float temperaturaM;

//_________________________nivel
#include <HX711.h>  
#define CLK1   5 
#define DOUT1  18
HX711 Spres1;                        // define instancia do HX711
float calibration_factor = 41130; 
float P1;
float P2;

float NivelAgua =0;
float PressAgua =0;
float VolAgua =0;
float CapAgua =0;
int cont5;
//_________________________Bluetooth
#include "BluetoothSerial.h" 
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT; 

//_________________________dados
String DADOS;
int cont=0;
long TempoCorrido=0;


//__________________________wifi e protoclo HTTP
#include <HTTPClient.h>
#include <WiFi.h>      //biblioteca do Mode MCU wifi
const char* SSID = "Laboratorio"; // rede wifi
const char* PASSWORD = "12345678"; // senha da rede wifi
String BASE_URL = "http://200.129.69.20/mccontrol/datasms/add.php?data="; //tag da URL de envio dos dados
WiFiClient client; //DleclaraÃƒÂ§ÃƒÂ£o de objeto do cliente
HTTPClient http;   //declaraÃƒÂ§ÃƒÂ£o de objto de protocolo de troca de dados

String nome1 = "Ecosolo";
String nome2 = "LAPTOP";
String nome3 = "SERVITEC_SIVP";
String nome4 = "AndroidAP";
String nome5 = "RamboDetona";

int i =0;
String wifiname;

//constantes de salvamento de dados
String dados;
String LeituraSD;
String arquivoSemanal;
const char*  arquivoSemanal2;
const char*  arquivoUp1;
String arquivoUp;
int contadorDesalvamento;
int StringSalvamento=1;
int contUP=1; 

//_________________________________RTC
#include <Wire.h> 
#include <RTClib.h> 
RTC_DS3231 rtc; 
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String data;
String hora;
String dia;

//___________________________________SD card
#include "FS.h"
#include "SD.h"
#include "SPI.h"
String dadosPrint;
char dadosPrint1;

//declaraÃƒÂ§ÃƒÂ£o dos voids de processamento
void initSerial();
void initWiFi();
void httpRequest(String path);

//variaveis de buffer e contagem                                                                        
float bufferTank;
String systemID="DEVICE_02";

//______________________________________________________________________________
void setup() {
    Serial.begin(115200);
    //______________Radiômetro
    Wire.begin();
    sensor.begin(ModeContinuous, ResolutionLow);
    sensor.startConversion();
    //______________Barômetro
    bmp.begin(0x76);   
    //______________higrômetro/termômetro
    dht.begin();         //inicia sensor de temperatura e umidade do ar    
    //______________Nivel
    Spres1.begin(DOUT1, CLK1);                     
    Spres1.set_scale(calibration_factor);             // ajusta fator de calibração
    Spres1.tare();                          
    sensor_t sensor;
    //______________Bluetootch
     SerialBT.begin("Estacao Alcalitech_INSA_PaqTcPB");
}

void loop() {
initWiFi();
relogio();  
nivel();

cont=cont+1;
if(cont==100){
pressao_atmosferica();
umidade_temperatura();
lux();   
concatenador();
gerador_de_arquivo();
appendFile(SD, arquivoSemanal2, dados.c_str());
cont=0;
}
if (WiFi.status() == WL_CONNECTED){
upLoad();
}
delay(10);
}

void lux(){
    if (sensor.isConversionCompleted()) {
        LuxRad = sensor.read();
        WmRad=LuxRad*0.0075;
        //Serial.print("Light: ");
        //Serial.print(lux);
        //Serial.print(" Lux");
        //Serial.print("/");
        //Serial.print(Wm);
        //Serial.print(" W/m²");
        //Serial.println();
    }
}
void pressao_atmosferica(){
    absoluta=absoluta+(bmp.readPressure()*0.01);
    cont2=cont2+1;
    if(cont2==100){
      absolutaM=absoluta/100;
      relativaM=(absolutaM+(ALTITUDE/8.3));
      absoluta=0;
      cont2=0;
      //Serial.print(F("Pressão Absoluta: ")); //IMPRIME O TEXTO NO MONITOR SERIAL
      //Serial.print(absolutaM); //IMPRIME NO MONITOR SERIAL A PRESSÃO
      //Serial.print(" Pa (Pascal)"); //IMPRIME O TEXTO NO MONITOR SERIAL
      //Serial.println();
      //Serial.print(F("Pressão Relativa: ")); //IMPRIME O TEXTO NO MONITOR SERIAL  
      // Serial.print(relativaM); //IMPRIME NO MONITOR SERIAL A PRESSÃO
      // Serial.print(" Pa (Pascal)"); //IMPRIME O TEXTO NO MONITOR SERIAL
      // Serial.println();
    }
}
void umidade_temperatura(){
  sensors_event_t event;  
  cont3=cont3+1;
  dht.humidity().getEvent(&event); 
  umidade=umidade+event.relative_humidity;
  dht.temperature().getEvent(&event);
  temperatura=temperatura+event.temperature;
  
  if(cont3==100){
    umidadeM=umidade/100;
    temperaturaM=temperatura/100;
    //Serial.print("Umidade: ");                
    //Serial.print(umidadeM);
    //Serial.println("%");
    //Serial.print("Temperatura: ");            
    //Serial.print(temperaturaM);
    //Serial.println(" *C");
    umidade=0;
    temperatura=0;
    cont3=0;
  }
}
void nivel(){
  cont5=cont5+1;
  P2=P2+Spres1.get_units()+5.20;
  
    if(cont5==100){
  P2=P2/100;
  if(P2<0.1993){
  P2=0.1993;
  }
  P1 = ((P2-0.1993)/0.8924); 
  NivelAgua = P1;
  PressAgua = (NivelAgua*0.98);
  VolAgua=(0.000055*NivelAgua*NivelAgua)+(0.0435*NivelAgua)+0.0584;
  CapAgua =100-((20.08458-VolAgua)/20.08458)*100;
  cont5=0;
  P2=0;  
  Serial.println(NivelAgua);
  }
  
}

//________________HTTP REQUEST

void httpRequest(String path){
  String payload = makeRequest(path);

  if (!payload) {
    return;
  }
}

String makeRequest(String path){
  http.begin(BASE_URL + LeituraSD);
  int httpCode = http.GET();

  if (httpCode < 0) {
    return "";
  }
  if (httpCode != HTTP_CODE_OK) {
    return "";
  }
  String response =  http.getString();
  http.end();

  return response;
}

void initWiFi() {
  
    if (WiFi.status() != WL_CONNECTED) {
    int network = WiFi.scanNetworks();
    Serial.print("disponivel:");
    Serial.println(network);
    Serial.println("no-WIFI");
    digitalWrite(17,HIGH);
    digitalWrite(16,LOW);

      //Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "   OPEN ":"   Password Required");
          
      if(WiFi.SSID(i)= (nome1 || nome2 || nome3 || nome4|| nome5)){
      Serial.println("-----------i: ");
      Serial.println(i);
      
      if(WiFi.SSID(i)==nome1){//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  wifi1
      WiFi.begin("Ecosolo", "ecosolo15");
      Serial.print("uma rede compatÃƒÂ­vel:"); 
      Serial.println(WiFi.SSID(i));
      wifiname="RamboDetona";
      delay(10000);
      }
      
      if(WiFi.SSID(i)==nome2){//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  wifi1
      WiFi.begin("LAPTOP", "6765Qi22");
      Serial.print("uma rede compatÃƒÂ­vel:"); 
      Serial.println(WiFi.SSID(i));
      wifiname="LAPTOP";
      delay(10000);
      }

      if(WiFi.SSID(i)==nome3){//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  wifi1
      WiFi.begin("SERVITEC_SIVP", "RR79BYSIVP");
      Serial.print("uma rede compatÃƒÂ­vel:"); 
      Serial.println(WiFi.SSID(i));
      wifiname="Laboratorio";
      delay(10000);
      }

      if(WiFi.SSID(i)==nome4){//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  wifi1
      WiFi.begin("AndroidAP", "brck7528");
      Serial.print("uma rede compatÃƒÂ­vel:"); 
      Serial.println(WiFi.SSID(i));
      wifiname="AndroidAP";
      delay(10000);
      }

      if(WiFi.SSID(i)==nome5){//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  wifi1
      WiFi.begin("RamboDetona", "rambo1992");
      Serial.print("uma rede compatÃƒÂ­vel:"); 
      Serial.println(WiFi.SSID(i));
      wifiname="RamboDetona";
      delay(10000);
      }

      if (WiFi.status() != WL_CONNECTED) {
      Serial.println("somando");
      i=i+1;
      }
      if(i>network){
      i=0;
      }
    }
  }else{
  digitalWrite(17,LOW);
  digitalWrite(16,HIGH);
  }
}

void concatenador(){
DADOS=String(temperaturaM)+";"+String(WmRad)+";"+String(LuxRad)+";"+String(umidadeM)+";"+String(absolutaM)+";"+String(relativaM)+";"+String(ALTITUDE)+";"+String(NivelAgua)+";"+String(PressAgua)+";"+String(VolAgua)+";"+String(CapAgua)+";"+String("00/00/00")+";"+String("00:00:00")+";"+String("00");
//DADOS=String("1")+";"+String("2")+";"+String("3")+";"+String("4")+";"+String("5")+";"+String("6")+";"+String("7")+";"+String("8")+";"+String("9")+";"+String("10")+";"+String("11")+";"+String("12")+";"+String("13")+";"+String("14");
Serial.println(DADOS);
SerialBT.println(DADOS);
}

void relogio(){
  DateTime now = rtc.now(); //chama a funÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o do relÃƒÆ’Ã‚Â³gio
  data=((String)now.day()+"/"+(String)now.month()+"/"+(String)now.year());
  hora=((String)now.hour()+":"+(String)now.minute()+":"+(String)now.second());
  dia=daysOfTheWeek[now.dayOfTheWeek()];
}

//voids SD card
void readFile(fs::FS &fs, const char * path){
  File file = SD.open(path);
  if(!file){
  Serial.println("erro de upload");
  }
  if( contUP==50){
  contUP=0;
  }
  while(file.available()){
  Serial.println("up33");
  LeituraSD=file.readString();
  Serial.println(LeituraSD);
  httpRequest(file.readString());
  file.close();
  }  
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  File file = SD.open(path, FILE_APPEND);
  if(!file){
  Serial.println("eeo ds");
  return;
  }
  //if(file.print(message)){
  if(file.print(message)){
  Serial.print("contadorDesalvamento:");
  Serial.print(contadorDesalvamento);
  Serial.println();
  Serial.print("StringSalvamento:");
  Serial.print(StringSalvamento);
  Serial.println();
  Serial.print("arquivoSemanal2:");
  Serial.print(arquivoSemanal2);
  Serial.println();
  contadorDesalvamento=contadorDesalvamento+1;
  gerador_de_arquivo();
  DateTime now = rtc.now();
    if(now.hour()==23 && now.minute()==59){
    contadorDesalvamento=0;
    StringSalvamento=0;
    }
      if(contadorDesalvamento==80){
      contadorDesalvamento=0;
      StringSalvamento=StringSalvamento+1;  
      }
    }

  file.close();
}
void gerador_de_arquivo(){
arquivoSemanal="/"+(String)dia+(String)StringSalvamento+".txt";
arquivoSemanal2 =arquivoSemanal.c_str();
}
void upLoad(){
arquivoUp="/"+(String)dia+(String)contUP+".txt";
arquivoUp1=arquivoUp.c_str();
readFile(SD,arquivoUp1);
Serial.println(arquivoUp1);
contUP=contUP+1; 
}
