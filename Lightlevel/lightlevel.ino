// -----------------
// Read temperature
// -----------------

// Create a variable that will store the temperature value
double temperature = 0.0;

void setup()
{
// Register a Spark variable here
Spark.variable("temperature", &temperature, DOUBLE);

// Connect the temperature sensor to A7 and configure it
// to be an input
pinMode(A6, INPUT);
Serial.begin(9600);
}

void loop()
{
int reading = 0;
//double voltage = 0.0;

// Keep reading the sensor value so when we make an API
// call to read its value, we have the latest one
reading = analogRead(A6);

// The returned value from the Core is going to be in the range from 0 to 4095
// Calculate the voltage from the sensor reading
temperature = reading;
//voltage = (reading * 3.3) / 4095 ;
// Calculate the temperature and update our static variable
// temperature = (2103 - voltage*1000)/10.9;
Serial.println(temperature);
}
