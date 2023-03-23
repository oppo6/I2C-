#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define LCD_DATA PORTA//LCD data port
#define ctrl PORTB
#define en PB2//enable signal
#define rw PB1// read/write signal
#define rs PB0//register select signal


void i2c_ini();
void i2c_start();
void i2c_wr(unsigned char); 
unsigned char i2c_re(unsigned char);
void i2c_stop();

void rtc_ini();
void rtc_set_t(unsigned char,unsigned char,unsigned char); 
void rtc_get_t(unsigned char *,unsigned char *,unsigned char *);


void cmd(unsigned char x);
void lcd_display(unsigned char x); 
void lcd_ini();
void lcd_str(char *str);
void lcd_pos(int line,int pos);

int main()
{
	unsigned char i,j,k;
	DDRB=0XFF;
	lcd_ini();
	lcd_str("TIME: ");
	
	rtc_ini();
	rtc_set_t(0x23,0x16,0x30);

	while(1)
	{  cmd(0x87);
		rtc_get_t(&i,&j,&k);
		lcd_display('0'+ (i>>4));
		lcd_display('0'+ (i & 0x0f));
		lcd_display(':');
		lcd_display('0'+(j>>4));
		lcd_display('0'+(j & 0x0f));
		lcd_display(':');
		lcd_display('0'+(k>>4));
		lcd_display('0'+(k & 0x0f));
		_delay_ms(500);
		
		}
	return 0;
}


void i2c_ini()
{
	TWSR=0X00;
	TWBR=0X47;
	TWCR=0X04;
}
void i2c_start()
{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while((TWCR	&(1<<TWINT))==0);
}
void i2c_wr(unsigned char x)
{
	TWDR=x;
	TWCR=(1<<TWINT)|(1<<TWEN);
	while((TWCR &(1<<TWINT))==0);
}
unsigned char i2c_re(unsigned char x)
{
	TWCR=(1<<TWINT)|(1<<TWEN)|(x<<TWEA);
	while((TWCR	&(1<<TWINT))==0);
	return TWDR;
}
void i2c_stop()
{
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	for(int i=0;i<200;i++);
}
void rtc_ini()
{
	i2c_ini();
	i2c_start();
	i2c_wr(0xd0); // address DS1307 for write
	i2c_wr(0x07); //set register pointer to 7 
	i2c_wr(0x00); //set value of location 7 to 	0 
	i2c_stop(); // transmit stop condition
}
void rtc_set_t(unsigned char h,unsigned char m,unsigned char s)
{
	i2c_start();
	i2c_wr(0xd0); // address DS1307 for write 
	i2c_wr(0); //set register pointer to 0 
	i2c_wr(s);
	i2c_wr(m);
	i2c_wr(h);
	i2c_stop();
}

void rtc_get_t(unsigned char *h,unsigned char *m,unsigned char *s)
{
	i2c_start();
	i2c_wr(0xd0); // address DS1307 for write 
	i2c_wr(0);
	//set register pointer to 0 
	i2c_stop();
	i2c_start();
	i2c_wr(0xd1); // address DS1307 for read
	*s=i2c_re(1);//read sec ,read ack
	*m=i2c_re(1); //read min ,read ack
	*h=i2c_re(0);//read hour ,read nack 
	i2c_stop();
}

void cmd(unsigned char x)
{
	
	LCD_DATA=x;
	ctrl =(0<<rs)|(0<<rw)|(1<<en);
	_delay_ms(1);
	ctrl =(0<<rs)|(0<<rw)|(0<<en);
	_delay_ms(1);
	return;
}
void lcd_display(unsigned char x)
{
	LCD_DATA= x;
	ctrl = (1<<rs)|(0<<rw)|(1<<en);
	_delay_ms(1);
	ctrl = (1<<rs)|(0<<rw)|(0<<en);
	_delay_ms(1);
	return;
}
void lcd_ini()
{
	cmd(0x38);
	_delay_ms(1);
	cmd(0x01);
	_delay_ms(1);
	cmd(0x0c);
	_delay_ms(1);
	cmd(0x80);
	return;
}
void lcd_str(char *str)
{

	int i=0;
	while(str[i]!='\0')// loop will go on till the NULL character in the string
	{
		lcd_display(str[i]);//sending data on LCD byte by byte
		i++;
	}
	return;
}