//exemplo dumpinfo
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>

#define SS_PIN 9
#define RST_PIN 10
//**************************************
//Substituir endereço lcd para 0x3F
LiquidCrystal_I2C lcd(0x27,16,2);
MFRC522 mfrc522(SS_PIN, RST_PIN); //Criar instância MFRC522

typedef struct tag{
  byte UID[4];
  bool pass;
  int hora, minuto, segundo;
}TAG;

time_t t;
int selecionaTAG();
int i;

//DECLARAR OS CARTÕES AQUI
TAG tag[2];
tag[0].UID = {00,00,00,00};
tag[0].pass = false;
tag[1].UID = {11,11,11,11};
tag[1].pass = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Aproxime a tag.");
  t = now();
  lcd.setCursor(0, 1);
  lcd.print(hour(t));
  lcd.print(":");
  lcd.print(minute(t));
}

void loop() {
  //Reseta o loop se nenhum cartão está presente no sensor
  if( ! mfrc522.PICC_IsNewCardPresent()){
    lcd.setCursor(0, 1);
    lcd.print(hour(t));
    lcd.print(":");
    lcd.print(minute(t));
    return;
  }

  //Seleciona um dos cartões
  if( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }
  
  //Procedimento para identificar qual cartão está sendo passado no leitor
  i = selecionaTAG();
  if(i == -1){
    lcd.print("Acesso negado.");
    return;
  }

  if(tag[i].pass == false){
    t = now();
    tag[i].hora = hour(t);
    tag[i].minuto = minute(t);
    tag[i].segundo = second(t);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID: ");
    lcd.print(tag[i].UID[0]);
    lcd.print(tag[i].UID[1]);
    lcd.print(tag[i].UID[2]);
    lcd.print(tag[i].UID[3]);
    
    lcd.setCursor(0, 1);
    lcd.print("HORA:");
    lcd.print(tag[i].hora);
    lcd.print(":");
    lcd.print(tag[i].minuto);
    lcd.print(":");
    lcd.print(tag[i].segundo);
    
    tag[i].pass = true;
    delay(2000);
  }
  else{
    t = now();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID:");
    lcd.print(tag[i].UID[0]);
    lcd.print(tag[i].UID[1]);
    lcd.print(tag[i].UID[2]);
    lcd.print(tag[i].UID[3]);
    
    lcd.setCursor(0, 1);
    lcd.print("TEMPO:");
    //procedimento para escrever a hora da maneira correta (HORA:MINUTO:SEGUNDO)
    int aux = hour(t)*3600 + minute(t)*60 + second(t);
    aux = aux - (tag[i].hora*3600 + tag[i].minuto*60 + tag[i].segundo);
    
    lcd.print(aux/3600);
    aux -= (aux/3600)*3600;
    lcd.print(":");
    lcd.print(aux/60);
    aux -= (aux/60)*60;
    lcd.print(":");
    lcd.print(aux);

    tag[i].pass = false;
    delay(2000);
  }
}

//Escolhe qual TAG está no leitor
int selecionaTAG(){
  if(mfrc522.uid.uidByte[0] == tag[0].UID[0] 
  && mfrc522.uid.uidByte[1] == tag[0].UID[1] 
  && mfrc522.uid.uidByte[2] == tag[0].UID[2] 
  && mfrc522.uid.uidByte[3] == tag[0].UID[3] &&)
    return 0;
  else if(mfrc522.uid.uidByte[0] == tag[1].UID[0] 
  && mfrc522.uid.uidByte[1] == tag[1].UID[1] 
  && mfrc522.uid.uidByte[2] == tag[1].UID[2] 
  && mfrc522.uid.uidByte[3] == tag[1].UID[3] &&)
    return 1;
  else
    return -1;
}
