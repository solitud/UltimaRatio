/* ------------------------------------------------------------ */
// Gezeiten Ultima Ratio Cascading Clock
// Knut Schade
/* ------------------------------------------------------------ */

#include <SPI.h>
#include <DAC_MCP49xx.h>
#include <AnalogEvent.h>

#define PERFDEBUG 0
#define SERIALDEBUG 0

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

/* ------------------------------------------------------------ */
// - DAC Setup
/* ------------------------------------------------------------ */

// The Arduino pin used for the slave select / chip select
#define SS_PIN 10

// Set up the DAC.
DAC_MCP49xx dac(DAC_MCP49xx::MCP4922, SS_PIN);

/* ------------------------------------------------------------ */
// - Pins
/* ------------------------------------------------------------ */

int const potModClock1Pin               = A0;
int const potModClock2Pin               = A1;
int const potEnvAttackPin               = A2;
int const potEnvReleasePin              = A3;

int const ledAlertPin                   = A4;
int const resetInputPin                 = A5;

int const ledModClock1Pin               = 2;
int const ledModClock2Pin               = 3;
int const outputModClock1Pin            = 4;
int const outputModClock2Pin            = 5;
int const ledClockInputPin              = 6;

int const clockInputPin                 = 7;

int const outputGeneratorClockPin       = 8;
int const outputMasterClockPin          = 9;
int const switchClockDividerRelationPin = 12;

/* ------------------------------------------------------------ */
// - Global Constants and Variables
/* ------------------------------------------------------------ */

int const dividers[]               = {1,2,3,4,6,8,12,16,24,32,48,64,96};

int const dacMaxVal                = 4095;
int clockDividerRelation           = HIGH;

boolean clkState[]                 = {LOW,LOW};
int envState[]                     = {0,0}; // 0=off, 1=rising, -1 = falling
int currValue[]                    = {0,0};

unsigned int riseValue             = 0;
unsigned int fallValue             = 0;

unsigned int potModClock2Val;

unsigned long generatorInterval    = 100;

unsigned int modClock1Divider      = 2;
unsigned int modClock2Divider      = 4;
unsigned int inc                   = 1;
volatile boolean trigger           = false;

unsigned long previousClockTime    = 0;
unsigned long previousSwitchTime   = 0;
unsigned long openedGateAt         = 0;
unsigned long const gateTimeMicros = 510;

boolean reset                      = 0;

#if PERFDEBUG
	unsigned long cycleQty          = 0; //for performance debug
	unsigned long previousDebugTime = 0;
#endif

/* ------------------------------------------------------------ */
// - Setup
/* ------------------------------------------------------------ */

void setup() {
	#if SERIALDEBUG
		Serial.begin(9600);
	#endif

	/* ------------------------------------------------------------ */
	// - Faster analog Read
	/* ------------------------------------------------------------ */

	// set adc prescaler to 32 for 38 kHz sampling frequency
	sbi(ADCSRA, ADPS2);
	cbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);

	ACSR =
		(0 << ACD) |    // Analog Comparator: Enabled
		(1 << ACBG) |   // Analog Comparator Bandgap Select
		(0 << ACO) |    // Analog Comparator Output: Off
		(1 << ACI) |    // Analog Comparator Interrupt Flag: Clear Pending Interrupt
		(1 << ACIE) |   // Analog Comparator Interrupt: Enabled
		(0 << ACIC) |   // Analog Comparator Input Capture: Disabled
		(1 << ACIS1) | (0 << ACIS0);   // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge

	pinMode(clockInputPin, INPUT);
	pinMode(ledClockInputPin, OUTPUT);
	pinMode(ledModClock1Pin, OUTPUT);
	pinMode(ledModClock2Pin, OUTPUT);
	pinMode(outputModClock1Pin, OUTPUT);
	pinMode(outputModClock2Pin, OUTPUT);
	pinMode(outputGeneratorClockPin, OUTPUT);
	pinMode(outputMasterClockPin, OUTPUT);
	pinMode(switchClockDividerRelationPin, INPUT);
	pinMode(resetInputPin, INPUT);
	pinMode(ledAlertPin, OUTPUT);

	//PullUp
	// digitalWrite(resetInputPin, HIGH);

	// Set the SPI frequency to 1 MHz (on 16 MHz Arduinos), to be safe.
	// DIV2 = 8 MHz works for me, though, even on a breadboard.
	// This is not strictly required, as there is a default setting.
	dac.setSPIDivider(SPI_CLOCK_DIV2);

	// Use "port writes", see the manual page. In short, if you use pin 10 for
	// SS (and pin 7 for LDAC, if used), this is much faster.
	// Also not strictly required (no setup() code is needed at all).
	dac.setPortWrite(true);

	// Pull the LDAC pin low automatically, to synchronize output
	// This is true by default, however.
	// dac.setAutomaticallyLatchDual(true);

	AnalogEvent.addAnalogPort(potModClock1Pin, handleModClock1, 20);
	AnalogEvent.addAnalogPort(potModClock2Pin, handleModClock2, 20);
	AnalogEvent.addAnalogPort(potEnvAttackPin, handleAttack, 10);
	AnalogEvent.addAnalogPort(potEnvReleasePin, handleRelease, 10);

	digitalWrite(ledAlertPin, HIGH);
}

void loop() {
	AnalogEvent.loop();

	unsigned long currentTimeMillis = millis();
	unsigned long currentTimeMicros = micros();
	// d("currentTimeMicros", currentTimeMicros);

	// Reset check
	if((PINC >> 5) & 1 == 1)  {
		if(reset == 0) {
			inc = 1;
			reset = 1;
		}
		// digitalWrite(ledAlertPin, HIGH);
	} else{
		reset = 0;
		// digitalWrite(ledAlertPin, LOW);
	}


	if(trigger == true) {

		// digitalWrite(ledModClock1Pin, LOW);//2->PD2
		// digitalWrite(ledModClock2Pin, LOW);//3->PD3
		// digitalWrite(outputModClock1Pin, LOW);//4->PD4
		// digitalWrite(outputModClock2Pin, LOW);//5->PD5
		// digitalWrite(ledClockInputPin, LOW); //6 ->PD6

		// digitalWrite(outputMasterClockPin, HIGH);//9->PB1
		PORTB |= B00000010;

		word portD = PORTD | B01000000;
		// digitalWrite(ledClockInputPin, HIGH);

		if(inc % modClock1Divider == 0) {
			clkState[0] = HIGH;
			// digitalWrite(ledModClock1Pin, HIGH);
			// digitalWrite(outputModClock1Pin, HIGH);
			portD |= B00010100;
		}

		if(inc % modClock2Divider == 0) {
			clkState[1] = HIGH;
			// digitalWrite(ledModClock2Pin, HIGH);
			// digitalWrite(outputModClock2Pin, HIGH);
			portD |= B00101000;
		}

		PORTD = portD;
		inc++;

		// trigger = false;
		// delayMicroseconds(450);
		// PORTB &= B11111101;
		// PORTD &= B10000011;

		openedGateAt = currentTimeMicros;
		trigger = false;
	}


	if((currentTimeMicros - openedGateAt) >= gateTimeMicros) {
		// digitalWrite(outputMasterClockPin, LOW);
		PORTB &= B11111101;
		PORTD &= B10000011;
	}

	if((currentTimeMillis - previousClockTime) > (generatorInterval / 192)) {
		previousClockTime = currentTimeMillis;
		//digitalWrite(outputGeneratorClockPin, !digitalRead(outputGeneratorClockPin));
		PINB = _BV(0);
	}

	if((currentTimeMillis - previousSwitchTime) > 500) {
		previousSwitchTime = currentTimeMillis;
		// boolean switchState = digitalRead(switchClockDividerRelationPin);
		boolean switchState = PINB >> 4;
		if(clockDividerRelation != switchState) {
			clockDividerRelation = switchState;
			setModClock2Divider();
		}
	}

	#if PERFDEBUG
		cycleQty++;
		if(currentTimeMillis - previousDebugTime > 500) {
			#if SERIALDEBUG
				Serial.print(" ClockDivider1: ");
				Serial.print(modClock1Divider);
				Serial.print(" ClockDivider2: ");
				Serial.print(modClock2Divider);
				Serial.print(" Cycles: ");
				Serial.println(cycleQty);
			#endif

			// if(cycleQty < 5000) {
			// 	digitalWrite(ledAlertPin, HIGH);
			// } else {
			// 	digitalWrite(ledAlertPin, LOW);
			// }

			// digitalWrite(ledAlertPin, LOW);
			PORTC &= B11101111;

			previousDebugTime = currentTimeMillis;
			cycleQty = 0;
		}
	#endif

	triggerEnvelopes();
}

void triggerEnvelopes() {
	for(int i=0; i<=1; i++) {
		boolean triggerReceived = false;
		// start envelope on trigger
		if (clkState[i] == HIGH) {
			clkState[i] = LOW;
			triggerReceived = true;
		}
		// attack phase on trigger
		if (triggerReceived) {
			triggerReceived = false;
			envState[i] = 1;
			//currValue[i] = 0; //<-Restart from Zero???
		}

		// add/substract values
		if (envState[i] == 1) {
			currValue[i] += riseValue;
		} else if (envState[i] == -1) {
			currValue[i] -= fallValue;
		}

		// decay phase
		if (currValue[i] > dacMaxVal) {
			currValue[i] = dacMaxVal;
			envState[i] = -1;
		}

		// end envelope
		if (currValue[i] < 0) {
			currValue[i] = 0;
			envState[i] = 0;
		}
	}
	dac.output2(int(currValue[0]), int(currValue[1]));
}

void handleModClock1(AnalogPortInformation* Sender) {
	// d("handleModClock1 changed", Sender->value);
	modClock1Divider = dividers[map(Sender->value, 0, 1023, 0, 12)];
	generatorInterval = map(Sender->value, 0, 1023, 5500, 800);
	setModClock2Divider();
}

void handleModClock2(AnalogPortInformation* Sender) {
	// d("handleModClock2 changed", Sender->value);
	potModClock2Val = Sender->value;
	setModClock2Divider();
}

void handleAttack(AnalogPortInformation* Sender) {
	// d("handleAttack changed", Sender->value);
	int unsigned potEnvAttackVal = map(Sender->value, 0, 1023, 1, 3000);
	riseValue = dacMaxVal / potEnvAttackVal;
}

void handleRelease(AnalogPortInformation* Sender) {
	// d("handleRelease changed", Sender->value);
	int unsigned potEnvReleaseVal = map(Sender->value, 0, 1023, 1, 4000);
	fallValue = dacMaxVal / potEnvReleaseVal;
}

void setModClock2Divider() {
	// d("clockDividerRelation changed", clockDividerRelation);
	if(clockDividerRelation == HIGH) { //reference is master
		modClock2Divider = dividers[map(potModClock2Val, 0, 1023, 0, 12)];
	} else { //reference is CD 1
		modClock2Divider = modClock1Divider * dividers[map(potModClock2Val, 0, 1023, 0, 12)];
	}
}

ISR(ANALOG_COMP_vect) {
	trigger = true;
}

// void d(const char* key, long value) {
// 	#if SERIALDEBUG
// 		Serial.print(key);
// 		Serial.print(" : ");
// 		Serial.println(value);
// 	#endif
// 	#if PERFDEBUG
// 		// digitalWrite(ledAlertPin, HIGH);
// 		PORTC |= B00010000;
// 	#endif
// }

