#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps, gps2; //Fecha y Hora
SoftwareSerial ss(4, 3); //Arduino:4(rx) y 3(tx). GPS:cable azul Tx cable verde Rx
float flat, flon; //Latitud y Longitud
int pinLED = 13;
int pinActivarGPRS = 12;  // pin para activar el módulo GPRS
int area_segura= 10; // radio area segura 
int tolerancia =0; //metros despues del area segura
int metros_fuera; //metros fuera del rango
int hora_SMS, minuto_SMS, dia_SMS, mes_SMS, anio_SMS; //declaraciones de tiempo

//no es necesario...
static void smartdelay(unsigned long ms);
static void print_flat(float val, float invalid, int len, int prec);
static void print_flong(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_distancia(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_date2(TinyGPS &gps2);
static void print_str(const char *str, int len);

void setup()
{
  Serial.begin(19200); //Monitor Serial (velocidad GPRS)
  Serial.println();
  Serial.println("Iniciando Prueba");
  Serial.println();
  ss.begin(9600); //Velocidad GPS
  
  Serial.println("GPRS Activando");
  pinMode(pinLED, OUTPUT);
  pinMode(pinActivarGPRS, OUTPUT);
  digitalWrite(pinActivarGPRS, HIGH);
  delay(2000);
  digitalWrite(pinActivarGPRS, LOW);
  delay(10000);
  Serial.println("GPRS Activado");
  delay(1000);
  
}

void loop()
{
  //delay(10000);
  smartdelay(5000);
  //Volvemos a inicializar los datos para que aparescan
  float flat, flon; //latitud y longitud
  unsigned long age, date, time, chars = 0; 
  unsigned short sentences = 0, failed = 0;
  static const double RANGO_LAT = -33.036636, RANGO_LON = -71.485702; //Rango predeterminado (Universidad Tecnica Federico Santa Maria JMC)
  
  //Datos en el monitor Serial
  Serial.print("Satelites:");
  print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
  gps.f_get_position(&flat, &flon, &age); //datos para que aparescan latitud y longitud
  Serial.print(" Latitud:");
  print_flat(flat,TinyGPS::GPS_INVALID_F_ANGLE, 10, 6); //Latitud
  Serial.print(" Longitud:");
  print_flon(flon,TinyGPS::GPS_INVALID_F_ANGLE, 11, 6); //Longitud
  Serial.print(" Fecha:");
  print_date(gps); //Fecha
  Serial.print(" Hora:");
  print_date2(gps2); //Hora
  Serial.print(" Distancia de la zona:");
  print_distancia(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long)TinyGPS::distance_between(flat, flon, RANGO_LAT, RANGO_LON), 0xFFFFFFFF, 9); //Distancia
  
  Serial.println();
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void print_flat(float val, float invalid, int len, int prec) //Latitud
{
  if (val == invalid)// si no encuentra las coordenadas empieza a tirar *
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else //tira las coordenadas en este caso latitud
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}


static void print_flon(float val, float invalid, int len, int prec) //Longitud
{
  if (val == invalid) //si no encuentra coordenadas muestra *
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else // tira las coordenadas de longitud
  {
    Serial.print(val, prec);
    
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
  //  for (int i=flen; i<len; ++i)
    //  Serial.print(' ');
  }
  smartdelay(0);
}
    
static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char ab[32];
  if (val == invalid)
    strcpy(ab, "*******");
  else
    sprintf(ab, "%ld", val);
  ab[len] = 0;
  for (int i=strlen(ab); i<len; ++i)
    ab[i] = ' ';
  if (len > 0) 
    ab[len-1] = ' ';
  Serial.print(ab);
  smartdelay(0);
}

static void print_distancia(unsigned long val, unsigned long invalid, int len) //Decaramos val que sera el valor de distancia
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
   
  else
    sprintf(sz, "   %ld", val);
  Serial.print(sz);
  
  if (val == invalid) //al no encontrar coodenadas dira el siguiente texto
  Serial.print(" GPS no encuentra coordenadas ");
  
  else if (val > area_segura + tolerancia && val != invalid) //si el area segura mas la tolerancia es mayor que val tirara Fuera de rango
  {
  metros_fuera = val - area_segura; //Datos SMS
  Serial.println("   Fuera de rango ");
  loopGPRS(); //llama a loopGPRS para enviar el mensaje
  }
  
  else //si la tolerancia mas el area segura es menor que val tirara Dentro del rango
  Serial.print("   Dentro del rango ");
  smartdelay(0);
}

static void print_date(TinyGPS &gps) //Fecha (monitor Serial y SMS)
{
  //se inicializan los datos
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ");
  else
  {
    char sz[32]; //mes, dia , año
    sprintf(sz, " %02d/%02d/%02d ",
        month, day, year);
    Serial.print(sz);
  }  
}

static void print_date2(TinyGPS &gps2) //Horario (monitor Serial y SMS)
{
  //inicializan los datos
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("******** ");
  else
  {
    char ch[32]; //hora, minuto , segundo
    sprintf(ch, " %02d:%02d:%02d ",
        hour-3, minute, second);
        Serial.print(ch);
  }  
}

void loopGPRS() //Proceso del GPRS SMS
{
  //Latitud y Longitud se inicializan para que se visualicen en el mensaje
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  gps.f_get_position(&flat, &flon, &age);
  
  Serial.println("Empezando el envio del mensaje");
  delay(5000);

  Serial.println("AT");//Activa modo comando
  delay(1500);
  Serial.println("ATE0");
  delay(1500);
  Serial.println("AT+CMGF=1"); //activamos el modo SMS*/
  delay(1500);
  Serial.print("AT+CMGS="); // comando AT para enviar SMS
  Serial.write(34);   // enviar caracter "
  Serial.print("945045128"); // numero de movil receptor del SMS
  Serial.write(34);  //  enviar caracter "
  Serial.println();
  delay(2000); 
  
  //Texto del SMS 
  Serial.print("Su animal salio de la zona segura el ");
  print_date(gps);
  Serial.print(" a las ");
  print_date2(gps2);
  Serial.print(" ubicacion actual: ");
  Serial.print("Latitud ");
  print_flat(flat,TinyGPS::GPS_INVALID_F_ANGLE, 10, 6); //Latitud
  Serial.print(" con Longitud ");
  print_flon(flon,TinyGPS::GPS_INVALID_F_ANGLE, 11, 6); //Longitud 
  Serial.print(" a ");
  Serial.print(metros_fuera);
  Serial.print(" metros de la zona segura. ");
  
  // Enlace mapa google
  Serial.print("https://www.google.cl/maps/place/");
  print_flat(flat,TinyGPS::GPS_INVALID_F_ANGLE, 10, 6); //Latitud
  Serial.print(",");
  print_flon(flon,TinyGPS::GPS_INVALID_F_ANGLE, 11, 6); //Longitud 
  delay(5000);
  Serial.write(0x1A); // final del SMS, comando 1A (hex)
  Serial.println();
  delay(20000);
  Serial.println("Mensaje enviado");  
  delay(5000);
  //Desactivamos el GPRS  
//  pinMode(pinLED, OUTPUT);
//  pinMode(pinActivarGPRS, OUTPUT);
//  digitalWrite(pinActivarGPRS, HIGH);
//  delay(2000);
//  digitalWrite(pinActivarGPRS, LOW);
//  Serial.println("GPRS desactivado");
  delay(60000);
}
