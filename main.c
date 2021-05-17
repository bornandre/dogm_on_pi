/* ---------------------------------------------------------------------

   Sample code for driving ST7036 on ELECTRONIC ASSEMBLY's DOG-Series
   (tested on RaspberryPi 1)
   *** NO SUPPORT ON THIS PIECE OF CODE ***
   
   --------------------------------------------------------------------- */

#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>
#include <st7036.h>

int main(void)
{
	time_t current_time;
	char* c_time_string;

	if (wiringPiSetup() == -1)
		return 1;

	pinMode (9, OUTPUT); // pin2  (CLK)
	pinMode (8, OUTPUT); // pin3  (SI)
	pinMode (7, OUTPUT); // pin7  (RS)
	pinMode (0, OUTPUT); // pin17 (CSB)

	ST7036_init();

	do
	{
		/* Obtain current time. */
		current_time = time(NULL);

		/* Convert to local time format. */
		c_time_string = ctime(&current_time);

		/* Print to stdout. ctime() has already added a terminating newline character. */
		ST7036_puts("Current tine is: %s", c_time_string);
	} while (1);

	return 0;
}
	
void ST7036_write_byte( char data )
{
	signed char	u8_zahl = 8;
	char c_data;

	// Chip-Select auf log.0
	digitalWrite (0,0);       // clear CSB
	c_data = data;

	do
	{
		delay(6);
		if ( c_data & 0x80 )		// oberstes Bit von c_data betrachten
			digitalWrite (8,1);		// und Datenleitung entsprechend setzen
		else
		 	digitalWrite (8,0);

		delay(5);			// einen Clockpuls erzeugen
		digitalWrite (9,0);			// CLK low
		delay(6);
		digitalWrite (9,1);			// CLK high

		c_data = c_data << 1;
		u8_zahl --;

	} while (u8_zahl > 0);

	// Chip-Select wieder auf log.1
	delay(2);
	digitalWrite (0,1);       // set CSB
}

/* ein Byte in das Control-Register des KS0073 senden */
void ST7036_write_command_byte( char data )
{
	digitalWrite(7,0);
	delay(1);
	ST7036_write_byte( data );
}

/* ein Byte in das Daten-Register des KS0073 senden */
void ST7036_write_data_byte( char data )
{
	digitalWrite(7,1);
	delay(7);
	ST7036_write_byte( data );
}

/* Reset durchführen */
void ST7036_reset(void)
{
#ifdef ST7036_RESET
	//digitalWrite(1,0);	// Hardware-Reset log.0 an den ST7036 anlegen
	//warte_ms( 100 );
	//digitalWrite(1,1);
#endif
}

/* ein Byte in das Daten-Register des KS0073 senden */
void ST7036_init(void)
{

	digitalWrite(9,1);
	digitalWrite(0,1);

	//ST7036_reset();
	delay(50);		// mehr als 40ms warten

	//ST7036_write_command_byte( 0x38 );	// Function set; 8 bit Datenlänge, 2 Zeilen
	delay(1);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x39 );	// Function set; 8 bit Datenlänge, 2 Zeilen, Instruction table 1
	delay(1);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x15 );	// Bias Set; BS 1/5; 3 zeiliges Display
	delay(1);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x55 );	// Booster ein; Kontrast C5, C4 setzen
	delay(1);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x6e );	// Spannungsfolger und Verstärkung setzen
	delay(1);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x72 );	// Kontrast C3, C2, C1 setzen
	delay(1);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x38 );	// Zurück zur Instruction table
	delay(500);		// mehr als 200ms warten !!!

	ST7036_write_command_byte( 0x0f );	// Display EIN, Cursor EIN, Cursor BLINKEN /0f
	delay(1);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x01 );	// Display löschen, Cursor Home
	delay(400);		//

	ST7036_write_command_byte( 0x06 );	// Cursor Auto-Increment
	delay(1);		// mehr als 26,3µs warten
}

/*
void ST7036_putsf( char * string )
{
	unsigned char zahl;
	zahl = 0;
	while (string[zahl] != 0x00)
	{
		ST7036_write_data_byte( string[zahl]);
		zahl++;
	}
}
*/

void ST7036_puts( char * string )
{
	unsigned char zahl;
	zahl = 0;
	while (string[zahl] != 0x00)
	{
		delay(1);
		ST7036_write_data_byte( string[zahl] );
		string ++;
	}
}

void ST7036_putc( char zeichen )
{
	ST7036_write_data_byte( zeichen );
}
// positioniert den Cursor auf x/y.
// 0/0 ist links oben, 2/15 ist rechts unten
void ST7036_goto_xy(unsigned char xwert, unsigned char ywert)
{
	//delay(50000);
	ST7036_write_command_byte(0x80 + ywert*0x20 + xwert);
	//delay(50000);
	digitalWrite(7,1);
}

void ST7036_clearline( unsigned char zeile )
{
	unsigned char zahl;
	ST7036_goto_xy( 0, zeile );
	for (zahl=1; zahl<20; zahl++)
		ST7036_write_data_byte( ' ' );
}

void ST7036_clear( void )
{
	ST7036_clearline( 0 );
	ST7036_clearline( 1 );
	ST7036_clearline( 2 );
}

