
#include "NewGSM.h"

//If you want to use the Arduino functions to manage SMS, uncomment the lines below.
#include "sms.h"
SMSGSM sms;

//Simple sketch to send and receive SMS.

int numdata;
boolean started=false;
char smsbuffer[160] = "Arduino SMS";
char n[20] = "09291234567";	//Replace with your cell number.

const int RX_pin = 10;
const int TX_pin = 3;
const int GSM_ON_pin = 7;

void setup()
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println(F("GSM Shield testing."));

  //Configure Comm Port to select Hardware or Software serial
  //gsm.SelectHardwareSerial(&Serial1, GSM_ON_pin);
  gsm.SelectSoftwareSerial(RX_pin, TX_pin, GSM_ON_pin);

  //Configure baudrate.
  if (gsm.begin(9600)) 
  {
    Serial.println(F("\nstatus=READY"));
    started=true;
  } else Serial.println(F("\nstatus=IDLE"));

  if(started) 
  {
    //Enable this two lines if you want to send an SMS.
    if (sms.SendSMS(n,smsbuffer))
      Serial.println(F("\nSMS sent OK"));
  }
};

void loop()
{
  if(started) 
  {
    //Read if there are messages on SIM card and print them.
    /**  deprecated method
    if(gsm.readSMS(smsbuffer, 160, n, 20)) {
       Serial.println(n);
       Serial.println(smsbuffer);
    }
    **/
    //get 1st sms
    int pos=sms.IsSMSPresent(SMS_ALL);
    if(pos>0&&pos<=20)
    {
      Serial.print(pos);
      Serial.print(". ");
      int ret_val = sms.GetSMS(pos, n, 20, smsbuffer, 160);
      if (ret_val>0)
      {
        switch (ret_val) {
        case GETSMS_UNREAD_SMS:
          Serial.print(F("UNREAD SMS from "));
          break;
        case GETSMS_READ_SMS:
          Serial.print(F("READ SMS from "));
          break;
        default:  
          Serial.print(F("OTHER SMS from "));
          break;
        }
        Serial.println(n);
        Serial.println(smsbuffer);
        if (ret_val==GETSMS_UNREAD_SMS)
        {
          if (sms.SendSMS(n,smsbuffer))
          {
            Serial.println(F("SMS resent OK"));
          }  
        }
        sms.DeleteSMS(pos);
      }
    }
  }
  delay(1000);
};
