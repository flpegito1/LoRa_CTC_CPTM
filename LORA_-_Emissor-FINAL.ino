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


//variáveis responsáveis por armazenar o valor das entradas CTC 1 e CTC 2)
int CTC1 = 34;
int CTC2 = 35;


//parametros: address,SDA,SCL 
SSD1306 display(0x3c, 4, 15); //construtor do objeto que controla o display


void setup()
{

  //configura os pinos como saida
  pinMode(16,OUTPUT); //RST do oled
  pinMode(25,OUTPUT);
  pinMode(CTC1,INPUT); // ENTRADA CTC 1
  pinMode(CTC2,INPUT); // ENTRADA CTC 2
  
  digitalWrite(16, LOW);    // reseta o OLED
  delay(50); 
  digitalWrite(16, HIGH); // enquanto o OLED estiver ligado, GPIO16 deve estar HIGH

  display.init(); //inicializa o display
  display.flipScreenVertically(); 
  display.setFont(ArialMT_Plain_10); //configura a fonte para um tamanho maior

  delay(1500);
  display.clear(); //apaga todo o conteúdo da tela do display

  Serial.begin(9600);                   //  Inicialização da comunicação serial
  while (!Serial);

  Serial.println("Comunicando \n");
  
  SPI.begin(SCK,MISO,MOSI,SS); //inicia a comunicação serial com o Lora
  LoRa.setPins(SS,RST,DI00); //configura os pinos que serão utlizados pela biblioteca (deve ser chamado antes do LoRa.begin)
  
  //inicializa o Lora com a frequencia específica.
  if (!LoRa.begin(BAND))
  {
    display.drawString(0, 0, "Falha ao iniciar LoRa!");
    Serial.println("Falha ao iniciar LoRa! \n");
    display.display(); //mostra o conteúdo na tela
    while (1);
  }
  //indica no display que inicilizou corretamente.
  display.drawString(0, 0, "LoRa Iniciado com sucesso!");
  Serial.println("LoRa Iniciado com sucesso! \n");
  display.display(); //mostra o conteúdo na tela
  delay(1000);
}

void loop()
{

//Condição para falha nos dois CTCs
  if (digitalRead(CTC1) == LOW && digitalRead(CTC2)== LOW) {
    //apaga o conteúdo do display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(0, 0, "CTC 1 - FALHA");
  display.drawString(0, 10, "CTC 2 - FALHA");
  Serial.println("CTC 1 - FALHA \n");
  Serial.println("CTC 2 - FALHA \n");
  display.display(); //mostra o conteúdo na tela
   
  //beginPacket : abre um pacote para adicionarmos os dados para envio
  LoRa.beginPacket();
  //print: adiciona os dados no pacote
  LoRa.print("1");
  //endPacket : fecha o pacote e envia
  LoRa.endPacket(); //retorno= 1:sucesso | 0: falha

  digitalWrite(25, HIGH);   // liga o LED indicativo

  
  delay (1000);
  }

//Condição para falha no CTC1
  else if (digitalRead(CTC1) == LOW && digitalRead(CTC2)== HIGH) {
    //apaga o conteúdo do display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(0, 0, "CTC 1 - FALHA");
  Serial.println("CTC 1 - FALHA \n");
  display.drawString(0, 10, "CTC 2 - NORMAL");
  Serial.println("CTC 2 - NORMAL \n");
  display.display(); //mostra o conteúdo na tela
   
  //beginPacket : abre um pacote para adicionarmos os dados para envio
  LoRa.beginPacket();
  //print: adiciona os dados no pacote
  LoRa.print("2");
  //endPacket : fecha o pacote e envia
  LoRa.endPacket(); //retorno= 1:sucesso | 0: falha

  digitalWrite(25, HIGH);   // liga o LED indicativo

  delay (1000);
  }

//Condição para falha no CTC2
  else if (digitalRead(CTC1) == HIGH && digitalRead(CTC2)== LOW) {
    //apaga o conteúdo do display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
 display.drawString(0, 0, "CTC 1 - NORMAL");
  Serial.println("CTC 1 - NORMAL \n");
  display.drawString(0, 10, "CTC 2 - FALHA");
  Serial.println("CTC 2 - FALHA \n");
  display.display(); //mostra o conteúdo na tela
   
  //beginPacket : abre um pacote para adicionarmos os dados para envio
  LoRa.beginPacket();
  //print: adiciona os dados no pacote
  LoRa.print("3");
  //endPacket : fecha o pacote e envia
  LoRa.endPacket(); //retorno= 1:sucesso | 0: falha

  digitalWrite(25, HIGH);   // liga o LED indicativo

  delay (1000);
  }

//Condição de normalidade nos dois CTCs
  else if (digitalRead(CTC1) == HIGH && digitalRead(CTC2)== HIGH) {
    //apaga o conteúdo do display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(0, 0, "CTC 1 - NORMAL");
  Serial.println("CTC 1 - NORMAL \n");
  display.drawString(0, 10, "CTC 2 - NORMAL");
  Serial.println("CTC 2 - NORMAL \n");
  display.display(); //mostra o conteúdo na tela
   
  //beginPacket : abre um pacote para adicionarmos os dados para envio
  LoRa.beginPacket();
  //print: adiciona os dados no pacote
  LoRa.print("4");
  //endPacket : fecha o pacote e envia
  LoRa.endPacket(); //retorno= 1:sucesso | 0: falha

  digitalWrite(25, HIGH);   // liga o LED indicativo
  
  delay (1000);
  }

//Condição de falha na comunicação
  else {
 display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(0, 0, "FALHA - COMUNICAÇÃO");
  Serial.println("FALHA DE COMUNICAÇÃO \n");
  display.display(); //mostra o conteúdo na tela 
 
  //beginPacket : abre um pacote para adicionarmos os dados para envio
  LoRa.beginPacket();
  //print: adiciona os dados no pacote
  LoRa.print("0");
  //endPacket : fecha o pacote e envia
  LoRa.endPacket(); //retorno= 1:sucesso | 0: falha
  
  delay (1000);
  }
}