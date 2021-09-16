//--------------------------------MONITORAMENTO CONTÍNUO DE ESCADAS ROLANTES------------------------------------
//Autor: Caio César de Miranda Viana
//Para cada novo equipamendo deve ser guardado o Mac Address

//--------------------------------Inclusão das bibliotecas utilizadas------------------------------------
#include <ESP8266WiFi.h> 
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
const String enderecoMac = WiFi.macAddress();

//-----------------------------------Cofiguração da rede Wireless----------------------------------------
//Modifique os campos abaixo para alterar os dados do roteador

const char* ssid = "nome_da_rede"; // Substitua nome_da_rede pelo nome da sua rede wireless
const char* pwd = "senha_rede"; // Substitua senha_rede pela senha da sua rede wireless
const char* enviaPost = "http://"; // Coloque aqui o endereço da sua API
const char* testeGetx = "http://"; // Coloque aqui o endereço da sua API
const String testeGet = String(testeGetx) + String(enderecoMac);

HTTPClient http;

//-------------------------------------Tipo de escada---------------------------------------------
//Escada tipo 1 (monitorando apenas um contator)- 0 desligado 1 ligado
//Escada tipo 2 (monitorando dois contatores)- 0: desligado 2: descendo 3: subindo 4: erro
const int tipoEscada=2;

//-------------------------------------Configuração dos Pinos---------------------------------------------
//ATENÇÃO: o Pino D0 não deve ser utilizado pois ele não possiu função Pull UP.
//É possível utilizar, porém em PULL DOWN e inserindo no pino 5v ao invés de GND
// ATENÇÃO o Pino D3 não deve ser utilizado. 
//Pois se estiver com nível lógico 0 durante a inicialização, o microcontrolador entra em moodo boot
// ATENÇÃO o Pino D4 não deve ser utilizado. 
//Pois se estiver com nível lógico 0 durante a inicialização, o microcontrolador entra em moodo boot
//ATENÇÃO: o Pino D8 não deve ser utilizado pois ele não possiu função Pull UP.
//É possível utilizar, porém em PULL DOWN e inserindo no pino 5v ao invés de GND

//-----------------------------------------Pinos I/O Digitais-------------------------------------------------
//const int PinD0 = 16; // Caso utilizar, usar em Pull Down
const int PinD1 = 5; //
const int PinD2 = 4; //
//const int PinD3 = 0; // Nunca utilizar
//const int PinD4 = 2; // Nunca utilizar
const int PinD5 = 14; //
const int PinD6 = 12; //
const int PinD7 = 13; //
//const int PinD8 = 15; //Caso utilizar, usar em Pull Down

//---------------------------------------------Pino Analógico----------------------------------------------------
//const int PinA0 = 17; //Pino de leitura e escrita ANALÓGICA

//----------------------------------------Variáveis de estado dos pinos------------------------------------------

int estadoPinD1;
int estadoPinD2;
int estadoPinD5;
int estadoPinD6;
int estadoPinD7;
int estadoAntPinD1;
int estadoAntPinD2;
int estadoAntPinD5;
int estadoAntPinD6;
int estadoAntPinD7;
int indice0;
int indice1;
int indice2;
int indice3;
int indice4;

//----------------------------------------Variáveis de tempo------------------------------------------
unsigned long tempo1 = 0;
unsigned long tempo2 = 0;

//-----------Função para Leitura dos pinos--------
void leituraPinos()
{
estadoPinD1 = digitalRead(PinD1);
estadoPinD2 = digitalRead(PinD2);
estadoPinD5 = digitalRead(PinD5);
estadoPinD6 = digitalRead(PinD6);
estadoPinD7 = digitalRead(PinD7);
}

//-----------Função para enviar mudança de estados--------
void enviaEstados()
{
String JsonEstados=("");

//Escada tipo 1 (monitorando apenas um contator)- 0 desligado 1 ligado
if (tipoEscada==1)
{
indice0= estadoPinD1; // Contador escada 1
indice1= estadoPinD2; // Contador escada 2
indice2= estadoPinD5; // Contador escada 3
indice3= estadoPinD6; // Contador escada 4
indice4= estadoPinD7; // Contador escada 5
}

//Escada tipo 2 (monitorando dois contatores)- 0 desligado 2 descendo 3 subindo 4 erro
if (tipoEscada==2)
{

// Contador 1 e 2 escada 1
 if (estadoPinD1==0 && estadoPinD2==0) // 2 Contatores desligados: Escada desligada
{
indice0= 0;
}
if (estadoPinD1==0 && estadoPinD2==1) // Contator 1 desligado e contator 2 ligado: Escada descendo
{
indice0= 2;
}
if (estadoPinD1==1 && estadoPinD2==0) // Contator 1 ligado e contator 2 desligado: Escada subindo
{
indice0= 3;
}
if (estadoPinD1==1 && estadoPinD2==1) // Contator 1 desligado e contator 2 desligado: ERRO!
{
indice0= 4;
}

// Contador 1 e 2 escada 2
if (estadoPinD5==0 && estadoPinD6==0)  // 2 Contatores desligados: Escada desligada
{
indice1= 0;
}
if (estadoPinD5==0 && estadoPinD6==1) // Contator 1 desligado e contator 2 ligado: Escada descendo
{
indice1= 2;
}
if (estadoPinD5==1 && estadoPinD6==0) // Contator 1 ligado e contator 2 desligado: Escada subindo
{
indice1= 3;
}
if (estadoPinD5==1 && estadoPinD6==1) // Contator 1 desligado e contator 2 desligado: ERRO!
{
indice1= 4;
}
indice4= estadoPinD7; // Pino reserva
}

// Cria documento Json
StaticJsonDocument<512> doc;

doc["t"] = enderecoMac;

JsonArray p = doc.createNestedArray("p");

JsonObject p_0 = p.createNestedObject();
p_0["i"] = 0;
p_0["s"] = indice0;
p_0["v"] = 0;

JsonObject p_1 = p.createNestedObject();
p_1["i"] = 1;
p_1["s"] = indice1;
p_1["v"] = 0;

JsonObject p_2 = p.createNestedObject();
p_2["i"] = 2;
p_2["s"] = indice2;
p_2["v"] = 0;

JsonObject p_3 = p.createNestedObject();
p_3["i"] = 3;
p_3["s"] = indice3;
p_3["v"] = 0;

JsonObject p_4 = p.createNestedObject();
p_4["i"] = 4;
p_4["s"] = indice4;
p_4["v"] = 0;

serializeJson(doc, JsonEstados);
Serial.println(JsonEstados);
   
//-------------Conecta no servidor e envia os dados-------------

http.begin(enviaPost);

http.addHeader("content-type", "application/json");

String body = JsonEstados;
int httpCode = http.POST(body);

}

void setup() {
delay(1000);
Serial.begin(115200);
Serial.println("Iniciando...");

//-----------Conecta no roteador wireless--------
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, pwd);
delay(2000);

Serial.print("Endereco IP: ");
Serial.println(WiFi.localIP());
delay(5000);
 
//---------------Configuração dos Pinos-------------------
pinMode(PinD1, INPUT_PULLUP);
pinMode(PinD2, INPUT_PULLUP);
pinMode(PinD5, INPUT_PULLUP);
pinMode(PinD6, INPUT_PULLUP);
pinMode(PinD7, INPUT_PULLUP);

//----------------Leitura inicial dos Pinos----------------------
estadoAntPinD1 = digitalRead(PinD1);
estadoAntPinD2 = digitalRead(PinD2);
estadoAntPinD5 = digitalRead(PinD5);
estadoAntPinD6 = digitalRead(PinD6);
estadoAntPinD7 = digitalRead(PinD7);

const String enderecoMac = WiFi.macAddress();
//------------Leitura e envio dos estados inciais dos pinos------------
leituraPinos();
enviaEstados();
delay(500);
}

void loop(){
tempo1 = millis();

//---------------Filtro para oscilações 60 segundos---------------
//----------------Primeira leitura dos Pinos----------------------
Serial.println("PRIMEIRA LEITURA");
Serial.println(digitalRead(PinD1));
Serial.println(digitalRead(PinD2));
Serial.println(digitalRead(PinD5));
Serial.println(digitalRead(PinD6));
 
leituraPinos();
delay(70000);
//delay(100);

if(
 //------------Segunda leitura dos Pinos-----------------
(estadoPinD1 == digitalRead(PinD1)) && 
(estadoPinD2 == digitalRead(PinD2)) && 
(estadoPinD5 == digitalRead(PinD5)) && 
(estadoPinD6 == digitalRead(PinD6)) && 
(estadoPinD7 == digitalRead(PinD7)) 
  ) 
{
Serial.println("SEGUNDA LEITURA: IGUAIS");
//------------Segunda leitura dos Pinos-----------------
if(
(estadoPinD1 == estadoAntPinD1) && 
(estadoPinD2 == estadoAntPinD2) && 
(estadoPinD5 == estadoAntPinD5) && 
(estadoPinD6 == estadoAntPinD6) && 
(estadoPinD7 == estadoAntPinD7) 
  ) 
{
Serial.println("Comparacao com leitura inicial: IGUAIS");
   
}
else
{
Serial.println("Comparacao com leitura inicial: DIFERENTES");  
Serial.println("ENVIADO PARA O SERVIDOR");
enviaEstados(); 

estadoAntPinD1 = digitalRead(PinD1);
estadoAntPinD2 = digitalRead(PinD2);
estadoAntPinD5 = digitalRead(PinD5);
estadoAntPinD6 = digitalRead(PinD6);
estadoAntPinD7 = digitalRead(PinD7);

}
}
else 
{
Serial.println("SEGUNDA LEITURA: DIFERENTES");
}

   
//--------Testa conexão wireless---------------
if(WiFi.status() != WL_CONNECTED)
{
Serial.println("Sem conexao reiniciando");
delay(5000);
ESP.reset();
}

//---------------------------Testa conexão com servidor--------------------
if (millis() > 604800000)
{
Serial.println("Tempo de uma semana atingido reiniciando");
delay(5000);
ESP.reset();
}

else
{
if (tempo1 - tempo2 >= 140000)
{
leituraPinos();
enviaEstados();
http.begin(testeGet); 
int httpCode = http.GET(); 
Serial.println("WATCHDOG ACIONADO, ENVIANDO TESTE DE COMUNICACAO");    
tempo2 = tempo1;
}
else
{
Serial.println("Tempo inferior");
}
}
} 
