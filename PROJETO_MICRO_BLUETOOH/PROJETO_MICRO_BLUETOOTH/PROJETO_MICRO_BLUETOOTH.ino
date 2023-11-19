//Configurações Iniciais
  //Definição de Bibliotecas
    #include <Arduino.h> //Arduino
    #include <SPI.h> //Sensor RFID
    #include <MFRC522.h> //Sensor RFID
    #include <Wire.h> //Módulo LCD
    #include <LiquidCrystal_I2C.h> //Módulo LCD
    #include <Stepper.h> //Motor de Passo
    #include <BluetoothSerial.h> //Bluetooth

  //Configurações do Bluetooth
    //Manipulação de Erros
      #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
      #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
      #endif

      #if !defined(CONFIG_BT_SPP_ENABLED)
      #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
      #endif

  //Mapeamento de Hardware
    #define SS_PIN 5 //Sensor RFID (SDA)
    #define RST_PIN 2 //Sensor RFID (RST)
    #define LEDG_PIN 15 //LED Verde
    #define LEDR_PIN 4 //LED Vermelho
    #define BUZZ_PIN 17 //Buzzer
    #define REL_PIN 13 //Relé
    #define OPEN_PIN 12 //Switch fim de curso
    #define IN1 14 //Pino 1 - Motor de Passo
    #define IN2 27 //Pino 2 - Motor de Passo
    #define IN3 26 //Pino 3 - Motor de Passo
    #define IN4 25 //Pino 4 - Motor de Passo

  //Definição de variáveis
    note_t tone1 = NOTE_Fs; //Nota Fá Sustenido
    note_t tone2 = NOTE_E; //Nota Mi

    String tag1 = "00 00 00 00"; //TAG 1
    String tag2 = "00 00 00 00"; //TAG 2
    String tag3 = "00 00 00 00"; //TAG 3
    String tag4 = "00 00 00 00"; //TAG 4
    String valorRecebido; //Valor Recebido pelo Serial Blutetooth

    bool doorOpen = false; //Manipulador do Estado de Abertura Porta
    unsigned long doorOpenTime = 0; //Manipulador do Tempo de Abertura da Porta
    const unsigned long openDuration = 5000; //Tempo Máximo de Abertura da Porta

    bool doorReleased = false; //Manipulador do Estado de Liberação da Porta
    unsigned long doorReleasedTime = 0; //Manipulador do Tempo de Liberação da Porta
    const unsigned long releaseDuration = 5000; //Tempo Máximo de Liberação da Porta

    const int stepsPerRevolution = 200; //Passos por volta do Motor de Passo

    char st[20]; //Variáveis Globais - Leitura das TAGS

  //Inicialização de Objetos
    MFRC522 mfrc522(SS_PIN, RST_PIN); //RFID
    LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD
    Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4); //Motor de Passo
    BluetoothSerial SerialBT; //Blutetooth

//Setup
  void setup() {
    //Inicialização da Serial
      Serial.begin(115200);

    //Definição do Modo dos Pinos
      pinMode(LEDR_PIN, OUTPUT);
      pinMode(LEDG_PIN, OUTPUT);
      pinMode(BUZZ_PIN, OUTPUT);
      pinMode(OPEN_PIN, INPUT_PULLUP);
      pinMode(REL_PIN, OUTPUT);

    //Inicialização do Sensor RFID
      SPI.begin();          
      mfrc522.PCD_Init();

    //Inicialização do Módulo LCD
      lcd.init();
      lcd.backlight();

    //Inicialização do Bluetooth
      SerialBT.begin("GLG");
    
    //Definição do Estado de Alguns Pinos
      digitalWrite(LEDR_PIN, HIGH);
      digitalWrite(LEDG_PIN, LOW);
      digitalWrite(REL_PIN, HIGH);
  }

//Loop
  void loop() {
    //Inicialização do Texto no LCD
      lcd.setCursor(0,0);
      lcd.print("Insira a TAG");

    //Controle de Inclusão e Exclusão das TAGs
      if (SerialBT.available()) {
        valorRecebido = SerialBT.readString();
        valorRecebido.trim();
        if (valorRecebido.startsWith("Cadastrar")) {
          if (tag1 == "00 00 00 00") {
            tag1 = valorRecebido.substring(10);
            Serial.print("TAG cadastrada: "); Serial.println(tag1);
            SerialBT.print("A TAG de UID "); SerialBT.print(tag1); SerialBT.println(" foi cadastrada com sucesso!");
            SerialBT.println("TAGs Cadastradas:"); SerialBT.println(tag1); SerialBT.println(tag2); SerialBT.println(tag3); SerialBT.println(tag4);
          } else if (tag2 == "00 00 00 00") {
            tag2 = valorRecebido.substring(10);
            Serial.print("TAG cadastrada: "); Serial.println(tag2);
            SerialBT.print("A TAG de UID "); SerialBT.print(tag2); SerialBT.println(" foi cadastrada com sucesso!");
            SerialBT.println("TAGs Cadastradas:"); SerialBT.println(tag1); SerialBT.println(tag2); SerialBT.println(tag3); SerialBT.println(tag4);
          } else if (tag3 == "00 00 00 00") {
            tag3 = valorRecebido.substring(10);
            Serial.print("TAG cadastrada: "); Serial.println(tag3);
            SerialBT.print("A TAG de UID "); SerialBT.print(tag3); SerialBT.println(" foi cadastrada com sucesso!");
            SerialBT.println("TAGs Cadastradas:"); SerialBT.println(tag1); SerialBT.println(tag2); SerialBT.println(tag3); SerialBT.println(tag4);
          } else if (tag4 == "00 00 00 00") {
            tag4 = valorRecebido.substring(10);
            Serial.print("TAG cadastrada: "); Serial.println(tag4);
            SerialBT.print("A TAG de UID "); SerialBT.print(tag4); SerialBT.println(" foi cadastrada com sucesso!");
            SerialBT.println("TAGs Cadastradas:"); SerialBT.println(tag1); SerialBT.println(tag2); SerialBT.println(tag3); SerialBT.println(tag4);
          } else {
            SerialBT.print("A TAG de UID "); SerialBT.print(valorRecebido.substring(10)); SerialBT.println("  não pôde ser cadastrada! Memória cheia. Exclua alguma TAG.");
          }
        }

        if (valorRecebido.startsWith("Excluir")) {
          if (tag1 == valorRecebido.substring(8)) {
            tag1 = "00 00 00 00";
            Serial.print("TAG cadastrada: "); Serial.println(tag1);
            SerialBT.print("A TAG de UID "); SerialBT.print(tag1); SerialBT.println(" foi descadastrada com sucesso!");
            SerialBT.println("TAGs Cadastradas:"); SerialBT.println(tag1); SerialBT.println(tag2); SerialBT.println(tag3); SerialBT.println(tag4);
          } else if (tag2 == valorRecebido.substring(10)) {
            tag2 = "00 00 00 00";
            Serial.print("TAG cadastrada: "); Serial.println(tag2);
            SerialBT.print("A TAG de UID "); SerialBT.print(tag2); SerialBT.println(" foi descadastrada com sucesso!");
            SerialBT.println("TAGs Cadastradas:"); SerialBT.println(tag1); SerialBT.println(tag2); SerialBT.println(tag3); SerialBT.println(tag4);
          } else if (tag3 == valorRecebido.substring(10)) {
            tag3 = "00 00 00 00";
            Serial.print("TAG cadastrada: "); Serial.println(tag3);
            SerialBT.print("A TAG de UID "); SerialBT.print(tag3); SerialBT.println(" foi descadastrada com sucesso!");
            SerialBT.println("TAGs Cadastradas:"); SerialBT.println(tag1); SerialBT.println(tag2); SerialBT.println(tag3); SerialBT.println(tag4);
          } else if (tag4 == valorRecebido.substring(10)) {
            tag4 = "00 00 00 00";
            Serial.print("TAG cadastrada: "); Serial.println(tag4);
            SerialBT.print("A TAG de UID "); SerialBT.print(tag4); SerialBT.println(" foi descadastrada com sucesso!");
            SerialBT.println("TAGs Cadastradas:"); SerialBT.println(tag1); SerialBT.println(tag2); SerialBT.println(tag3); SerialBT.println(tag4);
          } else {
            SerialBT.print("A TAG de UID "); SerialBT.print(valorRecebido.substring(10)); SerialBT.println("  não está cadastrada da base de dados! Nada a ser feito.");
          }
        }
      }

    //Leitura das TAGS
      if ( ! mfrc522.PICC_IsNewCardPresent()) { return; } // Verifica novos cartões
      if ( ! mfrc522.PICC_ReadCardSerial()) { return; } // Seleciona um dos cartões
    
      // Mostra UID na serial
        Serial.print("UID da tag :");
        String conteudo= "";
        byte letra;
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(mfrc522.uid.uidByte[i], HEX);
          conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
          conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        Serial.println();
        Serial.print("Mensagem : ");
        conteudo.toUpperCase();

    //Verifica se a porta está liberada
      if (!doorReleased) {
        if (conteudo.substring(1) == tag1 || conteudo.substring(1) == tag2 || conteudo.substring(1) == tag3 || conteudo.substring(1) == tag4 || conteudo.substring(1) == "FB 4A C9 1D") {
          doorReleased = true;
          doorReleasedTime = millis();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Acesso permitido!");
          Serial.println("Acesso permitido!");
          digitalWrite(LEDR_PIN, LOW);
          digitalWrite(LEDG_PIN, HIGH);
          digitalWrite(REL_PIN, LOW);
          for (int i = 0; i < 5; i++) {
            myTone(BUZZ_PIN, tone2);
            delay(90);
            myTone(BUZZ_PIN, tone1);
            delay(90);
          }
          myTone(BUZZ_PIN, tone2);
          delay(120);
          myNoTone(BUZZ_PIN);
        } else {
          doorReleased = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Acesso negado!");
          Serial.println("Acesso negado!");
          delay(2000);
          lcd.clear();
        }
      }
    
    //Após liberada, começam as subrotinas de contagem
      if (doorReleased) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Porta liberada!");
        Serial.println("Porta liberada!");

        while (doorReleased) {
          //Verifica se a porta foi aberta
            if (digitalRead(OPEN_PIN) == LOW) {
              if (!doorOpen) {
                doorOpenTime = millis();
                doorOpen = true;
              }
            }

          //Após aberta, começam as subrotinas de contagem
            if (doorOpen) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Porta aberta!");
              Serial.println("Porta aberta!");

              while (doorOpen) {
                if (millis() - doorOpenTime >= openDuration) {
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Fechando a porta");
                  Serial.println("Fechando a porta");
                  while(digitalRead(OPEN_PIN) == LOW) {
                    myStepper.setSpeed(180);  // Defina a velocidade do motor
                    myStepper.step(stepsPerRevolution*4);
                  }
                  doorOpen = false;
                }
              }
            }

            if (millis() - doorReleasedTime >= releaseDuration && !doorOpen) {
              Serial.println("Tempo Esgotado! Porta trancada!");
              doorReleased = false;
              digitalWrite(LEDR_PIN, HIGH);
              digitalWrite(LEDG_PIN, LOW);
              digitalWrite(REL_PIN, HIGH);
              lcd.clear();
            }
          }
      }  
  }

//Funções Independentes
  //Tocar o Buzzer
    void myTone(int pin, note_t tone) {
      ledcSetup(0, tone, 8);
      ledcAttachPin(pin, 0);
      ledcWriteNote(0, tone, 5);
    }

  //Parar o Buzzer
    void myNoTone(int pin) {
      ledcDetachPin(pin);
    }
