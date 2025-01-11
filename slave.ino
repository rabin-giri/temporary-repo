#define ON HIGH
#define OFF LOW

#define RED_0_P 11
#define YELLOW_0_P 12
#define GREEN_0_P 13
#define ARROW_0_P 10
#define RED_0_S 2
#define YELLOW_0_S 9
#define GREEN_0_S 3
#define ARROW_0_S 8
#define RED_PED_0 4
#define GREEN_PED_0 7

#define RED_1_P 35
#define YELLOW_1_P 33
#define GREEN_1_P 31
#define ARROW_1_P 37
#define RED_1_S 29
#define YELLOW_1_S 39
#define GREEN_1_S 27
#define ARROW_1_S 41
#define RED_PED_1 25
#define GREEN_PED_1 43


#define RED_2_P 48
#define YELLOW_2_P 50
#define GREEN_2_P 52
#define ARROW_2_P 46
#define RED_2_S 53
#define YELLOW_2_S 44
#define GREEN_2_S 51
#define ARROW_2_S 42
#define RED_PED_2 49
#define GREEN_PED_2 40

#define RED_3_P 24
#define YELLOW_3_P 26
#define GREEN_3_P 28
#define ARROW_3_P 22
#define RED_3_S 30
#define YELLOW_3_S 32
#define GREEN_3_S 34
#define ARROW_3_S 36
#define RED_PED_3 47
#define GREEN_PED_3 38

enum Pedestrian { RED_PEDESTRIAN, GREEN_PEDESTRIAN };

struct light_cmd
{
   uint8_t msg1;
   uint8_t msg2;
   Pedestrian Pedestrian_light; 
}; 

light_cmd light[4];

uint8_t RED_P[4];
uint8_t YELLOW_P[4];
uint8_t GREEN_P[4];
uint8_t ARROW_P[4];

uint8_t RED_S[4];
uint8_t YELLOW_S[4];
uint8_t GREEN_S[4];
uint8_t ARROW_S[4];


uint8_t RED_PED[4];
uint8_t GREEN_PED[4];


unsigned char serial_buf[11];
uint8_t serial_buf_pos=0;

unsigned char serial_buf_conflict[11];
uint8_t serial_buf_conflict_pos=0;
unsigned long reftime_conflict_card_timeout;

bool conflict_flag=false;
bool blink_mode_flag=true;
unsigned long reftime_communication_timeout;
void check_communication_wire(void);
void check_communication_confict_card(void);

void turn_off_all_light(void);
void parse_serial_data(void);
void phase_control_blink(void);
void phase_control_normal(void);

void setup() 
{
  Serial3.begin(38400); 
  Serial.begin(9600);  
  //Serial.begin(115200);
  turn_off_all_light();  

  pinMode(RED_0_P,OUTPUT);pinMode(GREEN_0_P,OUTPUT);pinMode(YELLOW_0_P,OUTPUT);pinMode(ARROW_0_P,OUTPUT);
  pinMode(RED_1_P,OUTPUT);pinMode(GREEN_1_P,OUTPUT);pinMode(YELLOW_1_P,OUTPUT);pinMode(ARROW_1_P,OUTPUT);
  pinMode(RED_2_P,OUTPUT);pinMode(GREEN_2_P,OUTPUT);pinMode(YELLOW_2_P,OUTPUT);pinMode(ARROW_2_P,OUTPUT);
  pinMode(RED_3_P,OUTPUT);pinMode(GREEN_3_P,OUTPUT);pinMode(YELLOW_3_P,OUTPUT);pinMode(ARROW_3_P,OUTPUT);

  pinMode(RED_0_S,OUTPUT);pinMode(GREEN_0_S,OUTPUT);pinMode(YELLOW_0_S,OUTPUT);pinMode(ARROW_0_S,OUTPUT);
  pinMode(RED_1_S,OUTPUT);pinMode(GREEN_1_S,OUTPUT);pinMode(YELLOW_1_S,OUTPUT);pinMode(ARROW_1_S,OUTPUT);
  pinMode(RED_2_S,OUTPUT);pinMode(GREEN_2_S,OUTPUT);pinMode(YELLOW_2_S,OUTPUT);pinMode(ARROW_2_S,OUTPUT);
  pinMode(RED_3_S,OUTPUT);pinMode(GREEN_3_S,OUTPUT);pinMode(YELLOW_3_S,OUTPUT);pinMode(ARROW_3_S,OUTPUT);


  pinMode(RED_PED_0,OUTPUT);pinMode(GREEN_PED_0,OUTPUT);pinMode(RED_PED_1,OUTPUT);pinMode(GREEN_PED_1,OUTPUT);
  pinMode(RED_PED_2,OUTPUT);pinMode(GREEN_PED_2,OUTPUT);pinMode(RED_PED_3,OUTPUT);pinMode(GREEN_PED_3,OUTPUT);
  

  RED_P[0]=RED_0_P;         RED_P[1]=RED_1_P;         RED_P[2]=RED_2_P;         RED_P[3]=RED_3_P;
  GREEN_P[0]=GREEN_0_P;     GREEN_P[1]=GREEN_1_P;     GREEN_P[2]=GREEN_2_P;     GREEN_P[3]=GREEN_3_P;
  YELLOW_P[0]=YELLOW_0_P;   YELLOW_P[1]=YELLOW_1_P;   YELLOW_P[2]=YELLOW_2_P;   YELLOW_P[3]=YELLOW_3_P;
  ARROW_P[0]=ARROW_0_P;     ARROW_P[1]=ARROW_1_P;     ARROW_P[2]=ARROW_2_P;     ARROW_P[3]=ARROW_3_P;

  RED_S[0]=RED_0_S;         RED_S[1]=RED_1_S;         RED_S[2]=RED_2_S;         RED_S[3]=RED_3_S;
  GREEN_S[0]=GREEN_0_S;     GREEN_S[1]=GREEN_1_S;     GREEN_S[2]=GREEN_2_S;     GREEN_S[3]=GREEN_3_S;
  YELLOW_S[0]=YELLOW_0_S;   YELLOW_S[1]=YELLOW_1_S;   YELLOW_S[2]=YELLOW_2_S;   YELLOW_S[3]=YELLOW_3_S;
  ARROW_S[0]=ARROW_0_S;     ARROW_S[1]=ARROW_1_S;     ARROW_S[2]=ARROW_2_S;     ARROW_S[3]=ARROW_3_S;
  
  RED_PED[0]=RED_PED_0;     RED_PED[1]=RED_PED_1;     RED_PED[2]=RED_PED_2;     RED_PED[3]=RED_PED_3;
  GREEN_PED[0]=GREEN_PED_0; GREEN_PED[1]=GREEN_PED_1; GREEN_PED[2]=GREEN_PED_2; GREEN_PED[3]=GREEN_PED_3;    
  

}

void loop() 
{ 
   check_communication_wire();
   check_communication_confict_card();
   if( (unsigned long)(millis()-reftime_communication_timeout)>3000)
   {
    reftime_communication_timeout=millis();
    turn_off_all_light();
   }

   if( (unsigned long)(millis()-reftime_conflict_card_timeout)>3000)
   {
    reftime_conflict_card_timeout=millis();
    conflict_flag=false;
   }
}
void parse_serial_data(void)
{
   
   if( serial_buf[0]==0xff && serial_buf[1]==0xff && serial_buf_pos==6 )
   {
      uint8_t sum=0;
      for(int i=2; i<6; i++)
      sum+=serial_buf[i];
      if( sum==0 )
      {
          reftime_communication_timeout=millis();
          if(conflict_flag)
          {
           turn_off_all_light();
           return;
          }
          if( (serial_buf[2]==0) && (serial_buf[3]==0) && (serial_buf[4]==0))  
          {
           if(blink_mode_flag==false)
           {
              turn_off_all_light();
              blink_mode_flag=true;
           }
           phase_control_blink();
          }
          
          else
          { 
            blink_mode_flag=false;
            for(uint8_t i=0; i<4 ; i++)
            {
              uint8_t temp=serial_buf[2+(i/2)];
              if( (i%2)==1 )
              {
                light[i].msg1=(temp>>4) & 0x03;
                light[i].msg2=(temp>>6) & 0x03;
              }
              else
              {
                light[i].msg1=temp & 0x03;
                light[i].msg2=(temp>>2) & 0x03;
              }
              temp=serial_buf[4];
              if( (temp & 0xf0)==0) 
              {
                if((temp & (1<<i))!=0 ) light[i].Pedestrian_light=GREEN_PEDESTRIAN;
                else light[i].Pedestrian_light=RED_PEDESTRIAN;
              }
            } 
            phase_control_normal();          
          }
      }
   }

}

void check_communication_wire(void)
{
  static long Time;
  if( Serial3.available() )
  {
    memset(serial_buf,NULL,10);
    serial_buf_pos=0;
    Time=millis();
    while( (unsigned long)(millis()-Time)<10 )
    {
      if( Serial3.available() )
      {
        if( serial_buf_pos<8 )
        serial_buf[serial_buf_pos++]= Serial3.read();
        else
        Serial3.read();
        Time=millis();
      }
     
    }
    parse_serial_data();
  }
}

void check_communication_confict_card(void)
{
  static long Time;
  if( Serial.available() )
  {
    memset(serial_buf_conflict,NULL,10);
    serial_buf_conflict_pos=0;
    Time=millis();
    while( (unsigned long)(millis()-Time)<10 )
    {
      if( Serial.available() )
      {
        if( serial_buf_conflict_pos<8 )
        serial_buf_conflict[serial_buf_conflict_pos++]= Serial.read();
        else
        Serial.read();
        Time=millis();
      }
     
    }
    //Serial.write(serial_buf_conflict,serial_buf_conflict_pos);
    reftime_conflict_card_timeout=millis(); 
    if(serial_buf_conflict[0]=='c' && serial_buf_conflict[1]=='o' && serial_buf_conflict[2]=='n')
    {
      conflict_flag=true;
      //Serial.println("conflict detect");
     }
     else
     { 
      conflict_flag=false;
      //Serial.println("no conflict");
     }
  }
}


void phase_control_normal(void)
{
  for(uint8_t i=0; i<4 ; i++)
  {
    switch(light[i].msg1)
    {
      case 1:
      digitalWrite(RED_P[i],ON);   digitalWrite(YELLOW_P[i],OFF);  digitalWrite(GREEN_P[i],OFF);
      digitalWrite(RED_S[i],ON);   digitalWrite(YELLOW_S[i],OFF);  digitalWrite(GREEN_S[i],OFF);
      break;
      case 2:
      digitalWrite(RED_P[i],OFF);  digitalWrite(YELLOW_P[i],ON);   digitalWrite(GREEN_P[i],OFF);
      digitalWrite(RED_S[i],OFF);  digitalWrite(YELLOW_S[i],ON);   digitalWrite(GREEN_S[i],OFF);
      break;
      case 3:
      digitalWrite(RED_P[i],OFF);  digitalWrite(YELLOW_P[i],OFF);  digitalWrite(GREEN_P[i],ON);
      digitalWrite(RED_S[i],OFF);  digitalWrite(YELLOW_S[i],OFF);  digitalWrite(GREEN_S[i],ON);
      break;
    }


    if(light[i].msg2==1 || light[i].msg2==3)
    {
      digitalWrite(ARROW_P[i],ON);
      digitalWrite(ARROW_S[i],ON);
    }
    else 
    {
      digitalWrite(ARROW_P[i],OFF);
      digitalWrite(ARROW_S[i],OFF);
    }

    if( light[i].Pedestrian_light==RED_PEDESTRIAN )
    {
      digitalWrite(GREEN_PED[i],OFF);
      digitalWrite(RED_PED[i],ON);
    }
    else
    {
      digitalWrite(GREEN_PED[i],ON);
      digitalWrite(RED_PED[i],OFF);
    }
  }
  
}


void phase_control_blink(void)
{
  static long timeRef=millis();
  static bool flag=false;
  if( (unsigned long)(millis()-timeRef)>1000 )
  {
    timeRef=millis();
    for(uint8_t i=0; i<4; i++)
    {
      if(flag)
      {
         digitalWrite(YELLOW_P[i],OFF);digitalWrite(YELLOW_S[i],OFF);digitalWrite(RED_PED[i],OFF);
         digitalWrite(GREEN_PED[i],OFF);digitalWrite(RED_PED[i],OFF);
      }
      else
      {
         digitalWrite(YELLOW_P[i],ON);digitalWrite(YELLOW_S[i],ON);digitalWrite(RED_PED[i],ON);
         digitalWrite(GREEN_PED[i],OFF);digitalWrite(RED_PED[i],ON);
      }
    }
    flag=!flag;
  }
}

void turn_off_all_light(void)
{
  digitalWrite(RED_0_P,OFF);digitalWrite(GREEN_0_P,OFF);digitalWrite(YELLOW_0_P,OFF);digitalWrite(ARROW_0_P,OFF);
  digitalWrite(RED_0_S,OFF);digitalWrite(GREEN_0_S,OFF);digitalWrite(YELLOW_0_S,OFF);digitalWrite(ARROW_0_S,OFF);

  digitalWrite(RED_1_P,OFF);digitalWrite(GREEN_1_P,OFF);digitalWrite(YELLOW_1_P,OFF);digitalWrite(ARROW_1_P,OFF);
  digitalWrite(RED_1_S,OFF);digitalWrite(GREEN_1_S,OFF);digitalWrite(YELLOW_1_S,OFF);digitalWrite(ARROW_1_S,OFF);

  digitalWrite(RED_2_P,OFF);digitalWrite(GREEN_2_P,OFF);digitalWrite(YELLOW_2_P,OFF);digitalWrite(ARROW_2_P,OFF);
  digitalWrite(RED_2_S,OFF);digitalWrite(GREEN_2_S,OFF);digitalWrite(YELLOW_2_S,OFF);digitalWrite(ARROW_2_S,OFF);
     
  digitalWrite(RED_3_P,OFF);digitalWrite(GREEN_3_P,OFF);digitalWrite(YELLOW_3_P,OFF);digitalWrite(ARROW_3_P,OFF);
  digitalWrite(RED_3_S,OFF);digitalWrite(GREEN_3_S,OFF);digitalWrite(YELLOW_3_S,OFF);digitalWrite(ARROW_3_S,OFF);

  digitalWrite(RED_PED_0,OFF);digitalWrite(GREEN_PED_0,OFF);digitalWrite(RED_PED_1,OFF);digitalWrite(GREEN_PED_1,OFF);
  digitalWrite(RED_PED_2,OFF);digitalWrite(GREEN_PED_2,OFF);digitalWrite(RED_PED_3,OFF);digitalWrite(GREEN_PED_3,OFF);
}
