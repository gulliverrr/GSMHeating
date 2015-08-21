#include "DHT.h"
#define DHTPIN 3

#define DHTTYPE DHT11   // DHT 11 

int SMS_location_number;
const unsigned int MAX_INPUT = 165; // 160 characters for SMS plus a few extra 
static unsigned int input_pos = 0;

char MyMaster[ ] = "+44778567834"; //MyMaster is not used anymore but functionality was left here for other people
const int NETWORK_LAG = 6000; // add this delay at the beginning of every outgoing call. Roughly after that much the other party will start ringing
const int MISSED_CALL_DELAY = 5000; // hang up after MISSED_CALL_DELAY seconds

const boolean RELAY_1_DEFAULT = HIGH; // set relay level to this state on startup or after the restart. LOW means relay contacts are closed
boolean RELAY_1_STATE = HIGH;

const unsigned int IGT = 2;
const unsigned int RELAY_1 = 4;

DHT dht(DHTPIN, DHTTYPE);
unsigned long time;
float h = 0; //Humidity
float t = 0; //Temperature
String sender;

void setup() 
{
  pinMode(IGT, OUTPUT);
  pinMode(RELAY_1, OUTPUT);

  delay(5000); // Wait to GSM board to start  
  Serial1.begin(9600); // Starts addtional serial port for GSM
  while (!Serial1);

  Serial.begin(9600);
  //while (!Serial);

  delay(1000);

  initialise();

  GSM_Startup();
  dht.begin();
  Serial.println("System ready");
  delay(1000);
}

void loop()   
{
  readTC35i();
  readDHT();
}

void GSM_Startup()
{
  Serial.println("GSM_Startup()");
  digitalWrite(IGT, LOW);
  delay(200);
  digitalWrite(IGT, HIGH);

  Serial1.println("AT+CMGF=1\r");
  delay(200);
  Serial1.println("AT+CNMI=2,1,0,0,1\r");
  delay(200);  
  delete_All_SMS();
}

void initialise()
{
  Serial.println("initialise()");
  RELAY_1_STATE = RELAY_1_DEFAULT;
  digitalWrite(RELAY_1, RELAY_1_STATE);
  time = millis();
}

void readTC35i()
{
  static char input_line [MAX_INPUT];
  if (Serial1.available () > 0)
  {
    while (Serial1.available () > 0)
    {
      char inByte = Serial1.read ();
      switch (inByte)
      {
      case '\n': // end of text
        input_line [input_pos] = 0; // terminating null byte

        // terminator reached! process input_line now
        process_data (input_line);

        // reset buffer for next time
        input_pos = 0;  
        break;

      case '\r': // discard carriage return
        break;

      default:
        // keep adding if not full... allow for terminating null byte
        if (input_pos < (MAX_INPUT - 1))
          input_line [input_pos++] = inByte;
        break;
      }
    }
  }
}

void readDHT()
{
  if ((time + 5000) < millis() )
  {
    time = millis();
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius
    t = dht.readTemperature();
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" *C");
    Serial.print(", Humidity: ");
    Serial.print(h);
    Serial.println("%");
  }
}

void process_data (char * data)
{
  Serial.print("Received GSM:");
  Serial.println(data); // display the data
  
  //Allow incoming SMSs only from approved callers. I have 2 examples here but you can add your own
  if(strstr(data, "+CMGR:") && strstr(data, "7795447123"))
  {
    Serial.println("Alice");
    //George
    sender = "+447795447123"; // call using international format
  } 
  else if(strstr(data, "+CMGR:") && strstr(data, "7795447122"))
  {
    Serial.println("Bob");
    //Mama
    sender = "+447795447122";
  }
 
  if (sender.length() > 0)
  {
    Serial.print("Sender:");
    Serial.println(sender);
  }
  
  if(strstr(data, "smsdelete"))
  {
    delete_All_SMS();
  }

  if(strstr(data, "^SMGO: 2"))
  { // SIM card FULL
    delete_All_SMS(); // delete all SMS
  }

  if(strstr(data, "+CMTI:"))
  { // An SMS has arrived
    char* copy = data + 12; // Read from position 12 until a non ASCII number to get the SMS location
    SMS_location_number = (byte) atoi(copy); // Convert the ASCII number to an int
    Serial1.print("AT+CMGR=");
    Serial1.println(SMS_location_number);
  }

  if(strstr(strlwr(data), "ring"))
  {
    // Incoming call, answer it
    Serial.println("Incoming call. Answering it in 2 sec...");
    delay(2000);
    Serial1.println("ATA");
    Serial.println("Incoming call, answered. Hanging up in 5 sec...");
    delay(5000);
    if(!RELAY_1_STATE) //if RELAY_1 is ON wait twice the time
    {
      delay(5000);
    }
    Serial1.println("ATH");
    Serial.println("Incoming call, answered and hung up.");
  }

  if(strstr(strlwr(data), "2341 on"))
  {
    RELAY_1_ON();
    delete_one_SMS();
    //CallMyMaster();
    CallSender();
  }

  if(strstr(strlwr(data), "2341 off"))
  {
    RELAY_1_OFF();
    delete_one_SMS();
    //CallMyMaster();
    CallSender();
  }

  if(strstr(strlwr(data), "2341 ask"))
  {
    sendSMS();
    delete_one_SMS();
    //Serial.println("Deleted SMS");
  }
}

void delete_one_SMS()
{
  Serial.println("Deleting SMS 1");
  Serial1.print("AT+CMGD=");
  Serial1.println("1");
  delay(1000);
}

void delete_All_SMS()
{
  Serial.println("delete_All_SMS()");
  for(int i = 1; i <= 10; i++) {
    Serial1.print("AT+CMGD=");
    Serial1.println(i);
    Serial.print("Deleting SMS ");
    Serial.println(i);
    delay(500);
  }
}

void sendSMS()
{
  Serial.println("sendSMS()");
  Serial1.print("AT+CMGF=1\r");
  delay(400);
  Serial1.print("AT+CMGS=\"");
  Serial1.print(sender);
  Serial1.println("\"");
  delay(100);
  Serial1.print("Heating is ");
  if(!RELAY_1_STATE) //RELAY_1 is ON
  {
    Serial1.println("ON");
  } else {
    Serial1.println("OFF");
  }
  Serial1.print("Temperature: ");
  Serial1.print(t, 1);
  Serial1.println(" 'C");
  Serial1.print("Humidity: ");
  Serial1.print(h, 0);
  Serial1.print("%");
  delay(100);
  Serial1.println((char)26);
}

void RELAY_1_ON()
{
  RELAY_1_STATE = LOW;
  digitalWrite(RELAY_1, RELAY_1_STATE);
  Serial.println("RELAY_1 is on");
  delay(100); 
}

void RELAY_1_OFF()
{
  RELAY_1_STATE = HIGH;
  digitalWrite(RELAY_1, RELAY_1_STATE);
  Serial.println("RELAY_1 is off");
  delay(100);
}

void CallMyMaster()
{
  Serial.println("Calling My Master.");
  Serial1.print("ATDT"); // Call
  Serial1.print(MyMaster); // This number
  Serial1.println(";"); // Do it!
  Serial.println("Dialling...");
  delay(NETWORK_LAG);
  delay(MISSED_CALL_DELAY);
  if(!RELAY_1_STATE) //if RELAY_1 is ON wait twice the time
  {
    delay(MISSED_CALL_DELAY);
  }
  Serial.println("Hunging up...");
  Serial1.println("ATH"); // end call
  Serial.println("Hung up!");
}

void CallSender()
{
  Serial.print("Calling Sender:");
  Serial.println(sender);
  Serial1.print("ATDT"); // Call
  Serial1.print(sender); // This number
  Serial1.println(";"); // Do it!
  Serial.println("Dialling...");
  delay(NETWORK_LAG);
  delay(MISSED_CALL_DELAY);
  if(!RELAY_1_STATE) //if RELAY_1 is ON wait twice the time
  {
    delay(MISSED_CALL_DELAY);
  }
  Serial.println("Hunging up...");
  Serial1.println("ATH"); // end call
  Serial.println("Hung up!");
}
