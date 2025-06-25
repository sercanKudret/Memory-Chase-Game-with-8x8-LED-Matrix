#include <string.h>
#include <STDLIBM.H>

#define clk_delay 1
#byte    SSPCON1  =     0xFC6
#bit     SSPEN    =     SSPCON1.5


#define FIRST_CHAR 32
#define LAST_CHAR 125
#define char_spacing 1
unsigned int8 cur_scroll_position=0;
unsigned int16 cur_letter_position=0;
unsigned int16 cur_letter_end_position=0;
unsigned int8 temp_char_spacing=0;
char* scroll_chars;
BYTE cur_char_data[8]={0,0,0,0,0,0,0,0};
unsigned int8 scroll_text_len=0;
BYTE const font_width[]={
    0x07, 0x01, 0x03, 0x05, 0x05, 0x07, 0x07, 0x01, 0x03, 0x03, 
    0x05, 0x07, 0x02, 0x05, 0x02, 0x03, 
    0x05, 0x03, 0x05, 0x05, 0x06, 0x05, 0x05, 0x05, 0x05, 0x05, 
    0x02, 0x02, 0x07, 0x06, 0x07, 0x05, 0x08, 
    0x07, 0x05, 0x05, 0x05, 0x05, 0x04, 0x06, 0x05, 0x01, 0x04,
    0x06, 0x05, 0x07, 0x05, 0x06, 0x05, 0x06, 0x05, 0x05, 0x05,
    0x05, 0x07, 0x07, 0x07, 0x07, 0x06,
    0x03, 0x03, 0x03, 0x05, 0x06, 0x02, 0x04, 0x04, 0x04, 0x04, 
    0x04, 0x03, 0x04, 0x04, 0x01, 0x03, 0x04, 0x01, 0x07, 0x04, 
    0x04, 0x04, 0x04, 0x03, 0x04, 0x03, 0x04, 0x05, 0x07, 0x05, 
    0x05, 0x05, 0x03, 0x01, 0x03,
};

BYTE const font[]={
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 32
    0xBE, // 33
    0x0E, 0x00, 0x0E, // 34
    0xE8, 0x3C, 0xEA, 0x3C, 0x2A, // 35
    0x4C, 0x92, 0xFE, 0x92, 0x64, // 36
    0x0C, 0x12, 0xCC, 0x30, 0x6C, 0x92, 0x60, // 37
    0x60, 0x94, 0x9A, 0xAA, 0x44, 0xE0, 0xA0, // 38
    0x0E, // 39
    0x38, 0x44, 0x82, // 40
    0x82, 0x44, 0x38, // 41
    0x22, 0x14, 0x3E, 0x14, 0x22, // 42
    0x10, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x10, // 43
    0x80, 0x60, // 44
    0x10, 0x10, 0x10, 0x10, 0x10, // 45
    0x60, 0x60, // 46
    0xC0, 0x38, 0x06, // 47
    0x7C, 0x82, 0x82, 0x82, 0x7C, // 48
    0x08, 0x04, 0xFE, // 49
    0x84, 0xC2, 0xA2, 0x92, 0x8C, // 50
    0x44, 0x82, 0x92, 0x92, 0x6C, // 51
    0x30, 0x28, 0x24, 0x22, 0xFE, 0x20, // 52
    0x4C, 0x8A, 0x8A, 0x8A, 0x72, // 53
    0x7C, 0x92, 0x92, 0x92, 0x64, // 54
    0x02, 0xC2, 0x32, 0x0A, 0x06, // 55
    0x6C, 0x92, 0x92, 0x92, 0x6C, // 56
    0x4C, 0x92, 0x92, 0x92, 0x7C, // 57
    0x66, 0x66, // 58
    0xA6, 0x66, // 59
    0x08, 0x08, 0x14, 0x14, 0x22, 0x22, 0x41, // 60
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, // 61
    0x41, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, // 62
    0x0C, 0x02, 0xA2, 0x12, 0x0C, // 63
    0x38, 0x44, 0x82, 0x99, 0x99, 0xA1, 0x22, 0x1C, // 64    
    0x80, 0x70, 0x2C, 0x22, 0x2C, 0x70, 0x80, // 65
    0xFE, 0x92, 0x92, 0x92, 0x7C, // 66
    0x7C, 0x82, 0x82, 0x82, 0x44, // 67
    0xFE, 0x82, 0x82, 0x44, 0x38, // 68
    0xFE, 0x92, 0x92, 0x92, 0x92, // 69
    0xFE, 0x12, 0x12, 0x02, // 70
    0x38, 0x44, 0x82, 0x92, 0x54, 0x30, // 71
    0xFE, 0x10, 0x10, 0x10, 0xFE, // 72
    0xFE, // 73
    0xC0, 0x80, 0x80, 0x7E, // 74
    0xFE, 0x20, 0x10, 0x28, 0x44, 0x82, // 75
    0xFE, 0x80, 0x80, 0x80, 0x80, // 76
    0xFE, 0x0C, 0x70, 0x80, 0x70, 0x0C, 0xFE, // 77
    0xFE, 0x0C, 0x10, 0x60, 0xFE, // 78
    0x7C, 0x82, 0x82, 0x82, 0x82, 0x7C, // 79
    0xFE, 0x12, 0x12, 0x12, 0x0C, // 80
    0x7C, 0x82, 0x82, 0xC2, 0xC2, 0xBC, // 81
    0xFE, 0x12, 0x12, 0x32, 0xCC, // 82
    0x4C, 0x92, 0x92, 0x92, 0x64, // 83
    0x02, 0x02, 0xFE, 0x02, 0x02, // 84
    0x7E, 0x80, 0x80, 0x80, 0x7E, // 85
    0x02, 0x1C, 0x60, 0x80, 0x60, 0x1C, 0x02, // 86
    0x7E, 0x80, 0x80, 0x70, 0x80, 0x80, 0x7E, // 87
    0x80, 0x42, 0x24, 0x18, 0x24, 0x42, 0x80, // 88
    0x02, 0x04, 0x08, 0xF0, 0x08, 0x04, 0x02, // 89
    0xC2, 0xA2, 0x92, 0x8A, 0x86, 0x82, // 90
    0x7F, 0x41, 0x41, // 91
    0x03, 0x1C, 0x60, // 92
    0x41, 0x41, 0x7F, // 93
    0x08, 0x06, 0x01, 0x06, 0x08, // 94
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 95
    0x04, 0x03, // 96
    0x60, 0x54, 0x54, 0x7C, // 97
    0x7F, 0x48, 0x48, 0x30, // 98
    0x38, 0x44, 0x44, 0x28, // 99
    0x30, 0x48, 0x48, 0x7F, // 100
    0x38, 0x54, 0x54, 0x58, // 101
    0x08, 0x7E, 0x09, // 102
    0x4C, 0x92, 0x92, 0x7E, // 103
    0x7F, 0x08, 0x08, 0x70, // 104
    0x7D, // 105
    0x20, 0x40, 0x3D, // 106
    0x7F, 0x10, 0x38, 0x44, // 107
    0x7F, // 108
    0x7E, 0x02, 0x02, 0x7E, 0x02, 0x02, 0x7C, // 109
    0x7E, 0x02, 0x02, 0x7C, // 110
    0x3C, 0x42, 0x42, 0x3C, // 111
    0xFE, 0x22, 0x22, 0x1C, // 112
    0x1C, 0x22, 0x22, 0xFE, // 113
    0x7C, 0x08, 0x04, // 114
    0x48, 0x54, 0x54, 0x24, // 115
    0x08, 0x7F, 0x48, // 116
    0x3C, 0x40, 0x40, 0x7C, // 117
    0x02, 0x1C, 0x20, 0x1C, 0x02, // 118
    0x1E, 0x20, 0x1C, 0x02, 0x1C, 0x20, 0x1E, // 119
    0x22, 0x14, 0x08, 0x14, 0x22, // 120
    0x02, 0x9C, 0x60, 0x1C, 0x02, // 121
    0x44, 0x64, 0x54, 0x4C, 0x44, // 122
    0x08, 0x36, 0x41, // 123
    0xFF, // 124
    0x41, 0x36, 0x08 // 125    
    };
BYTE const numbers[]={0x3F,0x30,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
int1 nd;
unsigned int8 pos1;
//concat two 8bit int and gives a 16 bit int
//helper function
unsigned int16 concat(unsigned int8 d1,unsigned int8 d2)
{
   unsigned int16 temp;
   temp=0x0000;
   temp=d1;
   shift_left(&temp,2,0);
   shift_left(&temp,2,0);
   shift_left(&temp,2,0);
   shift_left(&temp,2,0);
   shift_left(&temp,2,0);
   shift_left(&temp,2,0);
   shift_left(&temp,2,0);
   shift_left(&temp,2,0);
   //temp=temp&0x0111;
   temp=temp|d2;
   return temp;
}
//since the segment sequence of MAX7219 is
//D7 D6 D5 D4 D3 D2 D1 D0
//DP  A  B  C  D  E  F  G   while according to convention it is
//DP  G  F  E  D  C  B  A
//One may avoide this my doing the proper changes in the circuit wiring but
//it will again lead to more complication because in CODE B decode mode the
//segment are just opposit to what u see in no decode mode. so in any case
//one need to use this helper function if he wants to use the chip in both
//no decode and decode modes.
unsigned int8 reverse(unsigned int8 nd_data)
{
   int1 D[8];
   unsigned int8 i;
   unsigned int8 temp;
   for(i=0;i<8;i++)
   {
      if(shift_left(&nd_data, 1, 0))
         D[i]=1;
      else
         D[i]=0;
   }
   temp=D[1];
   D[1]=D[7];
   D[7]=temp;

   temp=D[2];
   D[2]=D[6];
   D[6]=temp;

   temp=D[3];
   D[3]=D[5];
   D[5]=temp;

   temp=0x00;
   for(i=0;i<8;i++)
      shift_left(&temp,1,D[i]);

   return temp;
}
//Quite self explanatory
//it displays a data at the address given
//1 -> 1st digit
//8 -> 8th digit
//and dis_data is the data that is to be displayed there
void display(char adresse,char dis_data)
{
 char x;
 unsigned int16 dout;
 SSPEN=0;
 if(nd==0)
   dis_data=reverse(dis_data);
 dout=concat(adresse,dis_data);
 output_low(LOAD);

 for (x=16;x>0;x--)
 {
  if (bit_test(dout,x-1)) output_high(DIN);
  else output_low(DIN);
  output_high(CLK);
  delay_cycles(clk_delay);
  output_low(CLK);
  delay_cycles(clk_delay);
 }
 SSPEN=1;
 output_high(LOAD);
}
//another variant of the display function if you use the hardware SPI
/*void display(char adresse,char dis_data)
{
output_low(LOAD);
spi_write(adresse);
delay_us(25);
spi_write(dis_data);
output_high(LOAD);
}*/
//use this function to continously put data in the digits
//without bothering about incremanting the address
//the address increment is done automatically
void seg_putc(unsigned int8 c)
{
   display(pos1,c);
   if(pos1>=8)
      pos1=1;
   else
      pos1++;
}

//Takes an array of 8 datas and put them on the digits
//the dp decides the position of the decimal point
//1 -> decimal point on 1st digit
//8 -> decimal point on 8th digit
void display_digits(unsigned int8 arr[],unsigned int8 dp)
{
      unsigned int8 i;
      //int decimal;
      if(dp>8 || dp<1)
         dp=0;
      for(i=1;i<=8;i++)
      {
         if(dp==i)
            display(i,arr[i-1]+0x80);
         else
            display(i,arr[i-1]+0x00);
      }
      pos1=1;
}
//Takes a character as argument and displays it on a 8x8
//Dot matrix display. Works only in mode 0 i.e. No decode
void display_char(char ch)
{
   int char_offset=0;
   int8 char_width=0;
   int8 char_data[8]={0,0,0,0,0,0,0,0};
   int8 j=0;
   unsigned int16 i=0;
   unsigned int16 sum=0;
   if(ch>=FIRST_CHAR && ch<=LAST_CHAR)
   {
      char_offset=ch-FIRST_CHAR;
      char_width=font_width[char_offset];
      for(i=0;i<char_offset;i++)
      {
        sum=sum+font_width[i];
        //printf("%d+",font_width[i]);
      }
      //printf("\r\n\r\n%ld\r\n",sum);
      j=((8-char_width)/2);
      for(i=sum;i<sum+char_width;i++)
      {
         char_data[j]=font[i];
         //printf("(%d, %X), "i,font[i]);
         j++;
      }
      //display_digits(char_data,0);
      for(i=8;i>=1;i--)
      {
         //printf("%X, ",char_data[i-1]);
         display(9-i,char_data[i-1]);
      }        
   }
}
//Sets the text for scrolling. Call it to change the
//scrolling text.
void scroll_text(unsigned char* str)
{
   int len=0;
   len=strlen(str);
   scroll_text_len=len;
   scroll_chars=malloc(len+1);
   strcpy(scroll_chars,str);
}
//keep on calling this function repetedly to scroll text
void start_scroll()
{
   int char_offset=0;
   int8 char_width=0;
   int1 interspace=0;
   unsigned int16 i=0;
   unsigned int16 sum=0;
   char ch=0;
   ch=scroll_chars[cur_scroll_position];
   if(ch>=FIRST_CHAR && ch<=LAST_CHAR)
   {
      if(cur_letter_position==cur_letter_end_position)
      {

        if(temp_char_spacing<char_spacing)
         {
               interspace=1;
               temp_char_spacing++;
         }
         else
         {
               interspace=0;
               temp_char_spacing=0;
               char_offset=ch-FIRST_CHAR;
               char_width=font_width[char_offset];
               for(i=0;i<char_offset;i++)
                 sum=sum+font_width[i];
               cur_letter_position=sum;
               cur_letter_end_position=cur_letter_position+char_width;               
               cur_scroll_position++;
               if(cur_scroll_position>=scroll_text_len)
                  cur_scroll_position=0;            
         }
      }
      for(i=7; i>0; i--)
          cur_char_data[i] = cur_char_data[i-1];      
      if(interspace==0)
      {
         cur_char_data[0]=font[cur_letter_end_position-1];
         cur_letter_end_position--;
      }
      else
      {
         cur_char_data[0]=0x00;;
      }
      for(i=1;i<=8;i++)
      {
         //printf("%X, ",cur_char_data[i-1]);
         display(9-i,cur_char_data[i-1]);
      }  
      //printf("\r\n");
   }
}
void put_number(unsigned char* str,int1 align=0,int1 pad_zeros=0)
{
   unsigned int8 len=0;
   int8 i=0,j=0;
   unsigned int8 char_data[8]={0,0,0,0,0,0,0,0};
   len=strlen(str);
   if(align==0)
   {
      for(i=0;i<len;i++)
      {
         if(str[i]=='.')
         {
            char_data[j-1]+=0x80;
         }
         else
         {
            if(str[i]>=48 && str[i]<=57)
            {
               char_data[j]=numbers[(str[i]-48)];
               j++;
            }
         }
      }
   }
   else
   {
      j=7;
      //printf("%d",i);
      for(i=len-1;;i--)
      {
         //printf("%d\r\n",i);
         if(i==-1)  break;
         if(str[i]=='.')
         {
            char_data[j]=numbers[(str[i-1]-48)]+0x80;
            i--;
            j--;
         }
         else
         {
            if(str[i]>=48 && str[i]<=57)
            {
               char_data[j]=numbers[(str[i]-48)];
               j--;
            }
         }
      }   
      if(pad_zeros)
      {
         if(j!=-1)
         for(i=0;i<=j;i++)
            char_data[i]=0x3F;
      }
   }
   display_digits(char_data,0);
}
//intialize the display driver chip
//it takes the argument decode that decides whether the digits are
//decoded using CODE B scheme or they are not decoded at all
//0 -> No decode
//1 -> Decode
void display_init(int1 decode)
{
 output_low(CLK);
 delay_us(1);
 output_high(LOAD);
 nd=1;
 display(0x0B,0x07);  // scan limit - 8 columnas
 display(0x0C,0x01);  // Shutdown - normal mode
 if(!decode)
   display(0x09,0x00);  // decode mode = 0
 else
   display(0x09,0xFF);  // decode mode = 1
 display(0x0A,0x07);  // intensity
 nd=decode;
 pos1=1;
 //display(0x0F,0x00);  // test
}

