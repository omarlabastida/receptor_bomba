#include <SPI.h> 
#include <RF24.h>
//#include <avr/wdt.h> 

/***
  Receptor de control de bomba
  Create By José Omar Labastida Cuadra
  V 2.0.0
***/

RF24 radio(8, 10);// CE y CSN

uint8_t identificacion[5] = {'L','B','0','0','2'};
const int Bomba = 5;
const int ActivadoR = A0;
const int ActivadoV = A1; 
const int Cisterna_LED = 2;
const int Cisterna_sen = 3;


bool bombaActiva = false;
//const int Cisterna_sen = 4;
int RED = 6;
int GREEN = 9;

int nivel = 11110;
int sen_cis;
int sen_cis_aux = 0;
int contSist = 0;
int cont=0;
int contOff = 0;
int parp=1;
bool constante = true;
unsigned long previousMillis = 0;  // Almacena el último tiempo registrado para el parpadeo
const long interval = 500;  // Intervalo de parpadeo de 500 milisegundos
void setup() {

  cli();//Desactiva la ejecución de interrupciones hasta nueva orden, se ejecuta con la libreria Arduino.h.
  TCCR1A=0; //Se configura el registro TCCR1A del microcontrolador con valor a 0
  TCCR1B=0; //Se configura el registro TCCR1B del microcontrolador con valor a 0
  //Se configura el tiempo de interrupción para el RTOS de 400Hz
  //Frecuencia de Interupción=16 000 000/(PRESCALER)*(Registro de comparación +1), el +1 está ahí porque el registro de comparación está indexado a cero
  // Registro de comparación= (16 000 000) / PRESCALER *(tiempo deseado)) - 1 (debe ser <65536)

  OCR1A=155;//valor de registro de comparacion =155 para interrupción de 400Hz tomando en cuenta que el preescaler esta configurado en 256

  
  
  TCCR1B |= (1<<WGM12);//Se coloca en 1 el bit WGM12 del registro TCCR1B, se inicializa el  modo CTC (Clear Timer on Compare) que reinicia el timer1 cuando los valores del registro OCR1A y el registro TMR1 coinciden
  //Se configura el preescaler en  256 //pagina 157 del Datasheet de Atmega2560
  TCCR1B |= (0<<CS10);
  TCCR1B |= (0<<CS11);
  TCCR1B |= (1<<CS12);
  
  TIMSK1=(1<<OCIE1A);//Se coloca en 1 el bit OCIE1A del registro TIMSK1, Habilita la temporización del comparador de tiempo.
  sei();//Reactiva las interrupciones, se ejecuta con la libreria Arduino.h

  
  Serial.begin(9600);
  //wdt_enable(WDTO_2S);  // Habilita el Watchdog Timer con un tiempo de 2 segundos
  pinMode(Bomba, OUTPUT);
  pinMode(ActivadoR, OUTPUT);
  pinMode(ActivadoV, OUTPUT);
  pinMode(Cisterna_LED, OUTPUT);
  pinMode(Cisterna_sen, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);

  digitalWrite(Bomba, LOW);
  analogWrite(RED, 255);
  analogWrite(GREEN, 0);
  delay(500);
  analogWrite(RED, 255);
  analogWrite(GREEN, 255);
  delay(500);
  analogWrite(RED, 0);
  analogWrite(GREEN, 255);
  delay(500);
  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  delay(1000);

  configurationRadio();
  bombaActiva = true;
  
}

void loop() {
   
  //if (radio.available()) {
    radio.read(&nivel, sizeof(nivel));  // Lee los datos de la radio
    sen_cis_aux = digitalRead(Cisterna_sen);  // Lee el estado del sensor de cisterna

    if ((nivel == 11110) || (nivel == 11111) || (nivel == 11112) || (nivel == 11113)) {
      Serial.println("Entras");
      cont = 0;
      contOff = 0;
      testSenCist();
      // Condiciones para diferentes niveles
      if (nivel == 11110) {
        bombaActiva = true;
        parpadeoBomba();  // Controla el parpadeo de la bomba
        controlaBomba(sen_cis, true);  // Controla el estado de la bomba
      } else if (nivel == 11111) {
        bombaActiva = true;//Controla el status de la bomba en el nivel amarillo
        analogWrite(RED, 255);
        analogWrite(GREEN, 0);
        controlaBomba(sen_cis, true);
      } else if (nivel == 11112) {
        analogWrite(RED, 255);
        analogWrite(GREEN, 255);
        controlaBomba(sen_cis, bombaActiva);
      } else if (nivel == 11113) {
        bombaActiva = false;
        analogWrite(RED, 0);
        analogWrite(GREEN, 255);
        controlaBomba(sen_cis, false);
      }
      //wdt_reset(); 
    }
  //}

  // Resetea el Watchdog Timer en cada iteración del loop
  //wdt_reset();  
}

// Función para parpadear el LED de la bomba sin usar delay()
void parpadeoBomba() {
  unsigned long currentMillis = millis();  // Obtén el tiempo actual en milisegundos

  // Si ha pasado el tiempo definido (500 ms en este caso), realiza el parpadeo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Actualiza el tiempo previo
      pinMode(Bomba, OUTPUT);

    if (parp) {
      analogWrite(RED, 0);
      analogWrite(GREEN, 0);
    } else {
      analogWrite(RED, 255);
      analogWrite(GREEN, 0);
    }
    parp = !parp;  // Cambia el estado de parpadeo
  }

}

void controlaBomba(int sensor, bool activa) {
  if (sensor == 1 && activa) {
    digitalWrite(Bomba, HIGH);
    //digitalWrite(ActivadoV, HIGH);
    //digitalWrite(ActivadoR, LOW);
  } else {
    digitalWrite(Bomba, LOW);
    //digitalWrite(ActivadoV, LOW);
    //digitalWrite(ActivadoR, HIGH);
  }
}

void configurationRadio(){
  radio.begin();
  radio.openReadingPipe(0, identificacion);
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void testSenCist(){
  if(sen_cis_aux = 1){
    if(contSist <= 20) contSist ++;
  }else{
    contSist = 0;
  };
  if(contSist >= 15){
    sen_cis = 1;
  }else{
    sen_cis = 0;
  };
}
