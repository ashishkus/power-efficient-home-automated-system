// This #include statement was automatically added by the Particle IDE.

// This #include statement was automatically added by the Particle IDE.
#include <TimeAlarms.h>


#include <Adafruit_ADS1X15.h>
#include "application.h"

// Thinkspeak channel information
String writeAPIKey = "ISXRI9J4RJQQLE3G";
String channelID = "671082";

// TCP socket initialize
TCPClient client;
// This #include statement was automatically added by the Particle IDE.


// defines pins numbers





Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

double offsetI;
double filteredI;
double sqI,sumI;
int16_t sampleI;
double Irms;

double squareRoot(double fg)  
{
  double n = fg / 2.0;
  double lstX = 0.0;
  while (n != lstX)
  {
    lstX = n;
    n = (n + fg / n) / 2.0;
  }
  return n;
}

double calcIrms(unsigned int Number_of_Samples)
{
    //int16_t adc0, adc1, adc2, adc3;
  /* Be sure to update this value based on the IC and the gain settings! */
  float multiplier =0.05F;    /* ADS1115 @ +/- 4.096V gain (16-bit results) */
  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleI = ads.readADC_Differential_0_1();

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset, 
  //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI-offsetI)/1024);
    filteredI = sampleI - offsetI;
    //filteredI = sampleI * multiplier;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum 
    sumI += sqI;
  }
  
  Irms = squareRoot(sumI / Number_of_Samples)*multiplier; 

  //Reset accumulators
  sumI = 0;
//--------------------------------------------------------------------------------------       
 
  return Irms;
}


int relay = D1; //pin to which relay is connected
int boardLed = D7;
bool vin = LOW; //a virtual boolean variable
bool motor_state=LOW;
bool ac_state=LOW;
int curr_time2=0;
int curr_time1=0;
int motor = D11;
int ac = D3;
int h;
int m;
int s;
int diff;
int m1;
int motor_time;
double amt=0;
double ac_amt=0;
double motor_amt=0;

int i=0;

unsigned long diff1 = 0;                         // time between pump runs
  


void setup() {
    Time.zone(+0.5);

  ads.setGain(GAIN_TWO);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.begin();
  
  
  // Setup GPIO
  pinMode(relay,OUTPUT); // relay pin is set as output
  pinMode(boardLed,OUTPUT);
   pinMode(motor,OUTPUT);
   pinMode(ac,OUTPUT);
  digitalWrite(relay,LOW);
  digitalWrite(motor,LOW);
  digitalWrite(ac,LOW);
  
  digitalWrite(boardLed,HIGH);
  // Subscribe to events published by IFTTT using Particle.subscribe
  Particle.subscribe("Turn_Light", myHandler); //turning off function declaration
  Particle.subscribe("Turn_Motor",Handler); //turning off function declaration
  Particle.subscribe("Turn_AC", thHandler); //turning on function declaration
  
 

}   

        
// loop() runs continuously, it's our infinite loop.
void loop(){
    double kWmh;
    
  double current = calcIrms(2048);
  //  Particle.publish(String(current));
    delay(500);
  double power =(250*current)/1000;
  //Particle.publish(String(power));
  if(current > 0.001000){
     i=i+8;
     delay(1000);
	
	int diff1=i;
 //	Particle.publish(String(diff1));
 double kWmh = (power*diff1)/(3600);
 //double amt2=(80+((150*kWmh)/1000));
  amt = (5*kWmh);
 Particle.publish(String(amt));
  // Must convert data to Strings, make sure you use capital "S" in Strings
          ThingSpeakUpdate("field3="+String(amt)+"&field2="+String(motor_amt)+"&field1="+String(ac_amt));
          
          // I put this delay in place so we don't flood Thingspeak but you should really use a timer, delays screw with the sparkcloud connection some times.
          delay(1000);

 Particle.publish(String(kWmh));
            
         
  
  }
  else {
      i=0;
  }
  // wait one second between clock display
  diff=(curr_time2-curr_time1);

  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
  motor_time=((42*60000)+75000+60000);
  if(h==15 && m>=42 && m1<=motor_time){
      
      
          digitalWrite(motor,HIGH);
           Particle.publish("e");
          
          
      }
  else if (motor_state==HIGH)
     {
         digitalWrite(motor,HIGH);
         //curr_time1=s;
         
         motor_amt=amt;
         Particle.publish("bye");
        //delay(500);
         
     }
else{
      digitalWrite(motor,LOW);
      motor_amt=0;
      
  }

 if (ac_state==HIGH)
     {
         digitalWrite(ac,HIGH);
        
         ac_amt=amt;
        Particle.publish("hi");
        //delay(500);
         
         
     }
else if (ac_state==LOW)
      {
         digitalWrite(ac,LOW);
         ac_amt=0;
       
     }
    
   
  
     if (vin==HIGH)
     {
         digitalWrite(relay,HIGH);
         
     }
else if (vin==LOW)
      {
         digitalWrite(relay,LOW);
     }
     
  

}

//our events are called when IFTTT applets are triggered


void myHandler(const char *event, const char *data)
{
    int f=atoi(data);
    if(f==1)
    {
        vin=HIGH;
       
    }
    else if(f==0)
    {
    vin=LOW;
   
    }
    
}

void Handler(const char *event, const char *data)
{
    int d=atoi(data);
 
    if(d==1)
    {
        motor_state=HIGH;
        curr_time1=millis();
        
    
    }
    else if(d==0)
    {
        motor_state=LOW;
       
        curr_time2=millis();
    }
    
    
}
void thHandler(const char *event, const char *data)
{
    int e=atoi(data);
    if(e==1)
    {
        ac_state=HIGH;
        
    }
    else if(e==0)
    {
        ac_state=LOW;
       
    }

     
}

void ThingSpeakUpdate(String tsData)
{

    
    // Connecting and sending data to Thingspeak
    if(client.connect("api.thingspeak.com", 80))
    {
        
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(tsData.length());
        client.print("\n\n");
        
        client.println(tsData); //the ""ln" is important here.
    
        // This delay is pivitol without it the TCP client will often close before the data is fully sent
        delay(200);
        
    }
   
    
    if(!client.connected()){
        client.stop();
    }
    client.flush();
    client.stop();
}

void digitalClockDisplay()
{
  // digital clock display of the time
  h=Time.hour();
  m=Time.minute();
  s=Time.second();
  m1=(m*60*1000)+(s*1000);
  Particle.publish(String(h));
  Particle.publish(String(m));
  
}


