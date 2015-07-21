#include "mbed.h"
#include "math.h"

const int ARRAY_LENGTH    = 500;
float temperature_celsius, temperature_voltage, light_voltage, raw_pir_reading, noise_voltage, noise_maximum, noise_average, noise_total, noise_variance, noise_sd, presence_percentage, num_consecutive_runs;
char publishString[40];
float noise_array[ARRAY_LENGTH];
int pir_array[ARRAY_LENGTH];

//bool DEBUG_MODE = false;

AnalogIn light_ain(A0);
AnalogIn temp_ain(A1);
AnalogIn pir_ain(A2);
AnalogIn noise_ain(A3);

void publish_measurements() {
	temperature_voltage = temp_ain.read_u16();
	// Convert DAC reading to millivolts
	temperature_voltage = (temperature_voltage * 3.3 * 1000) / 4095;
	// Convert millivolts to Celsius using datasheet equation
	temperature_celsius = (2103 - temperature_voltage) / 10.9;
	//if(DEBUG_MODE) {
		//Serial.println("Temperature (Celsius): " + String(temperature_celsius));
	//}

	light_voltage = light_ain.read_u16();

	sprintf(publishString,"%.1f, %.1f, %.1f, %.1f, %.1f", temperature_celsius, light_voltage, noise_maximum, noise_average, noise_variance);
	//Spark.publish("measurements", publishString);
	sprintf(publishString, "%.1f, %.1f", presence_percentage, num_consecutive_runs);
	//Spark.publish("measurements", publishString);
}


void measure_pir_and_noise() {
	noise_voltage = 0;

	for (int i = 0; i < ARRAY_LENGTH; i++) {
		raw_pir_reading = pir_ain.read_u16();
		noise_voltage = noise_ain.read_u16();
		noise_array[i] = noise_voltage;
		if (raw_pir_reading > 3000) {
			pir_array[i] = 1;
		} else {
			pir_array[i] = 0;
		}
		//if (DEBUG_MODE) {
			//Serial.println("PIR: " + String(raw_pir_reading));
			//Serial.println("Noise: " + String(noise_voltage));
		//}
		wait(0.1);
	}
}


void noise_analysis() {
	float residuals[ARRAY_LENGTH];
	noise_maximum = 0;
	noise_average = 0;
	noise_variance = 0;
	noise_total = 0;

	for (int i = 0; i < ARRAY_LENGTH; i++) {
		if (noise_array[i] > noise_maximum) noise_maximum = noise_array[i];
		noise_total += noise_array[i];
	}
	noise_average = noise_total / (ARRAY_LENGTH * 1.0);

	for (int i = 0; i < ARRAY_LENGTH; i++) {
		residuals[i] = noise_array[i] - noise_average;
		noise_variance += residuals[i] * residuals[i];
	}
	noise_variance = noise_variance / (ARRAY_LENGTH * 1.0);
}


void pir_analysis() {
	int counts_over_1s = 0;
	int longest_consecutive_run = 0;
	int curr_num_consecutive = 0;

	counts_over_1s = pir_array[0] == 1 ? 1 : 0;
	for (int i = 1; i < ARRAY_LENGTH; i++) {
		if (pir_array[i] == 1) {
			++counts_over_1s;
		}
		if (pir_array[i] == 1 && pir_array[i-1] == 1) {
			++curr_num_consecutive;
		} else {
			if (curr_num_consecutive > longest_consecutive_run) {
				longest_consecutive_run = curr_num_consecutive;
				curr_num_consecutive = 0;
			}
		}
	}

	presence_percentage = 100 * counts_over_1s * 1.0 / ARRAY_LENGTH;
	num_consecutive_runs = longest_consecutive_run;
}


int main() {
	while (1) {
	measure_pir_and_noise();
	noise_analysis();
	pir_analysis();
	publish_measurements();
}
}