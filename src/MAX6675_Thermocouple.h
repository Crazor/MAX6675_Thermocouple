/**
	MAX6675_Thermocouple.h - The interface describes a set of methods
	for working with a thermocouple based on the MAX6675 driver
	and reading a temperature in Celsius, Fahrenheit and Kelvin.

	Instantiation:
		MAX6675_Thermocouple thermocouple(SCK_pin, CS_pin, SO_pin);
		or
		MAX6675_Thermocouple thermocouple(
			SCK_pin, CS_pin, SO_pin,
			WINDOW_SIZE, DELAY_TIME
		);

		Where,
		WINDOW_SIZE - Window size of the moving average to apply to
		the data points. Set to 1 to disable averaging.

		DELAY_TIME - Delay time between a temperature readings
		from the temperature sensor (ms). A value of 250ms seems to be
		the minimum before the chip appears to "lock up", i.e. return
		the same value each time. Supposedly, the MAX6675 only takes
		readings when not busy transmitting values.

	Update:
		Call update() from the main loop. A value is only read after
		specified delay has passed.

	Read temperature:
		double celsius = thermocouple.readCelsius();
		double kelvin = thermocouple.readKelvin();
		double fahrenheit = thermocouple.readFahrenheit();

		All methods return NaN until WINDOW_SIZE readings have been taken.

	v 2.0:
	- Optimized moving average code
	- Refactored so that update() can be called from the main loop
	v.1.1.2:
	- optimized calls of private methods.

	https://github.com/YuriiSalimov/MAX6675_Thermocouple

	Created by Yurii Salimov, February, 2018.
	Released into the public domain.

	Updates by Crazor, November 2018
*/
#ifndef MAX6675_THERMOCOUPLE_H
#define MAX6675_THERMOCOUPLE_H

#if defined(ARDUINO) && (ARDUINO >= 100)
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include <deque>

#define MAX6675_DEFAULT_READINGS_NUMBER	5
#define MAX6675_DEFAULT_DELAY_TIME	250

class MAX6675_Thermocouple final {

	private:
		int SCK_pin;
		int CS_pin;
		int SO_pin;

		volatile unsigned int readingsNumber;
		volatile unsigned long delayTime;

		std::deque<double> *dataPoints;
		double meanTempC;

	public:
		/**
			Constructor.
			@param SCK_pin - SCK digital port number.
			@param CS_pin - CS digital port number.
			@param SO_pin - SO digital port number.
		*/
		MAX6675_Thermocouple(
			const int SCK_pin,
			const int CS_pin,
			const int SO_pin
		);

		/**
			Constructor.
			@param SCK_pin - SCK digital port number.
			@param CS_pin - CS digital port number.
			@param SO_pin - SO digital port number.
			@param readingsNumber - how many readings are
				taken to determine a mean temperature.
			@param delayTime - delay time between
				a temperature readings (ms).
		*/
		MAX6675_Thermocouple(
			const int SCK_pin,
			const int CS_pin,
			const int SO_pin,
			const unsigned int readingsNumber,
			const unsigned long delayTime
		);

		/** 
		    Reads current value from sensor observing the delay and calculates mean temperature.
			Call from main loop.
		 */
		void update();

		/**
			Reads and returns a temperature in Celsius
			from the thermocouple.
		*/
		double readCelsius();

		/**
			Returns a temperature in Kelvin.
		*/
		double readKelvin();

		/**
			Returns a temperature in Fahrenheit.
		*/
		double readFahrenheit();

		/**
			Returns a temperature in Fahrenheit.
			(For older devices.)
		*/
		double readFarenheit();

		void setReadingsNumber(const unsigned int newReadingsNumber);

		void setDelayTime(const unsigned long newDelayTime);

	private:
		/**
			Initialization of module.
		*/
		inline void init();

		/**
		 	Reads the current value from the chip
		 */
		inline double readValue();

		/**
			Calculates a temperature in Celsius.
			@return temperature in Celsius.
		*/
		inline void recalculate();

		/**
			Celsius to Kelvin conversion:
			K = C + 273.15
		*/
		inline double celsiusToKelvin(const double celsius);

		/**
			Celsius to Fahrenheit conversion:
			F = C * 9 / 5 + 32
		*/
		inline double celsiusToFahrenheit(const double celsius);

		byte spiread();

		template <typename A, typename B> A validate(const A data, const B min);
};

#endif
