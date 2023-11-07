//Biblioteca mfrc522
//exemplo dumpinfo
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>

#define SS_PIN 9
#define RST_PIN 10
LiquidCrystal_I2C lcd(0x27,16,2);
time_t t;
MFRC522 mfrc522(SS_PIN, RST_PIN); //Criar instância MFRC522


//Declarar aqui um byte accessUID[4] com o endereço da tag rfid
byte accessUID[4] = {00, 00, 00, 00};
int concedeAcesso();
bool pass;
int hora, minuto, segundo;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Aproxime a tag");
  t = now();
  lcd.setCursor(0, 1);
  lcd.print(hour(t));
  lcd.print(":");
  lcd.print(minute(t));
  Serial.println("Escaneie o PICC para ver UID, SAK, tipo e blocos de dados");
}

void loop() {
  t = now();
  hora = hour(t);
  minuto = minute(t);
  segundo = second(t);
  int aux = hour(t)*60 + minute(t);
  aux = aux - (hora*60 + minuto);
  lcd.clear();
  lcd.print(segundo);
  lcd.print(":");
  lcd.print(minuto);
  lcd.print(":");
  lcd.print(hora);
  
  //Reseta o loop se nenhum cartão está presente no sensor
  if( ! mfrc522.PICC_IsNewCardPresent()){
    return;
  }

  //Seleciona um dos cartões
  if( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }

  if(concedeAcesso()){
    if(pass == false){
      t = now();
      hora = hour(t);
      minuto = minute(t);
      segundo = second(t);
      lcd.print("Acesso concedido às ");
      //lcd.clear();
      lcd.print(hora);
      lcd.print(":");
      lcd.print(minuto);
      lcd.print(":");
      lcd.print(segundo);
      pass = true;
    }
    else{
      t = now();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tempo decorrido: ");
      lcd.setCursor(0, 1);

      //procedimento para escrever a hora da maneira correta
      int aux = hour(t)*3600 + minute(t)*60 + second(t);
      aux = aux - (hora*3600 + minuto*60 + segundo);
      lcd.print(aux/3600);
      aux -= (aux/3600)*3600;
      lcd.print(":");
      lcd.print(aux/60);
      aux -= (aux/60)*60;
      lcd.print(":");
      lcd.print(aux);

      pass = false;
    }
  }
  else {
    lcd.print("Acesso negado.");
  }

}

int concedeAcesso(){
  int i;
  for(i=0; i<4; i++){
    if(mfrc522.uid.uidByte[i] != accessUID[i])
      return 0;
  }
  return 1;
}