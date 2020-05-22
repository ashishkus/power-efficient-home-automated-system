#Libraries
import RPi.GPIO as GPIO
import time
 
#GPIO Mode (BOARD / BCM)
GPIO.setmode(GPIO.BCM)
 
#set GPIO Pins
GPIO_TRIGGER = 5
GPIO_ECHO = 6
GPIO_TRIGGER1 = 13
GPIO_ECHO1 = 26
 
#set GPIO direction (IN / OUT)
GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)
GPIO.setup(GPIO_TRIGGER1, GPIO.OUT)
GPIO.setup(11, GPIO.OUT)
GPIO.setup(GPIO_ECHO1, GPIO.IN)
GPIO.setup(27, GPIO.IN)
count=0
def distance(GPIO_TRIGGERR,GPIO_ECHOO):
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGERR, True)
 
    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGERR, False)
 
    StartTime = time.time()
    StopTime = time.time()
 
    # save StartTime
    while GPIO.input(GPIO_ECHOO) == 0:
        StartTime = time.time()
 
    # save time of arrival
    while GPIO.input(GPIO_ECHOO) == 1:
        StopTime = time.time()
 
    # time difference between start and arrival
    TimeElapsed = StopTime - StartTime
    # multiply with the sonic speed (34300 cm/s)
    # and divide by 2, because there and back
    distance = (TimeElapsed * 34300) / 2
 
    return distance
 
if __name__ == '__main__':
    try:
        while True:
            new=GPIO.input(27)
            dist = distance(GPIO_TRIGGER,GPIO_ECHO)
            dist1= distance(GPIO_TRIGGER1,GPIO_ECHO1)
        
            print ("Measured Distance = %.1f cm" % dist)
            print ("         ")
            print ("Measured Distance = %.1f cm" % dist1)
            time.sleep(1)
            
            
            if(dist1<70.0 and dist>70.0):
                print ("IN")
                count=count+1
                print (count)
                time.sleep(2)
                
                
            elif(dist<70.0 and dist1>70.0):
                print ("OUT")
                count=count-1
                print (count)
                time.sleep(2)
                
            if(count<=0):
                GPIO.output(11,False)
                print("off")
        
            elif(count!=0 and new==False):
                GPIO.output(11,False)
                print("off")
            else:
                GPIO.output(11,True)
                print("on")
                
        
                
                
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()
        
