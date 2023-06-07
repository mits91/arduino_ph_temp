// Libraries
#include <Wire.h>                 //I2C communication
#include <LiquidCrystal_I2C.h>    //interfacing with I2C-based LCD displays
#include <DallasTemperature.h>    //interface with Dallas temperature sensors

// Constants
#define pH_pin A0               //the analog pin connected to the pH sensor
#define ONE_WIRE_BUS 2          //the digital pin connected to the OneWire bus for temperature sensors
#define X 1.2                   //calibrating ph sensor
#define samplingInterval 20     //collect the value every 20ms
#define printInterval 1000      //print every 1000ms
#define ArrayLenth 50           //put the collect value in the array

int pHArray[ArrayLenth];                      //Store the average value of the sensor feedback
int pHArrayIndex=0;                           //initialize the value
const LiquidCrystal_I2C lcd(0x27,16,2);       //the I2C address of the LCD (0x27), the number of columns (16), and the number of rows (2)
const OneWire oneWire(ONE_WIRE_BUS);          //he digital pin (defined earlier as 2) connected to the OneWire bus for temperature sensors
const DallasTemperature sensors(&oneWire);    //allows the sensors object to communicate with Dallas temperature sensors using the OneWire bus

void setup() {         //This function is defined with the return type void, indicating that it doesn't return a value
  Serial.begin(9600);  //communication between the Arduino board and a computer through the USB connection
  lcd.init();          // initialize the lcd 
  lcd.backlight();     // turn light of the LCD on
  lcd.begin(16, 2);    // Initialize LCD in pins 16 columns 2 rows
  sensors.begin();     // Initialize the Dallas temperature sensor
}

void loop() {                               //function is called repeatedly
  float pHVal, voltVal;                     //floating-point variables, pHVal and voltVal, are declared
  mean_average_pH(pHVal, voltVal);          //calculates the mean average pH value and updates the pHVal and voltVal variables with the result
  float temperature = readTemperature();    //calls function readTemperature returns a float value temperature The returned value is stored in variable temperature
  displayData(pHVal, temperature);          //displays the pH value and temperature on LCD
  serialData(pHVal, voltVal, temperature);  //sends the pH value, voltage value, and temperature data over the serial communication
}

void mean_average_pH(float &pHVal, float &voltVal) {             //two float variables pHVal and voltVal are passed by reference (&) to allow their values to be modified inside the function
  static unsigned long samplingTime = millis();                  //static unsigned long variables, samplingTime and printTime
  static unsigned long printTime = millis();                     //millis returns the number of milliseconds that have passed since the Arduino board started running
  float pHValue, voltage;                                        //float variables, pHValue and voltage, are declared
  do {                                                           //continuously collects pH values, calculates the mean average, and updates the pHVal and voltVal variables
    if(millis() - samplingTime > samplingInterval)               //collect the value every 20ms if enough time has passed since the last sampling executes the code block inside the curly braces.
    {
      pHArray[pHArrayIndex++] = analogRead(pH_pin);              //reads the analog value from the pin and stores in the next position of the pHArray
      if(pHArrayIndex == ArrayLenth) pHArrayIndex = 0;           //reinitialize the array when the array is full
      voltage = avergearray(pHArray, ArrayLenth) * 5.0 / 1024;   //turn analog signal to voltage
      pHValue = 3.5 * voltage + X;                               //turn voltage to pH value
      samplingTime = millis();                                   //counter of time
    }
    if(millis() - printTime > printInterval)                     //checks if enough time has passed since the last print print the value every 800ms
    {
      pHVal = pHValue;                                           //values of pHVal and voltVal updated with most recent values every time is executed the mean_average_pH function.
      voltVal = voltage;
      break;
    }
  } while(true);
}

double avergearray(int* arr, int number) {                    //defines function avergearray calculates average of array of integers type of double it returns a floating-point value two parameters: a pointer to an integer array arr, an integer  length of the array.
  int i;                                                      //defines variables
  int max,min;
  double avg;
  long amount=0;
  if (number <= 0) {                                            //error message when nothing in the array
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if (number < 5) {                                            //checks if the number parameter is less than 5
    for(i = 0; i < number; i++) { amount +=arr [i]; }          //calculates the average of the array directly by summing
    avg = amount / number;                                     //dividing the sum by the length of the array
    return avg;
  } else {
    if (arr[0] < arr[1]) { min = arr[0];max=arr[1]; }        //determines minimum and maximum values among the first two elements of the array
    else { min=arr[1];max=arr[0]; }
    for(i = 2; i < number; i++){
      if(arr[i] < min){                                     //If element arr[i] smaller than current minimum (min), updates min and adds the previous minimum value to amount.
        amount += min;                                      //collect the min
        min = arr[i];
      } else {
        if(arr[i] > max){                                   //If element arr[i] larger than the current maximum (max), updates max and adds the previous maximum value to amount.
          amount += max;                                    //collect the max
          max = arr[i];
        } else {
          amount += arr[i];                                 //sum up the index without max and min
        }
      }
    }
    avg = (double) amount / (number - 2);                   //Deduct the maximum and minimum values ​​and average
  }
  return avg;
}

void displayData(float pHValue, float temperature) {      //defines a function named displayData that displays pH value and temperature on an LCD screen
  lcd.setCursor(0, 0);                                    //sets the cursor position on the LCD screen to the first row and the first column
  lcd.print("pH   : ");                                   //prints the string "pH : " on the LCD screen
  lcd.print(pHValue, 2);                                  //prints the value of pHValue with two decimal places

  lcd.setCursor(0, 1);                                    //sets the cursor position on the LCD screen to the second row and the first column
  lcd.print("Temp : ");                                   //prints the string "Temp : " on the LCD screen
  lcd.print(temperature, 2);                              //prints the value of temperarure with two decimal places
  lcd.print(" C");                                        //prints the string " C"
}

void serialData(float pHValue, float voltage, float temperature) {    //outputs pH value, voltage, and temperature data to the serial monitor
  Serial.println("*****************");                                //outputs a line of asterisks to the serial monitor.just to be easier to read
  Serial.print("pH      : ");                                         //outputs ph from phValue  two decimal
  Serial.println(pHValue, 2);
  Serial.print("Voltage : ");                                         //outputs voltage from voltage  two decimal
  Serial.println(voltage, 2);
  Serial.print("Temp    : ");                                         //outputs temp from temperature  two decimal
  Serial.print(temperature, 2);
  Serial.println(" C");
  Serial.println("*****************\n");                              //outputs a line of asterisks to the serial monitor.just to be easier to read
}

float readpH() {                                                      // just for debugging, we dont need it
  int pH_raw = analogRead(pH_pin);
  float pH = 3.5 * pH_raw * 5.0 / 1024.0 + X;
  return pH;
}

float readTemperature() {
  sensors.requestTemperatures();                      //sends a request to the DallasTemperature sensor to measure the temperature
  float temperature = sensors.getTempCByIndex(0);     //retrieves temperature in Celsius from sensor, getTempCByIndex(0) used to get temperature of first sensor connected to the DallasTemperature library
  return temperature;
}
