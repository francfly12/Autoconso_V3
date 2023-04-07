void microloop() {
   // boucle effectuee a chaque itération de la mesure

  // lecture tension sur ADC0 et filtrage :
  Vread_new = analogRead(A0);
  Vread_new = Vread_new - Vzero;                 // on ramene autour de zéro
  Sum_Vread_inst = Sum_Vread_inst + Vread_new;  // cumul des tensions pour rechercher Vzero
  Vread_delta = (Vread_new - Vread_inst) ;   // delta - on ne peut pas diviser ici sinon on accumule les defauts de divisions successives
  Vread_inst = Vread_inst + Vread_delta;         // Vread_inst est la valeur filtrée, 
  if ((Vread_inst <5 )&&(Vread_inst > -5)) { Vread_inst = 0; };  // teste si tension trop faible, on n'a que du bruit
  Vreadsqr += Vread_inst * Vread_inst ;        // somme des carrés   

  // lecture intensite sur ADC1 et filtrage :
   Iread_new = analogRead(A1);
  // generateur () ;  // renvoie une valeur simulée dans Iread_new 
  IreadT0l = Iread_new ;  // lecture
  Iread_new = Iread_new - Izero;                 // on ramene autour de zéro
  IreadT1l = Iread_new ; //lecture
  Sum_Iread_inst +=  Iread_new ;  // cumul des intensités pour rechercher Izero
  IreadT2l = Sum_Iread_inst ; //lecture
  Iread_delta = Iread_new - Iinstant ;   // delta 
  IreadT3l = Iinstant ; //lecture
  Iinstant =  Iinstant + (Iread_delta );         // Iinstant est la valeur filtrée finale
  IreadT4l = Iinstant ; //lecture
  Iinstant = Iread_new ;      // nouvelle valeur devient l'ancienne
  if ((Iinstant > -15) && (Iinstant < 15)) { Iinstant = 0; } ; // // else {Serial.println("#");} ; // teste si intensité est trop faible, on n'a que du bruit
  // attention, eviter la fonction abs() qui donne des resultats errones quand utilisée avec des ()
  IreadT5l = Iinstant ; //lecture
  if (Iinstant >=0) {Iinstantp = Iinstant;} else {Iinstantp = - Iinstant;};  // remplace abs ()
  if (Vread_inst >=0) {Vread_instp = Vread_inst;} else {Vread_instp = - Vread_inst;};  // remplace abs ()
  
  // calcul de la quantité d'énergie
  // comme P=UI et le deltaT etant constant, on peut estimer l'énergie comme la puissance * temps

  E_inst = ((Vread_instp >> 2) * Iinstantp) /4;  // P=U*I  , puis division pour eviter l'overflow ; ici abs() car l intensité peut etre négative
                                          // E_inst et E_cycle sont des long
  E_cycle += E_inst >> 6;               // Ajout pour une microloop, division, total sur un cycle

  delayMicroseconds(delayPLL);  // delai piloté par la PLL
}
// junk code :
/*
//  Serial.print("   Vread_inst = ");
//  Serial.println (Vread_inst);
//   Serial.print(" Vread_delta = ");
//   Serial.print(Vread_delta);
 // Serial.print(" Vmin = ");
  // Serial.print(Vmin);
  // Serial.print("   Vmax = ");
  // Serial.print(Vmax);
  // Serial.print("   Vread_new = ");
  // Serial.print(Vread_new);
  // Serial.print("   Vzero = ");
  // Serial.print(Vzero);  
  // Serial.print("   Vread_inst = ");
  //Serial.print(Vread_inst);
  */