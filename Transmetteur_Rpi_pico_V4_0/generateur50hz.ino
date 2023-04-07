void generateur ()
{
// utilisé seulement pour simuler un signal sinusoidal
valeur = ((float(millis())/100000)*2*PI) ;  //   pour 0.5 HZ
// valeur = ((float(millis())/20)*2*PI) ;   //  pour 50 HZ
//valeur = ((float(millis())/10)*PI) ;   //  pour 50 HZ
//valeur = 3*PI/2;
//valeur = 5000;
//Serial.print (" ; val=  ");
//Serial.print(valeur);

//Serial.print (" ; sin= ");
hzsimule = int(2048+(1800*(sin(valeur))));
Iread_new = hzsimule ;
//Serial.print(hzsimule);
// Serial.println();
//delay(10);
}
