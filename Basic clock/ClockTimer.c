#include<reg51.h>
sbit rs=P3^0;
sbit en=P3^1; 
sbit SDA=P1^0;              //data pin for ds1307 bi-directional bit
sbit SCL=P1^1;             //clock pin for ds1307 output bit
sbit alarm_set=P1^2;       //set alarm input bit
sbit alarm_beep=P1^3;      //alarm indicator output bit
sbit next=P3^2;            //increament digit
sbit inc=P3^3;            //increament value input bit
sbit set=P3^4;           //set time input bit
void set_time();        //time/date/day set function
void display();          //time/date/day display function 
void alarm();            //alarm set function
char ack;
unsigned char day1=1;
unsigned char k,x;
unsigned int date=1, mon=1, year=0x14, hour=0, min=0, sec=0; 
unsigned int alarm_min, alarm_hour; 
void I2CStart()
{
SDA=1;SCL=1,SDA=0,SCL=0;
}                             //"start" function for communicate with ds1307 RTC
void I2CStop()
{
SDA=0,SCL=1,SDA=1;
}                                //"stop" funci=tion for communicate wit ds1307 RTC
unsigned char I2CSend(unsigned char Data)      //send data to ds1307 
{
char i;
static bit ack_bit;
for(i=0;i<8;i++)
{
if(Data & 0x80) SDA=1;
else SDA=0;
SCL=1;
Data<<=1;
SCL=0;
}
SDA=1,SCL=1;
ack_bit=SDA;
SCL=0;
return ack_bit;
}
unsigned char I2CRead(char ack)           //receive data from ds1307
{
unsigned char i, Data=0;
SDA=1;
for(i=0;i<8;i++)
{
Data<<=1;
do{SCL=1;}
while(SCL==0);
if(SDA) Data|=1;
SCL=0;
}
if(ack)SDA=0;
else SDA=1;
SCL=1;
SCL=0;
SDA=1;
return Data;
}
void delay(unsigned int time)                // delay function
{
unsigned int i,j;
for(i=0;i<time;i++)
for(j=0;j<1000;j++);}
void lcdcmd(unsigned char cmd)               // send command to LCD
{
P2=cmd;
rs=0,en=1;
delay(2),en=0;
}
void lcddata(unsigned char item)                //send data to LCD
{
P2=item;
rs=1,en=1;
delay(2),en=0;
} 
void lcd_init()                         //LCD initialize
{
  lcdcmd(0x38);
  lcdcmd(0x06);
  lcdcmd(0x0c);
} 
void lcdnum(unsigned char convert)          //convert data in to ascii
{
 lcddata(convert/16+48);
 lcddata(convert%16+48);
}
void print(char *str)                     // print data/values on LCD
{
 while(*str!='\0')
 {
  lcddata(*str);
  str++;
 }
}
void day(char d)                         // Function for display day on LCD
{
switch(d)
{
  case 0:
  print("DAY");
  break;
  case 1:
  print("SUN");
  break;
  case 2:
  print("MON");
  break;
  case 3:
  print("TUE");
  break;
  case 4:
  print("WED");
  break;
  case 5:
  print("THU");
  break;
  case 6:
  print("FRI");
  break;
  case 7:
  print("SAT");
  break;
 }
 }
void main() // main function
{
 lcd_init();
 lcdcmd(0x80);
 print("RTC clock by ");
 lcdcmd(0xc0);
 print("Saddam Khan  ");
 delay(300);
 lcdcmd(0x01);
 while(1)
 {
 if(set==0)  // check time set button press
 {
 I2CStart();
 I2CSend(0xD0);
 I2CSend(0x00);
 I2CSend(0x00);
 I2CSend(0x00);
 I2CSend(0x00);
 I2CSend(0x01);
 I2CSend(0x01);
 I2CSend(0x01);
 I2CSend(0x00); 
 I2CSend(0x80); 
 I2CStop();
 k=1;
 set_time();   // call time set function
 I2CStart();
 I2CSend(0xD0);
 I2CSend(0x00);
 I2CSend(0x00);
 I2CSend(min);
 I2CSend(hour);
 I2CSend(day1);
 I2CSend(date);
 I2CSend(mon);
 I2CSend(year); 
 I2CSend(0x80); 
 I2CStop(); 
 }
 if(alarm_set==0)    //check alarm set button press
 {    
  alarm();    //call alarm set function
  while(alarm_set==0);
  k=5;
 }
  I2CStart();
  I2CSend(0xD0);
  I2CSend(0x00);
  I2CStart();
  I2CSend(0xD1);
  sec=I2CRead(1);
  min=I2CRead(1);
  hour=I2CRead(1);
  day1=I2CRead(1);
  date=I2CRead(1);
  mon=I2CRead(1);
  year=I2CRead(1);
  I2CStop();
  lcdcmd(0x0c);
  if(hour==alarm_hour && min==alarm_min)    // check alarm time with real time
  { 
   k=7;
   if(sec>88){k=0;}    //k=0; remove alarm
   alarm_beep=1;    //alarm output set
  } 
 else
  {
   alarm_beep=0;    //alarm output reset
  }  
   display();    //display time/date/day 
   delay(1);
 }
 }
void set_time() //time set function
{
 lcdcmd(0x0e);
 while(k<7)
 {
  while(k==3) //set date
  {
   x=year%4;
   if(inc==0)
   {
    date++;while(inc==0);
    if(date==10){date=date+6;} //covert decimal value into BCD
    if(date==26){date=date+6;}
    if(date==42){date=date+6;}
    if(x==1 && mon==2 && date==41){date=1;} //check for 28 day febuary
    if(x==0 && mon==2 && date==48){date=1;} //check for 29 day febuary
    if((date==49) && (mon==4) || (mon==6) || (mon==9) || (mon==17)){date=1;
    } // check for 30 day month
   if(date==50){date=1;} // check for 31 day month
   display();}
   if(next==0){k=4;while(next==0);} //check for next digit
   lcdcmd(0x81);
  } 
  while(k==2) //set month
  {
   if(inc==0)
   {
    mon++;while(inc==0);
    if(mon==10){mon=mon+6;}
    if(mon==19){mon=1;}   //check for end of year
    display();
    }
    if(next==0){k=3;while(next==0);}
    lcdcmd(0x84);
   }
   while(k==1)  //set year
  {
   if(inc==0)
   {
   year++;while(inc==0);
   if(year==10){year=year+6;}
   if(year==26){year=year+6;}
   if(year==41){year=0;} 
   display(); 
   }
   if(next==0){k=2;while(next==0);}
   lcdcmd(0x89);
  }
  while(k==4)    //set day
  {
   if(inc==0)
   {
   day1++;while(inc==0);
   if(day1==8){day1=1;}
   display(); 
   }
   if(next==0){
   k=5;while(next==0);
   }
   lcdcmd(0x8f);
  }
  while(k==5)   //set hour
  {
   if(inc==0)
   {
   hour++;while(inc==0);
   if(hour==10){
   hour=hour+6;
   }
   if(hour==26){
   hour=hour+6;
   }
   if(hour==36)
   {
   hour=0;
   }
   display();
   }
   if(next==0){
   k=6;while(next==0);
   }
   lcdcmd(0xc1);
  }
  while(k==6)    //set min
  {
   if(inc==0)
   {
   min++;while(inc==0);
   if(min==10)
   {
   min=min+6;
   }
   if(min==26)
  {
 min=min+6;
 }
   if(min==42){min=min+6;}
   if(min==58){min=min+6;}
   if(min==74){min=min+6;}
   if(min==90){min=min+6;}
   if(min==96){min=0;}
   display();}
   if(next==0){k=10;while(next==0);}
   lcdcmd(0xc4);
  }
 }
} 
void display() //function to display time/date/day on LCD
{
  lcdcmd(0x80);
  lcdnum(date);
  lcdcmd(0x82);
  print("-");
  lcdcmd(0x83);
  lcdnum(mon);
  lcdcmd(0x85);
  print("-20");
  lcdcmd(0x88);
  lcdnum(year);
  lcdcmd(0x8a);
  print("  ");
  lcdcmd(0x8c);
  day(day1);
  lcdcmd(0xc0);
  lcdnum(hour);
  lcdcmd(0xc2);
  print(":");
  lcdcmd(0xc3);
  lcdnum(min);
  lcdcmd(0xc5);
  print(":");
  lcdcmd(0xc6);
  lcdnum(sec);
  if(k==7)
  { 
   if(sec%2==0)
    {
    lcdcmd(0xca);
    print("ALARM");
    }
else
   {
print("       ");
   }
  }
  else if(k==5)
  {
    lcdcmd(0xca);
    print("ALARM");
  }
  else
  {
  lcdcmd(0xc8);
  print("         "); 
  }
  }
 void alarm() // set alarm
 {
  unsigned int k=1;
  alarm_hour=0, alarm_min=0;
  hour=alarm_hour;
  min=alarm_min;
  lcdcmd(0x0e);
  lcdcmd(0x01);
  print("Set alarm      ");
  delay(200);
  display();
  while(k<3) //set hour for alarm
  {
   while(k==1)
   {
    if(inc==0)
    {alarm_hour++;while(inc==0);
    if(alarm_hour==10){alarm_hour=alarm_hour+6;}
    if(alarm_hour==26){alarm_hour=alarm_hour+6;}
    if(alarm_hour==36){alarm_hour=0;}
hour=alarm_hour;
    display();}
    if(next==0){k=2;while(next==0);}
    lcdcmd(0xc1);
   }
     //set min for alarm
   while(k==2)
   {
    if(inc==0)
    {alarm_min++;while(inc==0);
    if(alarm_min==10){alarm_min=alarm_min+6;}
    if(alarm_min==26){alarm_min=alarm_min+6;}
    if(alarm_min==42){alarm_min=alarm_min+6;}
    if(alarm_min==58){alarm_min=alarm_min+6;}
    if(alarm_min==74){alarm_min=alarm_min+6;}
    if(alarm_min==90){alarm_min=alarm_min+6;}
    if(alarm_min==96){alarm_min=0;}
    min=alarm_min;
    display();}
    if(next==0){k=3;while(next==0);}
    lcdcmd(0xc4);
  }
 }
 }






//without time and alarm set

#include<reg51.h>
sbit rs=P3^0;
sbit en=P3^1; 
sbit SDA=P1^0;
sbit SCL=P1^1;
char ack;
char array[7];
void I2CStart(){SDA=1;SCL=1,SDA=0,SCL=0;}
void I2CStop(){SDA=0,SCL=1,SDA=1;}
unsigned char I2CSend(unsigned char Data)
{
char i;
static bit ack_bit;
for(i=0;i<8;i++)
{
if(Data & 0x80) SDA=1;
else SDA=0;
SCL=1;
Data<<=1;
SCL=0;
}
SDA=1,SCL=1;
ack_bit=SDA;
SCL=0;
return ack_bit;
}
unsigned char I2CRead(char ack)
{
unsigned char i, Data=0;
SDA=1;
for(i=0;i<8;i++)
{
 Data<<=1;
 do{SCL=1;}
 while(SCL==0);
 if(SDA) Data|=1;
 SCL=0;
}
if(ack)SDA=0;
else SDA=1;
SCL=1;
SCL=0;
SDA=1;
return Data;
}
void delay(int time)
{unsigned int i,j;
for(i=0;i<time;i++)
for(j=0;j<1000;j++);}
void LCD_command(unsigned char cmd)
{
 P2=cmd;
 rs=0,en=1;
 delay(5),en=0;
}
void LCD_senddata(unsigned char item)
{
 P2=item;
 rs=1,en=1;
 delay(1),en=0;
} 
void LCD_init()
{
  LCD_command(0x38);
  LCD_command(0x06);
  LCD_command(0x0c);
} 
void LCD_sendnum(unsigned char khan)
{
 LCD_senddata(khan/16+48);
 LCD_senddata(khan%16+48);
}
void print(char *str)
{
 while(*str!='\0')
 {
  LCD_senddata(*str);
  str++;
 }
}
void day(char d)
{
  if(d==01)
  print("SUN");
  if(d==02)
  print("MON");
  if(d==03)
  print("TUE");
  if(d==04)
  print("WED");
  if(d==05)

 print("THU");
 if(d==06)
  print("FRI");
  if(d==07)
  print("SAT");
 }
void main()
{
 int s;
 LCD_init();
 LCD_command(0x80);
 print("Saddam Khan");
 delay(200);
 LCD_command(0x01);
 I2CStart();
 I2CSend(0xD0);
 I2CSend(0x00);
 I2CSend(0x01);
 I2CSend(0x42);
 I2CSend(0x17);
 I2CSend(0x01);
 I2CSend(0x12);
 I2CSend(0x01);
 I2CSend(0x14); 
 I2CSend(0x80);
 I2CStop(); 
 while(1)
 {
  I2CStart();
  I2CSend(0xD0);
  I2CSend(0x00);
  I2CStart();
  I2CSend(0xD1);
  for(s=0;s<7;s++)
  {array[s]=I2CRead(1);}
  I2CStop();
  LCD_command(0x80);
  LCD_sendnum(array[4]);
  print("-");
  LCD_sendnum(array[5]);
  print("-20");
  LCD_sendnum(array[6]);
  LCD_command(0x8c);
  day(array[3]);
  LCD_command(0xc0);
  LCD_sendnum(array[2]);
  print(":");
  LCD_sendnum(array[1]);
  print(":");
  LCD_sendnum(array[0]);
  LCD_command(0xc9);  
 }
}