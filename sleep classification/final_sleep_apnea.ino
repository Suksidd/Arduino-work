const int sensorPin = A0;  // Analog input pin that senses Vout
int sensorValue = 0;       // sensorPin default value
float Vin = 5;             // Input voltage
float Vout = 0;            // Vout default value
float Rref = 50000000;          // Reference resistor's value in ohms (you can give this value in kiloohms or megaohms - the resistance of the tested resistor will be given in the same units)
float R = 0;               // Tested resistors default value
float average = 0;
int alert = 0;
int j = 1;
void setup ()
{

  Serial.begin(9600);      // Initialize serial communications at 9600 bps
}

void loop ()
{
  sensorValue = analogRead(sensorPin);  // Read Vout on analog input pin A0 (Arduino can sense from 0-1023, 1023 is 5V)
  Vout = (Vin * sensorValue) / 1023;    // Convert Vout to volts
  R = Rref * (1 / ((Vin / Vout) - 1));  // Formula to calculate unknown resistor's value
  //Serial.print("R: ");
  // Serial.println(R);   // Give calculated resistance in Serial Monitor
  if (j) {
    for (int i = 0; i < 5; i++) {
      sensorValue = analogRead(sensorPin);  // Read Vout on analog input pin A0 (Arduino can sense from 0-1023, 1023 is 5V)
      Vout = (Vin * sensorValue) / 1023;    // Convert Vout to volts
      R = Rref * (1 / ((Vin / Vout) - 1));  // Formula to calculate unknown resistor's value
      average = average + R;
      delay(900);
    }
  }
  j = 0;
  average = average / 5;
  float array1[300];
  float change = 0;
  for (int i = 0; i < 300; i++)
  {
    Vout = (Vin * sensorValue) / 1023;    // Convert Vout to volts
    R = Rref * (1 / ((Vin / Vout) - 1));  // Formula to calculate unknown resistor's value
    array1[i] = R;
    if (i == 0)
      change = ((array1[0] - average) / average) * 100;
    else
      change = ((array1[i] - array1[i - 1]) / array1[i - 1]) * 100;

    if (change >= 30)
    {
      alert = alert + 1;
      Serial.print("NORMAL");
    }
    else
    {
      Serial.print ("ABNORMAL");
      alert = 0;
    }

    if (alert > 2)
      Serial.print ("ALERT");

  }
  delay(2000);
}
