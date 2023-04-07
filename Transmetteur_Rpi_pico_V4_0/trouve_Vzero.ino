

void trouve_Vzero() {
  // but: trouver la valeur moyenne de tension mesurée sur ADC0
  // et trouve également la valeur mediane Izero pour l'intensité
  //  Serial.println("trouve_Vzero");
  SumVread = 0;  // vide le compteur totalisateur
  Vmin = 4096;   // pour initialiser
  Vmax = 0;      // idem
  Imin = 4096;   // idem pour I
  Imax = 0;      // idem
  nbloop2 = 0;   // compteur du nombre de boucles
  temps2 = millis() + 20;
  // on va mesurer Vmin et Vmax sur une amplitude complete ...

  while (millis() < temps2) {          // sequence pendant 20  ms, correspond a un cycle secteur 50hz
    Vread = long(analogRead(A0));        // lire la valeur'ADC 0
    if (Vread < Vmin) { Vmin = Vread; }  // tension mini
    if (Vread > Vmax) { Vmax = Vread; }  // tension mini
    Iread = analogRead(A1);        // lire la valeur'ADC 1
  if (Iread < Imin) { Imin = Iread; }    // intensité mini
    if (Iread > Imax) { Imax = Iread; }  // intensité mini

    nbloop2++;
    // on boucle pendant 20 ms
  }
  // fin while
  /*
  Vmedian = (Vmax + Vmin) / 2;  // 
  if (Vmedian > (Vzero + epsilon)) { Vzero ++;}//   augmente de une unité seulement
  if (Vmedian < (Vzero - epsilon)) { Vzero --;}//   diminue de une unité seulement
  */
  Imedian = (Imax + Imin)/2 ;   //
  if (Imedian > (Izero + epsilon)) {Izero ++;}//   augmente de une unité seulement
  if (Imedian < (Izero - epsilon)) {Izero --;}//   diminue de une unité seulement
 
 /*
#ifdef debug  // debut debug
  Serial.print("trouve_Vzero : ");
  Serial.print(" Vmin = ");
  Serial.print(Vmin);
  Serial.print(" Vmax = ");
  Serial.print(Vmax);
  Serial.print(" Vzero = ");
  Serial.print(Vmedian);
   Serial.print("  Izero = ");
  Serial.print(Izero);
  Serial.print("\n");  // retour a la ligne
#endif                 // fin debug \
*/      
}

  /*
   SumVread = 0;  // vide le compteur totalisateur
  Vmin = 4096;   // pour initialiser
  Vmax = 0;      // idem
  nbloop2 = 0;   // compteur du nombre de boucles
  temps2 = millis()+20;
  while (millis() < (temps2)) {  // sequence pendant 20  ms, correspond a un cycle secteur 50hz
    Vread = analogRead(A0);   // 
//    Serial.println (Vread) ;
    SumVread = SumVread + Vread;
    nbloop2++;
  delayMicroseconds(50)    ; // tres lent !

  }                                   // fin while
  Vzero = SumVread / nbloop2;  //

#ifdef debug  // debut debug
  Serial.print("    SumVread = ");
  Serial.print(SumVread);  //
  Serial.print(" ; nbloop2 = ");
  Serial.print(nbloop2);
  Serial.print(" Vzero = ");
  Serial.print(Vzero);  // note; c'est souvent 2065
   // if ((Vmax - Vmin) < 30) { Serial.print("   Pas de tension secteur"); }  // 30 car il reste un peu de bruit
  // if (Vzero < 100) { Serial.print("   Anomalie, pas de tension lue"); }
 
#endif                 // fin debug

 Serial.print("\n");  // retour a la ligne
*/
