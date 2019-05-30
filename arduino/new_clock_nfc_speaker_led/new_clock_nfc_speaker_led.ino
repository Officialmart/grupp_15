/**************************************************************************/
/*!
    @file     mifareclassic_memdump.pde
    @author   Adafruit Industries
  @license  BSD (see license.txt)
    This example attempts to dump the contents of a Mifare Classic 1K card
    Note that you need the baud rate to be 115200 because we need to print
  out the data and read from the card at the same time!
    This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
    This library works with the Adafruit NFC breakout
      ----> https://www.adafruit.com/products/364
    Check out the links above for our tutorials and wiring diagrams
    These chips use SPI or I2C to communicate
    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!
*/
/**************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <DS3231.h> //Klocka
#include <Adafruit_PN532.h> //NFC

//För SPI kommunikation definerades dessa pins för NFC:n. För denna produkt användes SPI.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// För I2C kommunikation defineras dessa pins för NFC:n
//#define PN532_IRQ   (2)
//#define PN532_RESET (3)  

//För högtalaren användes pin 7
#define SPEAKER 7

// Dessa pins kommer att användas och skickas till metoden nfc
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);


//Högtalare
//De olika tonerna för högtalaren
int BassTab[] = {1911, 1702, 1516, 1431, 1275, 1136, 1200}; 

//Clock
DS3231  rtc(SDA, SCL); //Objekt av DS231

#if defined(ARDUINO_ARCH_SAMD)

   #define Serial SerialUSB
#endif



//EGET ------------------------------------------------
String s; //Global string variabel som innehåller data som kommer ifrån NFC:n läsaren
String arr[10]; //Global array som innehåller stop kommandot för alarmet
String stringone[10]; //Global array som innehåller alarmtider
int a = 0; // Globar variabel som används för att printa ut all data som finns i arrayerna arr och stringone
int alarm_counter = 0; //Räknarare som räknar igenom arrayen stringone för att byta till nästa tid i arrayen när larmet stängts av från tidigare alarmet
int stopme = 1; //Global variabel för att stoppa alarmet
//EGET ------------------------------------------------

void setup(void) {
  //NFC
  #ifndef ESP8266
    while (!Serial); // för Leonardo/Micro/Zero
  #endif
  
  Serial.begin(115200); // NFC:n printar ut data i respektive baud, bits per sekund. Kommunikationen sker alltså i denna frekvens
  Serial.println("Looking for PN532..."); //Printar ut text

  nfc.begin(); //Initierar nfc objekt

  uint32_t versiondata = nfc.getFirmwareVersion(); //Variabel som lagrar versionen för NFC:n
  if (! versiondata) {
    Serial.print("Didn't find PN53x board"); // Om ingen version hittas, alltså NFC:n inte hittas, skriv ut text.
    while (1); // halt
  }
  // NFC:n har hittas och all data har hittats om NFC:n
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); //Skriver informationen om chippet och omvandlar om datan till hexadecimalt
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);  //Skriver informationen om chippet och omvandlar om datan till decimalt
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC); //-||-

  //Konfigurerar NFC:n till att läsa RFID taggar (nfc brickor)
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ..."); //Skriver ut att NFC:n är redo och väntar på att läsa av data från en NFC bricka.


  //CLOCK
// Setup Serial connection
  Serial.begin(115200); // Klockan printar ut data i respektive baud, bits per sekund. Kommunikationen sker alltså i denna frekvens
  
  // Initierar rtc objekt
  rtc.begin();
  
  // Här sätta dag, tid och datum i klockan
  //rtc.setDOW(WEDNESDAY);     // Sätter vilken dag det är i veckan
  rtc.setTime(03, 46, 00);     // Sätter tiden i formatet HH:MM:SS (24hr format)
  //rtc.setDate(1, 5, 2019);   // Set the date to January 1st, 2014 Sätter datum, t.ex 1, 5, 2019 (första maj 2019)


  //SPEAKER
  pinInit(); //Initierar högtalaren

  //LED
  pinMode(LED_BUILTIN, OUTPUT);
}


void loop(void) { //All kod i loop kommer att upprepas hela tiden
  //CLOCK och LED
 
  Serial.println(rtc.getTimeStr()); //Printar klockslagen från klockan till seriell monitor
  
  delay (1000); // Vänta en sekund mellan klockslagen

  
  if(stringone[alarm_counter]+":00" == rtc.getTimeStr() || stopme == 0) //Om tiden i fältet stringone och dess index alarm_clounter är samma som tiden från klockan, så körs koderna i if-satsen. Eller om variabeln stopme är lika med 0.
  {
    stopme = 0; //Variablen stopsme tilldelas värdet 0, så raderna nedan, som aktiverar alarm och LED-lampan, körs tills alarmet stängs av
    for (int note_index = 0; note_index < 1; note_index++)
    {
      //SPEAKER
      sound(1); //Ljud spelas upp från högtalaren
      digitalWrite(LED_BUILTIN, HIGH); //Led-lampa tänds
      delay(25); //Vänta 20ms
      digitalWrite(LED_BUILTIN, LOW);//Led-lampa släcks
      delay(25); //Vänta 25ms
      sound(1);//Ljud spelas upp från högtalaren
      digitalWrite(LED_BUILTIN, HIGH); //Led-lampa tänds
      delay(25);//Vänta 25ms
      digitalWrite(LED_BUILTIN, LOW);//Led-lampa släcks
    }
  }

  //NFC
  uint8_t success;                          // Flagga som kontrollerar ifall något fel skett med läsningen från NFC:n
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Fält som lagrar returnerande UID
  uint8_t uidLength;                        // UID:s längd, på detta sätt avgörs om UID består av 4 bytes (Mifare Classic bricka) eller 6 bytes (Mifare Ultralight brick)
  uint8_t currentblock;                     // Räknare för att hålla reda på vilket block vi befinner oss i när NFC:n läser av från en NFC:bricka
  bool authenticated = false;               // Flagga som indikerar om någon sektorn är autentisead
  uint8_t data[16];                         // Array som lagrar alla data från NFC under avläsningen
  
  uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) { //Om NFC:n kommer i kontakt med en NFC bricka
    digitalWrite(LED_BUILTIN, HIGH);   // LED-lampa lyser under tiden NFC:n läser av data från en bricka, så användaren inte tar bort brickan för tidigt under avläsningen
    int counter = 0; //COUNTER nollställs varje gång ett NFC-kort läses
    clear_array(arr); //Tömmer arrayen arr som innehåller kommandot för att stoppa alarmet
    int increment_alarm_counter = 0;
    
    // Skriv ut information om NFC brickan/kortet
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength); //Printar ut kord uid och antal bytes
    Serial.println("");

    if (uidLength == 4) //Om vi har en Mifare Classic kort (4 byte)
    {
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      //For-loopen försöker gå igenom alla sektorer, det bör finnas 16 sektorer och varje sektor består av 4 block. Så 64 block sammanlagt. 
      for (currentblock = 0; currentblock < 64; currentblock++)
      {
        // Kontrollerar ifall detta nuvarande blocket är ett nytt block, om det är nytt block så måste den autentiseras, så vi kan få tillgång till den data som blocket innehåller
        if (nfc.mifareclassic_IsFirstBlock(currentblock))
          authenticated = false;

        // Om sektorn inte är autentiserad
        if (!authenticated)
        {
          // Starta en ny sektor och försök autentisera sektorn
          Serial.print("------------------------Sector ");Serial.print(currentblock/4, DEC);Serial.println("-------------------------");
          if (currentblock == 0)//Om vi befinner oss i första blocket
          {
              
              success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
          }
          else
          {
              
              success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
          }
          if (success) //Om blocket är autentiserad, tilldelar vi boolean variabeln värdet true
          {
            authenticated = true;
          }
          else //Om det inte går att autentisera blocket, så skrivs en error ut för respektive block
          {
            Serial.println("Authentication error");
          }
        }
        // Om blocket fortfarande inte lyckats autentiseras, så skippar programmet blocket som inte går att autentisera
        if (!authenticated)
        {
          Serial.print("Block ");Serial.print(currentblock, DEC);Serial.println(" unable to authenticate");
        }
        else
        {
          // Nu är blocket autentiserad, data som finns i NFC brickan/kortet kan läsas av nu
          success = nfc.mifareclassic_ReadDataBlock(currentblock, data);
          if (success && currentblock)
          {
            //---------------------------------------------------------Här till...
            // All data som lästes från det autentiserade blocket lagras i variabeln s
            s = nfc.PrintHexChar(data, 16); //I s lagras alla tider
             int i = 0; //Räknare som stegar igenom antal tecken som finns i varaibeln s.
             if(s.length() > 3) // Om den data som lagrats i variabeln s är består av flera tecken än tre, då ska koderna i if-satsen köras      autentisera
             {
               while(i<s.length())
               {
                      arr[0] = s.substring(0,s.length()); //Hela strängen som finns i variabeln s lagras i första platsen (index 0) i fältet arr
                      if(arr[0] == "SSSS") //Kontrollerar ifall det är brickan i basstationen som blir avläst av NFC läsaren, som innehåller strängen "SSSS", detta används för att stänga av alarmet när nfc läsaren kommer i kontakt med brickan som stoppar larmet
                      {
                        if(increment_alarm_counter == 0) //Om variabeln är lika med noll körs koderna i if satsen
                        {
                          alarm_counter++; //Byt index i fältet till nästa index som innehåller nästa alarmtid i fältet stringone
                          increment_alarm_counter++; // Ökar variabelns innehåll som var 0 till 1, så denna if sats inte körs igen
                        }
                        stopme = 1; //stopme tilldelas värdet 1, detta stoppar alarmet när kommandot SSSS har lästs av från NFC brickan som finns i basstationen, då kommer alarmet, som är aktiverat, att stängas av.
                        Serial.println(arr[0]); //Skriver ut data i fältet arr.
                        i++; //Ökar i med 1 så while satsen kan gå vidare
                      }
                      else if(arr[0] != "SSSS"){ //Om första avlästa strängen inte innehåller "SSSS" så läser vi inte av nfc brickan som finns i basstationen
                        stringone[counter] = s.substring(0,s.length()); //Lagrar data som finns i varaibeln s, som innehåller data från avläsningen av en bricka.
                        i++; //Ökar i med 1 så while satsen kan gå vidare
                      }
               }
               counter++; //Ökar counter med ett, så nästa tid som ska lagras i fältet stringone lagrar den tiden i nästa index av fältet
               Serial.println(stringone[counter] + " has been stored"); //Skriver ut tiden som lagrats i fältet stringone
             }
             else
              Serial.println("No time in string to store"); //Om blocken inte innehåller någon data som efterfrågas så skrivs denna rad ut
            
             //--------------------------------------------------------- ...Hit
          }
          else
          {
            // Om något går fel och det inte går läsa av den data som finns i blocket, så skrivs text ut som talar om vilket block inte kunde läsa av.
            Serial.print("Block ");Serial.print(currentblock, DEC); //Omvandlar data till decimalt
            Serial.println(" unable to read this block");
          }
        }
      }
    }
    else
    {
      Serial.println("Ooops ... this doesn't seem to be a Mifare Classic card!"); //Om vi kortet/brickan som nfc:n försöker läsa av inte är ett Mifare Classic kort, så skrivs detta meddelande ut
    }
    digitalWrite(LED_BUILTIN, LOW);    // Stäng av lampan när NFC är klar med avläsandet
    if(a==0){ //Så detta inte skrivs ut under varje loop, så används en if-sats som är true om a är lika med noll.
      Serial.println("Stop Array: "); //Skriver ut all data som lagrats i fältet arr
      print_alarm_time(arr); //Kallar på metoden och skickar fältet arr som parameter. Metoden skriver ut all data i fältet
      Serial.println("ALARM TIMES: "); // -||-
      print_alarm_time(stringone); // -||-
      Serial.println("CURRENT INDEX IN ALARMTIME ARRAY: ");
      Serial.println(alarm_counter); //Skriver ut vart programmet befinner sig i fältet stringone, som innehåller alla alarmtider. alarm_counter är en räknare som stegar igenom stringone fältet efter att alarmet från tidigare tiden stängts av
    }
 // Wait a bit before trying again
 arr[0] = ""; //Sätter första indexet i fältet arr till "" (blank) för att tömma den, så alarmet inte stängs av när det är dags för nästa alarm tid.
  Serial.println("You need to wait for 5 seconds before using the NFC again");
  delay(5000); //En delay som används för att nfc läsaren inte ska läsa av samma bricka så fort den är klar med första avläsningen av samma bricka.
  }
}


//Metoder ------------------------------------------------
//Skriver ut innehållet av en string fält
void print_alarm_time(String t_array[]) //Egen
{
  for(int i=0; i<10; i++)
  {
    if(t_array[i].length()>0)
      Serial.println(t_array[i]);
  }
}

//Sätter alla data i en string fält till "" (blank), alltså tömmer fältet.
void clear_array(String c_array[])
{
  for(int i=0; i<10; i++)
  {
      c_array[i] = "";
  }
}

//SPEAKER
void pinInit()
{
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
}

//SPEAKER testar högtalarna
void sound(uint8_t note_index)
{
  for (int i = 0; i < 100; i++)
  {
    digitalWrite(SPEAKER, HIGH);
    delayMicroseconds(BassTab[note_index]);
    digitalWrite(SPEAKER, LOW);
    delayMicroseconds(BassTab[note_index]);
  }
}
//------------------------------------------------
