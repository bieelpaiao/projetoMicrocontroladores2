//Configurações Iniciais
  //Definição de Bibliotecas
    #include <Arduino.h> //Arduino
    #include <WiFi.h> //WiFi
    #include <AsyncTCP.h> //Comunicação TCP Assíncrona
    #include <ESPAsyncWebServer.h> //Servidor Web Assíncrono
    #include <SPI.h> //Sensor RFID
    #include <MFRC522.h> //Sensor RFID
    #include <Wire.h> //Módulo LCD
    #include <LiquidCrystal_I2C.h> //Módulo LCD
    #include <Stepper.h> //Motor de Passo

  //Mapeamento de Harware
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
    AsyncWebServer server(80); //WebServer Wi-fi
    Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4); //Motor de Passo

  //Configurações do Wi-fi
  const char* ssid = "TESTE_WIFI"; //Nome
  const char* pass = "12345678"; //Senha

  const char* PARAM_INPUT_1 = "input1"; //Manipulador do Formulário de Cadastro
  const char* PARAM_INPUT_2 = "input2"; //Manipulador do Formulário de Exclusão


  //Páginas HTML
    String index_html_1 = "<!DOCTYPE html><html lang=\"pt-br\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>Controle de usuários</title><style>body{display:flex;justify-content:center;align-items:center;height:100vh;margin:0;background-color:#f2f2f2;font-family:Arial,sans-serif}.background{background-color:#fff;border-radius:10px;padding:20px;box-shadow:0 0 10px rgba(0,0,0,.1);text-align:center}h1{font-family:Arial,sans-serif;color:#000;font-weight:400}form{margin-bottom:20px}input[type=text]{padding:10px;margin-right:10px;border:1px solid #ccc;border-radius:5px;font-family:Arial,sans-serif;font-size:14px}input[type=submit]{padding:10px 20px;border:none;border-radius:5px;background-color:#0074d9;color:#fff;cursor:pointer;font-family:Arial,sans-serif;font-size:14px}input[type=submit]:hover{background-color:#0056b3}</style></head><body><div class=\"background\"><h2>Controle de TAGs - SmartLock</h2><h4>TAGS cadastradas:</h4>";
    String index_html_2 = "<br><form action=\"/get\">Cadastrar TAG: <input type=\"text\" name=\"input1\"> <input type=\"submit\" value=\"Enviar\"></form><form action=\"/get\">Excluir TAG: <input type=\"text\" name=\"input2\"> <input type=\"submit\" value=\"Enviar\"></form>";

    String create_html_1 = "<!DOCTYPE html><html lang=\"pt-br\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>Controle de usuários</title><style>body{display:flex;justify-content:center;align-items:center;height:100vh;margin:0;background-color:#f2f2f2;font-family:Arial,sans-serif}.background{background-color:green;border-radius:10px;padding:20px;box-shadow:0 0 10px rgba(0,0,0,.1);text-align:center}h3{font-family:Arial,sans-serif;color:#fff;font-weight:700}span{font-family:Arial,sans-serif;color:#fff;font-weight:bolder}a{color:#0ff}form{font-family:Arial,sans-serif;margin-bottom:20px}input[type=text]{padding:10px;margin-right:10px;border:1px solid #ccc;border-radius:5px;font-family:Arial,sans-serif;font-size:14px}input[type=submit]{padding:10px 20px;border:none;border-radius:5px;background-color:#0074d9;color:#fff;cursor:pointer;font-family:Arial,sans-serif;font-size:14px}input[type=submit]:hover{background-color:#0056b3}</style></head><body><div class=\"background\"><h3>A TAG de UID <span>";
    String create_html_2 = "</span> foi cadastrada com sucesso!<br><a href=\"/\">Retornar à página inicial</a>";

    String delete_html_1 = "<!DOCTYPE html><html lang=\"pt-br\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>Controle de usuários</title><style>body{display:flex;justify-content:center;align-items:center;height:100vh;margin:0;background-color:#f2f2f2;font-family:Arial,sans-serif}.background{background-color:red;border-radius:10px;padding:20px;box-shadow:0 0 10px rgba(0,0,0,.1);text-align:center}h3{font-family:Arial,sans-serif;color:#fff;font-weight:700}span{font-family:Arial,sans-serif;color:#fff;font-weight:bolder}a{color:#0ff}form{font-family:Arial,sans-serif;margin-bottom:20px}input[type=text]{padding:10px;margin-right:10px;border:1px solid #ccc;border-radius:5px;font-family:Arial,sans-serif;font-size:14px}input[type=submit]{padding:10px 20px;border:none;border-radius:5px;background-color:#0074d9;color:#fff;cursor:pointer;font-family:Arial,sans-serif;font-size:14px}input[type=submit]:hover{background-color:#0056b3}</style></head><body><div class=\"background\"><h3>A TAG de UID <span>";
    String delete_html_2 = "</span> foi excluída com sucesso!<br><a href=\"/\">Retornar à página inicial</a>";

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

    //Inicialização do Módulo Wi-Fi
      WiFi.softAP(ssid, pass); //Inicia o ponto de acesso
      Serial.print("Se conectando a: "); //Imprime mensagem sobre o nome do ponto de acesso
      Serial.println(ssid);

      IPAddress ip = WiFi.softAPIP(); //Endereço de IP
      
      Serial.print("Endereço de IP: "); //Imprime o endereço de IP
      Serial.println(ip);

      server.begin();
      Serial.println("Servidor online"); //Imprime a mensagem de início

    
    // Criação da página no servidor web
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html_1 + "<li>" + tag1 + "</li>" + "<li>" + tag2 + "</li>" + "<li>" + tag3 + "</li>" + "<li>" + tag4 + "</li>" + index_html_2);
      });

    // Manipuação das requisições GET
      server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String inputMessage;
        String inputParam;
        
        //Cadastro de TAG
          if (request->hasParam(PARAM_INPUT_1)) {
            inputMessage = request->getParam(PARAM_INPUT_1)->value();
            inputParam = PARAM_INPUT_1;
            if (tag1 == "00 00 00 00") {
              tag1 = inputMessage;
              request->send(200, "text/html", create_html_1 + inputMessage + create_html_2);
            } else if (tag2 == "00 00 00 00") {
              tag2 = inputMessage;
              request->send(200, "text/html", create_html_1 + inputMessage + create_html_2);
            } else if (tag3 == "00 00 00 00") {
              tag3 = inputMessage;
              request->send(200, "text/html", create_html_1 + inputMessage + create_html_2);
            } else if (tag4 == "00 00 00 00") {
              tag4 = inputMessage;
              request->send(200, "text/html", create_html_1 + inputMessage + create_html_2);
            } else {
              request->send(200, "text/html", delete_html_1 + inputMessage + "</span> não pôde ser cadastrada! Memória cheia. Exclua alguma TAG.<br><a href=\"/\">Retornar à página inicial</a>");
            }
          }
        
        // Exclusão de TAG
          else if (request->hasParam(PARAM_INPUT_2)) {
            inputMessage = request->getParam(PARAM_INPUT_2)->value();
            inputParam = PARAM_INPUT_2;
            if (tag1 == inputMessage) {
              tag1 = "00 00 00 00";
              request->send(200, "text/html", delete_html_1 + inputMessage + delete_html_2);
            } else if (tag2 == inputMessage) {
              tag2 = "00 00 00 00";
              request->send(200, "text/html", delete_html_1 + inputMessage + delete_html_2);
            } else if (tag3 == inputMessage) {
              tag3 = "00 00 00 00";
              request->send(200, "text/html", delete_html_1 + inputMessage + delete_html_2);
            } else if (tag4 == inputMessage) {
              tag4 = "00 00 00 00";
              request->send(200, "text/html", delete_html_1 + inputMessage + delete_html_2);
            } else {
              request->send(200, "text/html", create_html_1 + inputMessage + "</span> não está cadastrada da base de dados! Nada a ser feito.<br><a href=\"/\">Retornar à página inicial</a>");
            }
          } 
          
        //Manipulação de Erros - Caso não tenha parâmetros
          else {
            inputMessage = "No message sent";
            inputParam = "none";
          }
      });
    
    //Manipulação de Erros - Servidor não encontrado
      server.onNotFound(notFound);
    
    //Inicialização do Servidor WEB
      server.begin();
    
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

    //Leitura das TAGS
      if ( ! mfrc522.PICC_IsNewCardPresent()) { return; } // Verifica novos cartões
      if ( ! mfrc522.PICC_ReadCardSerial()) { return; } // Seleciona um dos cartões
      
      // Mostra UID da TAG na Serial
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

  //Manipulação de Erros - Servidor não encontrado
  void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  }