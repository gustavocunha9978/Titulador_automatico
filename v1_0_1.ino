#include <LiquidCrystal.h>
#include <ESP32Servo.h>

// Definição dos pinos
#define SERVO_PIN 25 // porta 25
#define LED_PIN 21 // porta 21
#define LDR_PIN 34 // porta 23
#define S0_PIN 27 
#define S1_PIN 14 
#define S2_PIN 13 
#define S3_PIN 12 
#define SENSOR_OUT_PIN 26

// Constantes
const int RED_FREQUENCY_THRESHOLD = 400;
const int BLUE_FREQUENCY_THRESHOLD = 400;
const int GREEN_FREQUENCY_THRESHOLD = 400;
const int LDR_THRESHOLD = 1000;
const int SERVO_START_POS = 0;
const int SERVO_END_POS = 360;
const unsigned long SERVO_DELAY = 35;
const unsigned long GOTAS_DELAY = 500;
const unsigned long PRINT_DELAY = 1000;  
const int SERVO_MOVE_DELAY = 5; // Tempo em milissegundos entre movimentos do servo

Servo servo; 
int redFrequency = 0;
int blueFrequency = 0;
int greenFrequency = 0;
float gotas = 0;
unsigned long lastServoMove = 0;
unsigned long lastGotasUpdate = 0;
unsigned long lastPrintTime = 0;
int currentServoPos = SERVO_START_POS;
bool movingToEndPos = true;
int ldrValue = 0;

void setup() {
  // Configurações iniciais
  pinMode(S0_PIN, OUTPUT);
  pinMode(S1_PIN, OUTPUT);
  pinMode(S2_PIN, OUTPUT);
  pinMode(S3_PIN, OUTPUT);
  pinMode(SENSOR_OUT_PIN, INPUT);
  digitalWrite(S0_PIN, HIGH);
  digitalWrite(S1_PIN, LOW);

  servo.attach(SERVO_PIN);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  servo.write(SERVO_START_POS);
  Serial.begin(115200);
} 

void moveServoToPosition(int position) {
  // Move o servo para a posição especificada
  servo.write(position);
  delay(500); // Pequeno delay para permitir que o servo se mova
}

void loop() {
  // Leitura do sensor de cor
  digitalWrite(S2_PIN, LOW);
  digitalWrite(S3_PIN, LOW);
  redFrequency = pulseIn(SENSOR_OUT_PIN, LOW);
  digitalWrite(S2_PIN, HIGH);
  digitalWrite(S3_PIN, LOW);
  greenFrequency = pulseIn(SENSOR_OUT_PIN, LOW);
  digitalWrite(S2_PIN, LOW);
  digitalWrite(S3_PIN, HIGH);
  blueFrequency = pulseIn(SENSOR_OUT_PIN, LOW);
  
  // Parada do sistema
  if (redFrequency >= RED_FREQUENCY_THRESHOLD) {
    Serial.println(" Deu Vermelho ");
    while (true);
  }

  if (greenFrequency >= GREEN_FREQUENCY_THRESHOLD){
    Serial.println(" Deu Verde ");
    while (true);
  }

  if (blueFrequency >= BLUE_FREQUENCY_THRESHOLD){
    Serial.println(" Deu Azul ");
    while (true);
  }

  // Controle do LED e contagem de gotas
  if (analogRead(LDR_PIN) > LDR_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);
    if (millis() - lastGotasUpdate > GOTAS_DELAY) {
      gotas += 0.05;
      lastGotasUpdate = millis();
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // Controle do servo para gotejamento
  if (millis() - lastServoMove > SERVO_MOVE_DELAY) {
    if (movingToEndPos && currentServoPos < SERVO_END_POS) {
      currentServoPos++;
      servo.write(currentServoPos);
    } else if (!movingToEndPos && currentServoPos > SERVO_START_POS) {
      currentServoPos--;
      servo.write(currentServoPos);
    }

    if (currentServoPos == SERVO_END_POS || currentServoPos == SERVO_START_POS) {
      movingToEndPos = !movingToEndPos; // Muda a direção do movimento
    }

    lastServoMove = millis();
  }

  // Limitar a frequência de impressão
  if (millis() - lastPrintTime > PRINT_DELAY) {
    Serial.print("Frequencia vermelho = ");
    Serial.println(redFrequency);
    Serial.print("Frequencia azul = ");
    Serial.println(blueFrequency);
    Serial.print("Frequencia verde = ");
    Serial.println(greenFrequency);
    ldrValue = analogRead(LDR_PIN);
    Serial.print("Valor LDR = ");
    Serial.println(ldrValue);
    Serial.print("Quantidade de gotas = ");
    Serial.println(gotas, 2);
    Serial.println("----------------------------------------------");

    lastPrintTime = millis();
  }
}
