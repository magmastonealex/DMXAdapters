/* 
** This code enables any Arduino-compatable chip to be compatable with DMX USB PRO software.
** Baud rate modification IS required, and may unfortunately require re-compiling.
** (115200 is max supported at 16MHz.
*/

#include <DmxSimple.h>

	void setup() {
  Serial.begin(115200); //Adjust this up if you're running faster.
  pinMode(9,OUTPUT); // 9 is a debug LED pin.
  digitalWrite(9,HIGH);
  delay(100);
  digitalWrite(9,LOW);
  DmxSimple.usePin(10);  //Adjust this based on DMX output pin.
  DmxSimple.maxChannel(128);
  DmxSimple.write(17,255);
  Serial.println("hi"); // Hello, world!
}

char type=0; // type
char st=0;  // state
void loop() {
	char type=0;
	char st=0;


//Much of this will look like black magic if you aren't familiar with the DMX USB PRO protocol.
	if(Serial.available()>0){

		char st=Serial.read();
    if(st==0x7E){ //starting a packet?

  	type=Serial.read();
  	if(type==0x06){
      // DMX packet?
  	}
    //If something's not working quite right, uncomment this. 
    //Some implementations don't seem to agree with my implementation of the status packet (QLC+ works fine)

    /*else
      return; // Lots of packet types.
    }*/
      
  //  Serial.write(type); //Debug, yay
    char szLSB=Serial.read();
    char szMSB=Serial.read();
    unsigned short sz= (szLSB << 8) | szMSB; // Yay, bitshifting...
    unsigned short curPos=0;
    char startCode=0;
    while(curPos <= sz){
    	char curChar=Serial.read();
    	if(type==0x06){
        //Sends a strange start byte in the data.
    		if(startCode==0){
    			if(curChar==0x00){
    				digitalWrite(9,HIGH); 
    			}
    			startCode=1;
    		}else{

        //Do something with the DMX data.
        if(curPos < 128){ //Only write 128 channels, but read the rest
        	if(curChar==0xFF){
        		digitalWrite(9,HIGH); // Turn the LED on if it's full up (Just a sanity check for quick check while live)
        	}  
        	DmxSimple.write(curPos,curChar); 
        }

    }


}
curPos++;
}
    st=Serial.read(); // Read our last byte.
    if(st!=0xE7){

      //We messed something up, the packet was shorter.
    }
    if(type==3){
      //digitalWrite(9,HIGH);
      //Start message:
      Serial.print(0x7E);
      Serial.print(0x03);
      Serial.print(0x05);
      Serial.print(0x00);

      Serial.print(0x03); //Version LSB
      Serial.print(0x01); //Version MSB
      Serial.print(0x09); //Timing 1
      Serial.print(0x01); //Timing 2
      Serial.print((char)40); //Packets per second
      
      Serial.print(0xE7); //Finish message
      
  }else if(type == 0x0A){
   	  Serial.print(0x7E);
  	  Serial.print(0x10);
  	  Serial.print(0x04);
  	  Serial.print(0x00);

      Serial.print(0x01); //Byte1
      Serial.print(0x01); //Byte2
      Serial.print(0x01); //Byte3
      Serial.print(0x01); //Byte4
      
      Serial.print(0xE7);
      
  }
}
}
}
