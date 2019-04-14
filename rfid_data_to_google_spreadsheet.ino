
/* Arduino Code which sends data to google spreadsheet */

#include<SPI.h>
#include<MFRC522.h>
#include <Ethernet.h>
#define SS_PIN 4 //FOR RFID SS PIN BECASUSE WE ARE USING BOTH ETHERNET SHIELD AND RS-522
#define RST_PIN 9
#define No_Of_Card 3
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "api.pushingbox.com";   //YOUR SERVER
IPAddress ip(192, 168, 137, 177);
EthernetClient client;     
MFRC522 rfid(SS_PIN,RST_PIN);
MFRC522::MIFARE_Key key; 
byte id[No_Of_Card][4]={
  {142,76,58,42},             //RFID NO-1
  {153,178,60,64},             //RFID NO-2
  {141,764,60,94}              //RFID NO-3
};
byte id_temp[3][3];
byte i;
int j=0;


// the setup function runs once when you press reset or power the board
void setup(){
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    for(byte i=0;i<6;i++)
    {
      key.keyByte[i]=0xFF;
    }
    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      Ethernet.begin(mac, ip);
    }
    delay(1000);
    Serial.println("connecting...");
 }

// the loop function runs over and over again forever
void loop(){
    int m=0;
    if(!rfid.PICC_IsNewCardPresent())
    return;
    if(!rfid.PICC_ReadCardSerial())
    return;
    for(i=0;i<4;i++)
    {
     id_temp[0][i]=rfid.uid.uidByte[i]; 
               delay(50);
    }
    
     for(i=0;i<No_Of_Card;i++)
    {
            if(id[i][0]==id_temp[0][0])
            {
              if(id[i][1]==id_temp[0][1])
              {
                if(id[i][2]==id_temp[0][2])
                {
                  if(id[i][3]==id_temp[0][3])
                  {
                    Serial.print("your card no :");
                    for(int s=0;s<4;s++)
                    {
                      Serial.print(rfid.uid.uidByte[s]);
                      Serial.print(" ");
                     
                    }
                    Serial.println("\nValid Person");
                    Sending_To_spreadsheet();
                    j=0;
                              
                              rfid.PICC_HaltA(); rfid.PCD_StopCrypto1();   return; 
                  }
                }
              }
            }
     else
     {j++;
      if(j==No_Of_Card)
      {
        Serial.println("Not a valid Person");
        Sending_To_spreadsheet();
        j=0;
      }
     }
    }
    
       // Halt PICC
    rfid.PICC_HaltA();
  
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
 }

 void Sending_To_spreadsheet()   //CONNECTING WITH MYSQL
 {
   if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET /pushingbox?devid=<Your Device ID>&allowed_members=");     //YOUR URL
    if(j!=No_Of_Card)
    {
      client.print('1');
//      Serial.print('1');
    }
    else
    {
      client.print('0');
    }
    
    client.print("&Member_ID=");
    for(int s=0;s<4;s++)
                  {
                    client.print(rfid.uid.uidByte[s]);
                                  
                  }
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: api.pushingbox.com");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
 }

 
