/**
	The class implements a set of methods of the MAX6675_Thermocouple.h
	interface for working with a thermocouple based on the MAX6675
	driver and reading a temperature in Celsius, Fahrenheit and Kelvin.

	https://github.com/YuriiSalimov/MAX6675_Thermocouple

	Created by Yurii Salimov, February, 2018.
	Released into the public domain.

	Updates by Crazor, November 2018
*/
#include "MAX6675_Thermocouple.h"

MAX6675_Thermocouple::MAX6675_Thermocouple(
	const int SCK_pin,
	const int CS_pin,
	const int SO_pin
) : MAX6675_Thermocouple(
		SCK_pin, CS_pin, SO_pin,
		MAX6675_DEFAULT_READINGS_NUMBER,
		MAX6675_DEFAULT_DELAY_TIME
) {
}

MAX6675_Thermocouple::MAX6675_Thermocouple(
	const int SCK_pin,
	const int CS_pin,
	const int SO_pin,
	const unsigned int readingsNumber,
	const unsigned long delayTime
) {
	this->SCK_pin = SCK_pin;
	this->CS_pin = CS_pin;
	this->SO_pin = SO_pin;
	setReadingsNumber(readingsNumber);
	this->dataPoints = new std::deque<double>();
	setDelayTime(delayTime);
	init();
}

inline void MAX6675_Thermocouple::init() {
	pinMode(this->SCK_pin, OUTPUT);
	pinMode(this->CS_pin, OUTPUT);
	pinMode(this->SO_pin, INPUT);
	digitalWrite(this->CS_pin, HIGH);
}

void MAX6675_Thermocouple::update() {
	static unsigned long lastUpdate = 0;
	auto now = millis();
	if ((now - lastUpdate) > this->delayTime) {
		lastUpdate = now;
		auto value = this->readValue();
		if (value != NAN) {
			if (this->dataPoints->size() >= this->readingsNumber) {
				this->dataPoints->pop_front();
			}
			this->dataPoints->push_back(value / this->readingsNumber);
			this->recalculate();
		}
	}
}

/**
	Reads a temperature from the thermocouple.
	Takes a readings number samples in a row,
	with a slight delay time.
	@return average temperature in Celsius.
*/
double MAX6675_Thermocouple::readCelsius() {
	return this->meanTempC;
}

void MAX6675_Thermocouple::recalculate() {
	if (this->dataPoints->size() != this->readingsNumber) {
		this->meanTempC = NAN;
		return;
	}
	double celsiusTemp = 0;
	for (auto d: *this->dataPoints) {
		celsiusTemp += d;
	}
	this->meanTempC = celsiusTemp;
}

inline double MAX6675_Thermocouple::readValue() {
	int value;
	digitalWrite(this->CS_pin, LOW);
	delay(1);
	value = spiread();
	value <<= 8;
	value |= spiread();
	digitalWrite(this->CS_pin, HIGH);
	if (value & 0x4) {
		return NAN;
	}
	value >>= 3;
	return (value * 0.25);
}

byte MAX6675_Thermocouple::spiread() {
	byte value = 0;
	for (int i = 7; i >= 0; i--) {
		digitalWrite(this->SCK_pin, LOW);
		delay(1);
		if (digitalRead(this->SO_pin)) {
			value |= (1 << i);
		}
		digitalWrite(this->SCK_pin, HIGH);
		delay(1);
	}
	return value;
}

/**
	Returns a temperature in Kelvin.
	Reads the temperature in Celsius,
	converts in Kelvin and return it.
	@return temperature in Kelvin.
*/
double MAX6675_Thermocouple::readKelvin() {
	return celsiusToKelvin(readCelsius());
}

inline double MAX6675_Thermocouple::celsiusToKelvin(const double celsius) {
	return (celsius + 273.15);
}

/**
	Returns a temperature in Fahrenheit.
	Reads a temperature in Celsius,
	converts in Fahrenheit and return it.
	@return temperature in Fahrenheit.
*/
double MAX6675_Thermocouple::readFahrenheit() {
	return celsiusToFahrenheit(readCelsius());
}

inline double MAX6675_Thermocouple::celsiusToFahrenheit(const double celsius) {
	return (celsius * 9.0 / 5.0 + 32);
}

void MAX6675_Thermocouple::setReadingsNumber(const unsigned int newReadingsNumber) {
	this->readingsNumber = validate(newReadingsNumber, MAX6675_DEFAULT_READINGS_NUMBER);
	this->dataPoints = new std::deque<double>();
}

void MAX6675_Thermocouple::setDelayTime(const unsigned long newDelayTime) {
	this->delayTime = validate(newDelayTime, MAX6675_DEFAULT_DELAY_TIME);
}

template <typename A, typename B>
A MAX6675_Thermocouple::validate(const A data, const B min) {
	return (data > 0) ? data : min;
}
