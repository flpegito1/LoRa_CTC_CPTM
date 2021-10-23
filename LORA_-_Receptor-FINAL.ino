/*
Essa programação é referente ao monitoramento das entradas de duas linhas de alimentação.
A principal denominada CTC1 e sua redundância denominada CTC2.
Para isso é utilizado a comunicação via LoRa entre o campo e uma base operacional e,
entre o dispositivo receptor e a interface gráfica é utilizado via Wi-fi com protocolo ModbusIP.

Para dúvidas e/ou informações entrar em contato com a equipe desenvolvedora do código pelo e-mail: felipe.egito@cptm.sp.gov.br

*/
#include "images.h"
#include <SPI.h> //responsável pela comunicação serial
#include <LoRa.h> //responsável pela comunicação com o WIFI Lora
#include <Wire.h>  //responsável pela comunicação i2c
#include "SSD1306.h" //responsável pela comunicação com o display


// Definição dos pinos 
#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)

#define BAND    915E6  //Frequencia do radio - podemos utilizar ainda : 433E6, 868E6, 915E6

//parametros: wi-fi
#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>


//parametros: address,SDA,SCL 
SSD1306 display(0x3c, 4, 15); //construtor do objeto que controlaremos o display

String packSize = "--";
String packet ;

//Declarações das variáveis do supervisório
const int CTC1 = 100;
const int CTC2 = 101;
const int COM = 102;

int CTC1S;
int CTC2S;



//ModbusIP object
ModbusIP mb;

void setup() {
  
 //configura os pinos como saida
  pinMode(12,OUTPUT); // CTC 1
  pinMode(13,OUTPUT); // CTC 2
  pinMode(25,OUTPUT); // COMUNICAÇÃO 
  pinMode(16,OUTPUT); //RST do oled
  digitalWrite(16, LOW);    // reseta o OLED
  delay(50); 
  digitalWrite(16, HIGH); // enquanto o OLED estiver ligado, GPIO16 deve estar HIGH

  //Definição do tamanho e da fonte do display
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);
  
  delay(1500);
  display.clear();
  
  Serial.begin(9600);                   //  Inicialização da comunicação serial
  while (!Serial);

  Serial.println("Comunicando \n");

// Dados do wi-fi
WiFi.begin("House", "#hdgprgp#123");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi Conectado");  
  Serial.println("Endereço IP : ");
  Serial.println(WiFi.localIP());

//Configuração doprotocolo ModbusIP
mb.server();

//Leitura das entradas digitais pelo supervisório
  mb.addIsts(CTC1);
  mb.addIsts(CTC2);
  mb.addIsts(COM);

  SPI.begin(SCK,MISO,MOSI,SS); //inicia a comunicação serial com o Lora
  LoRa.setPins(SS,RST,DI00); //configura os pinos que serão utlizados pela biblioteca (deve ser chamado antes do LoRa.begin)
  
  //inicializa o Lora com a frequencia específica.
  if (!LoRa.begin(BAND)) {
    display.drawString(0, 0, "Falha ao iniciar LoRa!");
    Serial.println("Falha ao iniciar LoRa! \n");
    display.display();
    while (1);
  }

  //indica no display que inicilizou corretamente.
  display.drawString(0, 0, "LoRa Iniciado com sucesso!");
  Serial.println("LoRa Iniciado com sucesso! \n");
  display.drawString(0, 10, "Aguardando comunicação");
  display.display();
  delay(1000);

  //LoRa.onReceive(cbk);
  LoRa.receive(); //habilita o Lora para receber dados
}

void loop() {

//Inicia a comunicação ModbusIP
  mb.task();
   delay(10);
  
//parsePacket: checa se um pacote foi recebido
//retorno: tamanho do pacote em bytes. Se retornar 0 (ZERO) nenhum pacote foi recebido

  int packetSize = LoRa.parsePacket();

//caso tenha recebido pacote chama a função para configurar os dados que serão mostrados em tela
  
 if (packetSize) { 
    cbk(packetSize);  
  }

//Condição para falha nos dois CTCs
if (packet == "1") {

//apaga o conteúdo do display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(40, 30, "CTC 1 - FALHA");
  display.drawString(40, 40, "CTC 2 - FALHA");
  Serial.println("CTC 1 - FALHA \n");
  Serial.println("CTC 2 - FALHA \n");
  display.display(); //mostra o conteúdo na tela

  CTC1S = 1;
  CTC2S = 1;
  mb.Ists(CTC1, CTC1S);
  mb.Ists(CTC2, CTC2S);
  
  digitalWrite(12, HIGH); // ALARME CTC 1
  digitalWrite(13, HIGH); // ALARME CTC 2

  delay(10);
}

//Condição para falha no CTC1
else if (packet == "2"){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(40, 30, "CTC 1 - FALHA");
  Serial.println("CTC 1 - FALHA \n");
  display.drawString(40, 40, "CTC 2 - NORMAL");
  Serial.println("CTC 2 - NORMAL \n");
  display.display(); //mostra o conteúdo na tela

  CTC1S = 1;
  CTC2S = 0;
  mb.Ists(CTC1, CTC1S);
  mb.Ists(CTC2, CTC2S);
  
  digitalWrite(12, HIGH); // ALARME CTC 1
  digitalWrite(13, LOW); // ALARME CTC 2
  
  delay(10);
}

//Condição para falha no CTC2
else if (packet == "3"){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(40, 30, "CTC 1 - NORMAL");
  Serial.println("CTC 1 - NORMAL \n");
  display.drawString(40, 40, "CTC 2 - FALHA");
  Serial.println("CTC 2 - FALHA \n");
  display.display(); //mostra o conteúdo na tela

  CTC1S = 0;
  CTC2S = 1;
  mb.Ists(CTC1, CTC1S);
  mb.Ists(CTC2, CTC2S);
  
  digitalWrite(12, LOW); // ALARME CTC 1
  digitalWrite(13, HIGH); // ALARME CTC 2
  
  delay(10);
}

//Condição de normalidade nos dois CTCs
else if (packet == "4"){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(40, 30, "CTC 1 - NORMAL");
  Serial.println("CTC 1 - NORMAL \n");
  display.drawString(40, 40, "CTC 2 - NORMAL");
  Serial.println("CTC 2 - NORMAL \n");
  display.display(); //mostra o conteúdo na tela

  CTC1S = 0;
  CTC2S = 0;
  mb.Ists(CTC1, CTC1S);
  mb.Ists(CTC2, CTC2S);
  
  digitalWrite(12, LOW); // ALARME CTC 1
  digitalWrite(13, LOW); // ALARME CTC 2
  
  delay(10);
}
}
//função responsável por recuperar o conteúdo do pacote recebido
//parametro: tamanho do pacote (bytes)
void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC); //transforma o tamanho do pacote em String para imprimirmos
  for (int i = 0; i < packetSize; i++) { 
    packet += (char) LoRa.read(); //recupera o dado recebido e concatena na variável "packet"
  }
}
