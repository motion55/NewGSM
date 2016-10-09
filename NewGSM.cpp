
#include "NewGSM.h"

#define _GSM_CONNECTION_TOUT_ 5
#define _TCP_CONNECTION_TOUT_ 20
#define _GSM_DATA_TOUT_ 10

//#define RESETPIN 7

//static NEWGSM::NEWGSM* _instance = NULL;
NEWGSM::NEWGSM() {};
NEWGSM::~NEWGSM() {};

/**********************************************************
Function: 	This function permits to wake up the module
			(only for SIM908) when it goes in energy saving
			mode.

Author:		Marco Martines
Created:	unknown
Modified:	18/02/2014

Output:		none

Comments:	It would be nice to call this function
 			automatically when begin() is called (of course
 			only if a SIM908 is used). 
**********************************************************/

char NEWGSM::forceON()
{
	 char ret_val=0;
     char *p_char;
     char *p_char1;

     SimpleWriteln(F("AT+CREG?"));
     WaitResp(5000, 100, str_ok);
	 String str_ok_string(str_ok);
     if(IsStringReceived(str_ok_string.c_str())) 
	 {
          ret_val=1;
     }
     
     p_char = strchr((char *)(comm_buf),',');
     p_char1 = p_char+1;
     *(p_char1+2)=0;
     p_char = strchr((char *)(p_char1), ',');
     if (p_char != NULL) {
          *p_char = 0;
     }

     if((*p_char1)=='4') {
          digitalWrite(_GSM_ON, HIGH);
          delay(1200);
          digitalWrite(_GSM_ON, LOW);
          delay(10000);
          ret_val=2;
     }

     return ret_val;
}

int NEWGSM::configandwait(char* pin)
{
     int connCode;
     //_tf.setTimeout(_GSM_CONNECTION_TOUT_);

     if(pin) setPIN(pin); //syv

     // Try 10 times to register in the network. Note this can take some time!
     for(int i=0; i<10; i++) {
          //Ask for register status to GPRS network.
          SimpleWriteln(F("AT+CGREG?"));

          //Se espera la unsolicited response de registered to network.
          while(WaitResp(5000, 50, F("+CGREG: 0,"))!=RX_FINISHED_STR_RECV)
               //while (_tf.find("+CGREG: 0,"))  // CHANGE!!!!
          {
               //connCode=_tf.getValue();
               connCode=_cell.read();
               if((connCode==1)||(connCode==5)) {
                    setStatus(READY);

                    SimpleWriteln(F("AT+CMGF=1")); //SMS text mode.
                    delay(200);
                    // Buah, we should take this to readCall()
                    SimpleWriteln(F("AT+CLIP=1")); //SMS text mode.
                    delay(200);
                    //_cell << "AT+QIDEACT" <<  _DEC(cr) << endl; //To make sure not pending connection.
                    //delay(1000);

                    return 1;
               }
          }
     }
     return 0;
}

/**
 * read(char* buffer, int buffersize)
 *
 * Waits for data to be readable from the gsm module, reads data until
 * no more is available or the buffer has been filled
 *
 * returns number of bytes read
 *
 */
int NEWGSM::read(char* result, int resultlength)
{
     char temp;
     int i=0;

#ifdef DEBUG_SERIAL
	 DEBUG_SERIAL.print(F("Starting read..\nWaiting for Data.."));
#endif
     // Wait until we start receiving data
     while(available()<1) {
          delay(100);
#ifdef DEBUG_SERIAL
		  DEBUG_SERIAL.print(F("."));
#endif
     }

     while(available()>0 && i<(resultlength-1)) {
          temp=_cell.read();
          if(temp>0) {
#ifdef DEBUG_SERIAL
               DEBUG_SERIAL.print(temp);
#endif
               result[i]=temp;
               i++;
          }
          delay(1);
     }

     // Terminate the string
     result[resultlength-1]='\0';

#ifdef DEBUG_SERIAL
     DEBUG_SERIAL.println(F("\nDone.."));
#endif
     return i;
}

int NEWGSM::readCellData(int &mcc, int &mnc, long &lac, long &cellid)
{
     if (getStatus()==IDLE)
          return 0;

     //_tf.setTimeout(_GSM_DATA_TOUT_);
     //_cell.flush();
     SimpleWriteln(F("AT+QENG=1,0"));
     SimpleWriteln(F("AT+QENG?"));
     if(WaitResp(5000, 50, F("+QENG"))!=RX_FINISHED_STR_NOT_RECV)
          return 0;

     //mcc=_tf.getValue(); // The first one is 0
     mcc=_cell.read();
     //mcc=_tf.getValue();
     mcc=_cell.read();
     //mnc=_tf.getValue();
     mnc=_cell.read();
     //lac=_tf.getValue();
     lac=_cell.read();
     //cellid=_tf.getValue();
     cellid=_cell.read();

     WaitResp(5000, 50, F("+OK"));
     SimpleWriteln(F("AT+QENG=1,0"));
     WaitResp(5000, 50, F("+OK"));
     return 1;
}

boolean NEWGSM::readSMS(char* msg, int msglength, char* number, int nlength)
{
#ifdef ERROR_SERIAL
	ERROR_SERIAL.println(F("This method is deprecated! Please use GetSMS in the SMS class."));
#endif // DEBUG

     long index;
     char *p_char;
     char *p_char1;

     /*
     if (getStatus()==IDLE)
       return false;
     */
     _tf.setTimeout(_GSM_DATA_TOUT_);

     //_cell.flush();
     WaitResp(500, 500);
     SimpleWriteln(F("AT+CMGL=\"REC UNREAD\",1"));

     WaitResp(5000, 500);
     if(IsStringReceived("+CMGL")) {

          //index
          p_char = strchr((char *)(comm_buf),'+CMGL');
          p_char1 = p_char+3;  //we are on the first char of string
          p_char = p_char1+1;
          *p_char = 0;
          index=atoi(p_char1);

          p_char1 = p_char+1;
          p_char = strstr((char *)(p_char1), "\",\"");
          p_char1 = p_char+3;
          p_char = strstr((char *)(p_char1), "\",\"");
          if (p_char != NULL) {
               *p_char = 0;
          }
          strcpy(number, (char *)(p_char1));
          //////

          p_char1 = p_char+3;
          p_char = strstr((char *)(p_char1), "\",\"");
          p_char1 = p_char+3;

          p_char = strstr((char *)(p_char1), "\n");
          p_char1 = p_char+1;
          p_char = strstr((char *)(p_char1), "\n");
          if (p_char != NULL) {
               *p_char = 0;
          }
          strcpy(msg, (char *)(p_char1));

          SimpleWrite(F("AT+CMGD="));
          SimpleWriteln(index);
          // DEBUG_SERIAL.print("VAL= ");
          // DEBUG_SERIAL.println(index);
          WaitResp(5000, 50, str_ok);
          return true;
     };
     return false;
};

boolean NEWGSM::readCall(char* number, int nlength)
{
     int index;

     if (getStatus()==IDLE)
          return false;

     //_tf.setTimeout(_GSM_DATA_TOUT_);
     if(WaitResp(5000, 50, F("+CLIP: \""))!=RX_FINISHED_STR_RECV)
          //if(_tf.find("+CLIP: \""))
     {
		 _tf.getString("", "\"", number, nlength);

          SimpleWriteln(F("ATH"));
          delay(1000);
          //_cell.flush();
          return true;
     };
     return false;
};

boolean NEWGSM::call(char* number, unsigned int milliseconds)
{
     if (getStatus()==IDLE)
          return false;

     //_tf.setTimeout(_GSM_DATA_TOUT_);

     SimpleWrite(F("ATD"));
     SimpleWrite(number);
     SimpleWriteln(F(";"));
     delay(milliseconds);
     SimpleWriteln(F("ATH"));

     return true;

}

int NEWGSM::setPIN(char *pin)
{
     //Status = READY or ATTACHED.
     if((getStatus() != IDLE))
          return 2;

     //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

     //_cell.flush();

     //AT command to set PIN.
     SimpleWrite(F("AT+CPIN="));
     SimpleWriteln(pin);

     //Expect str_ok.

     if(WaitResp(5000, 50, str_ok)!=RX_FINISHED_STR_NOT_RECV)
          return 0;
     else
          return 1;
}

int NEWGSM::changeNSIPmode(char mode)
{
     //_tf.setTimeout(_TCP_CONNECTION_TOUT_);

     //if (getStatus()!=ATTACHED)
     //    return 0;

     //_cell.flush();

     SimpleWrite(F("AT+QIDNSIP="));
     SimpleWriteln(mode);
     if(WaitResp(5000, 50, str_ok)!=RX_FINISHED_STR_NOT_RECV) return 0;
     //if(!_tf.find(str_ok)) return 0;

     return 1;
}

int NEWGSM::getCCI(char *cci)
{
     //Status must be READY
     if((getStatus() != READY))
          return 2;

     //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

     //_cell.flush();

     //AT command to get CCID.
     SimpleWriteln(F("AT+QCCID"));

     //Read response from modem

     _tf.getString("AT+QCCID\r\r\r\n","\r\n",cci, 21);

     //Expect str_ok.
     if(WaitResp(5000, 50, str_ok)!=RX_FINISHED_STR_NOT_RECV)
          return 0;
     else
          return 1;
}

int NEWGSM::getIMEI(char *imei)
{

     //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

     //_cell.flush();

     //AT command to get IMEI.
     SimpleWriteln(F("AT+GSN"));

     //Read response from modem
//#ifdef UNO
//     _tf.getString("\r\n","\r\n",imei, 16);
//#endif
//#ifdef MEGA
//     _cell.getString("\r\n","\r\n",imei, 16);
//#endif

     //Expect str_ok.
     if(WaitResp(5000, 50, str_ok)!=RX_FINISHED_STR_NOT_RECV){
     	memcpy(imei,comm_buf+2,15);
     	imei[15]='\0';
          return 0;
     }else
          return 1;
}

