struct TSL{double t; double s; double l;};
int out = 2; // definicion de salida
int S0 = 3;
int S1 = 4;
int S2 = 5;
int S3 = 6;
int OE = 7; // salida habilitada
int LED = 8; // pulso de activacion de un led para deteccion de color(opcional)
int LT = 13; // indicador de adquisicion de color segun codificacion RGB
int sombras_nb = 6;

double sombras_val[] = {30.0, 60.0, 120.0, 180.0, 240.0, 330.0};
char* sombras_id[] = {"rojo", "amarillo", "verde", "cyan", "azul", "magenta"};
void setup() {
  TCS3210setup();//salta a configurar el sensor
  Serial.begin(115200);//configura la velocidad
  //Serial.print("\nleyendo sensor 3210\n");//imprime un mensaje
  delay(100);
}
void loop() {
  struct TSL tsl = detectorColor();//toma los datos de la funcion de los colores y lo almacena en struct
  double t = tsl.t;
  //Serial.println();
  // esta escala es aproximada al valor real. esto depende de la cantidad de luz que incida en el sensor
  if (tsl.l < 38.0) {
    Serial.println(",negro");
  }
  else if (tsl.s < 4.0 && tsl.l > 60.0) {
    Serial.println(",blanco");
  }
  else if (tsl.s < 4.2 && tsl.l > 40.0) {
    Serial.println(",gris");
  }
  else // déterminacion de la tinta
    if (t >= sombras_val[0] && t < sombras_val[sombras_nb - 1])
    {
      for (int idx = 0; idx < sombras_nb - 1; ++idx)
      {
        if (t >= sombras_val[idx] && t < sombras_val[idx + 1])
        {
          //Serial.print("tendencia: ");
          Serial.print(",");Serial.print(sombras_id[idx + 1]);
          break;
        }
      }
    } else {
      Serial.println(",rojo");
    }
  //Serial.println("\n\n");
  delay(1000);
  Serial.println();
}
// comando para detectar si hay color alfrente del sensor.
bool isPresent()
{
  //se cataloga el grado de luminosidad segun la hoja de datos
  //para determinar el grado de luminosdad o oscuridad
  double isPresentTolerance = 2;
  // número se hace grande cuando algo está en frente del sensor.
  double isPresent = colorRead(out, 0, 0) / colorRead(out, 0, 1);
  if (isPresent < isPresentTolerance) {
    Serial.println("no hay nada en frente del sensor");
    return false;
  }
  return true;
}

// retorna TSL segun color detectado
//
struct TSL detectorColor() {
  struct TSL tsl;
  double white = colorRead(out, 0, 1); //out es el pin 2 del arduino
  double red = colorRead(out, 1, 1);
  double blue = colorRead(out, 2, 1);
  double green = colorRead(out, 3, 1);
//  Serial.print("datos capturados blanco : "); Serial.println(white);
//  Serial.print("datos capturados R : "); Serial.println(red);
//  Serial.print("datos capturados G : "); Serial.println(green);
//  Serial.print("datos capturados B : "); Serial.println(blue); Serial.println();
  double r, v, b; // r,v,b € [0..1]
  double offset = 3.0 / white; // compensacion de luminosidad
  r = min(1.0, offset + (white / red));
  v = min(1.0, offset + (white / green));
  b = min(1.0, offset + (white / blue));
 // Serial.print(" Rojo : "); Serial.println( r);
 // Serial.print(" verde : "); Serial.println( v);
//  Serial.print(" azul : "); Serial.println( b);
 //Serial.print("------------------------------------------------------------");
  //Serial.println();
  // transformacion RVB -> TSL
  // r,v,b € [0..1]
  // t € [0°..360°]; s,l € [%]
  double t, s, l;
  double maxRVB = max(max(r, b), v);
  double minRVB = min(min(r, b), v);
  double delta = maxRVB - minRVB;
  double somme = maxRVB + minRVB;

  // luminancia basada en valores de colores segun software de diseño grafico
  l = (somme / 2.0);
  if (delta == 0.0) // gris
  {
    t = s = 0.0;
  } else {
    //saturacion
    if ( l < 0.5 ) s = delta / somme;
    else s = delta / ( 2.0 - delta );
    // tinte
    double del_R = ( ( ( maxRVB - r ) / 6.0 ) + ( delta / 2.0 ) ) / delta;
    double del_G = ( ( ( maxRVB - v ) / 6.0 ) + ( delta / 2.0 ) ) / delta;
    double del_B = ( ( ( maxRVB - b ) / 6.0 ) + ( delta / 2.0 ) ) / delta;
    if ( r == maxRVB ) t = del_B - del_G;
    else if ( v == maxRVB ) t = ( 1.0 / 3.0 ) + del_R - del_B;
    else if ( b == maxRVB ) t = ( 2.0 / 3.0 ) + del_G - del_R;
    if ( t < 0 ) t += 1.0;
    if ( t > 1 ) t -= 1.0;
  }
  // normalizacion
  t *= 360.0; // [0°..360°]
  s *= 100.0; // [%]
  l *= 100.0; // [%]
  tsl.t = t; tsl.s = s; tsl.l = l;
  //Serial.print(" tinte [0..360]: "); 
  Serial.print(t);
  Serial.print(",");
  //Serial.print(" saturacion [%]: "); 
  Serial.print(s);
  Serial.print(",");
  //Serial.print(" luminancia [%]: "); 
  Serial.print(l);
  
  return tsl;
}
/***
  Este método devolverá el valor medido por el sensor seleccionado. Dado que la frecuencia
  es proporcional a la intensidad de la luz del filtro de color seleccionado, el valor más pequeño es
  (Cerca de 10 con setMode (2)), además de que es la luz.
  params:
  outPin: pin de salida
  color: 0 = blanco, 1 = rojo, 2 = azul, verde = 3
  Los LEDs se encienden LEDstate.
*/
double colorRead(int outPin, int color, boolean LEDstate) {
  setMode(2); // frecuencia max 100Khz => 10us
  //sélectiona el filtro
  if (color == 0) { //clear
    digitalWrite(S3, LOW); //S3
    digitalWrite(S2, HIGH); //S2
  } else if (color == 1) { //red
    digitalWrite(S3, LOW); //S3
    digitalWrite(S2, LOW); //S2
  } else if (color == 2) { //blue
    digitalWrite(S3, HIGH); //S3
    digitalWrite(S2, LOW); //S2
  } else if (color == 3) { //green
    digitalWrite(S3, HIGH); //S3
    digitalWrite(S2, HIGH); //S2
  }
  if (LEDstate) {
    digitalWrite(LED, HIGH);

    digitalWrite(LT, HIGH);
  } else {
    digitalWrite(LED, LOW);
    digitalWrite(LT, LOW);
  }
  //tiempo para que el sensor tome su lectura y se estabilice
  int sensorDelay = 10; //ms
  // lee el pulso
  double readPulse;
  delay(sensorDelay);
  readPulse = pulseIn(outPin, LOW, 25000000);
  //si el tiempo es mayor de lo que lee pulsein regresa cero
  if (readPulse < .1) {
    readPulse = 25000000;
  }
  //descativa el sensor para ahorrar energia
  setMode(0);
  return readPulse;
}
/*** selecciona el modo
*/
void setMode(int mode) {
  if (mode == 0) {
    //power OFF
    digitalWrite(LED, LOW);
    digitalWrite(LT, LOW);
    digitalWrite(S0, LOW); //S0
    digitalWrite(S1, LOW); //S1
  } else if (mode == 1) {
    //escala 1:1
    digitalWrite(S0, HIGH); //S0
    digitalWrite(S1, HIGH); //S1
  } else if (mode == 2) {
    //escala 1:5
    digitalWrite(S0, HIGH); //S0
    digitalWrite(S1, LOW); //S1
  } else { //if(mode == 3)
    //escala 1:50
    digitalWrite(S0, LOW); //S0
    digitalWrite(S1, HIGH); //S1
  }
}
///////////////////funcion de configuracion de pines/////////////////
void TCS3210setup() {
  //configuracion de frecuencia en el setup estan los pines respectivos
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  //seleccion de color
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  //salida
  pinMode(out, INPUT);
  //habilita la salida
  pinMode(OE, OUTPUT); //habilita el modulo de luz
  // activa el led
  pinMode(LED, OUTPUT);
  // halita la deteccion de color
  pinMode(LT, OUTPUT);
}
