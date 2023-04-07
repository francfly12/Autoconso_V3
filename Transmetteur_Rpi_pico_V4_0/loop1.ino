void loop1() {  // boucle PLL *****************************************************
   // effectuée à chaque cycle, tous les 20 millisecondes
  nb_boucles1 =0 ,nb_boucles2 =0 ; nb_boucles3 =0 ; nb_boucles4=0 ;nb_boucles5 =0;
  Sum_Vread_inst = 0 ; // reset V
  Sum_Iread_inst = 0 ; // reset I
  V0 = analogRead(A0);
  temps1 = millis();
  // premier quart %%%%%%%%%%%%%%%%%%%%%%%
  for (int i1 = 0; i1 < i1max; i1++)  // premier quart, 50 steps, 5 ms environ
  {
    microloop (); //  on y met les operations a effectuer, P=U*I, somme, etc
    nb_boucles1++;
   }
  // T1, ici on doit arriver au sommet de la boucle
  VreadT1 = (analogRead(A0));
  IreadT1 = (analogRead(A1));
  V1 = VreadT1 ;
  //    sommet %%%%%%%%%%%%%%%%%%%%%%%%%
  if (IreadT1 >= Izero) {  //I et U sont en phase
    currentflow = true;    // direction de la puissance, signe positif
  } else {                 //I et U sont en opposition de phase
    currentflow = false;   // direction de la puissance, signe negatif
  }
  climb = false;    
  // on redescend %%%%%%%%%%%%%%%%%%%%%%%
  // durée 5ms, jusqu'au point le plus milieu, 50 steps
   while (analogRead(A0) > Vzero)  //
  {
    microloop ();
    nb_boucles2 ++;
  }
  V2 = analogRead(A0);
  //  point milieu  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // durée 5ms, jusqu'au point le plus bas, 50 steps
   for (int i1 = 0; i1 < i1max; i1++)  //
  {
    microloop ();
    nb_boucles3++;
  }
   //           point bas %%%%%%%%%%%%%%%%%%%%%%%%%%%
     climb = true;  // remontée
  VreadT3 = analogRead(A0) ; 
  V3 = VreadT3 ; 
  // quatrieme quart, xx steps %%%%%%%%%%%%%%%%%%%%%%%
  while (analogRead(A0) < Vzero)
  {
   microloop ();
    nb_boucles4 ++;
  }
  // fin de période, arrivé à Vzero
 //    VreadT4 = (analogRead(A0));
 // V4 = VreadT4 ;
   
  digitalWrite(14, LOW);    digitalWrite(15, LOW);  //eteint les leds bleue et verte
  // ajustement de la phase PLL
  nb_boucles_total = nb_boucles1 + nb_boucles2 + nb_boucles3 + nb_boucles4 ;
     if (nb_boucles_total < 195) {   //   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ajuster
    if (delayPLL > 75) {delayPLL--;};   // reduit la durée mais pas au dessous de 75 microsecondes 
    digitalWrite(14, HIGH);  // led verte
    digitalWrite(15, LOW);
  }  // reduire la durée, led verte 
     if (nb_boucles_total > 204) {   //   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ajuster
     if (delayPLL < 100) {delayPLL++;};  // augmente la durée mais pas dessus de 100 microsecondes
     // un changement de 1 unité de PLL provoque un delta de nb_boucles de 13 unités 
    digitalWrite(14, LOW);  // led bleue
    digitalWrite(15, HIGH);
  } 
  // Serial.println (delayPLL);
  // ajustement Vzero 
   if (Sum_Vread_inst >= 400){ Vzero ++ ;} ;  // on ajuste Vzero
   if (Sum_Vread_inst < -400){ Vzero -- ;} ;  // on ajuste Vzero
  // Serial.println (Sum_Vread_inst);
   if (Sum_Iread_inst >= 400){ Izero ++ ;} ;  // on ajuste Vzero
   if (Sum_Iread_inst < -400){ Izero -- ;} ;  // on ajuste Izero
  // Serial.println (Sum_Iread_inst);

  duree_cycle = (millis() - temps1);  //  
 
  if (duree_cycle == 20) {digitalWrite(16, LOW);} else {digitalWrite(16, HIGH);
  // Led_rouge ();  // affiche données anormales, 
   };      // out of sync, led rouge
 //  if (nb_boucles5 > 10) {delayPLL++;} ;  // retard de phase
if (delayPLL < 70 ) {delayPLL = 70 ;}     // pour eviter erreurs de debordement
if (delayPLL > 100 ) {delayPLL = 100 ;}   // idem

  // tension efficace
  V_eff = (sqrt (Vreadsqr/nb_boucles_total)) * 0.2380 ;  //  racine de la somme des V multiplié par un facteur, depend du transfo et du circuit (ici, 237V/996unités)
  Vreadsqr = 0 ;                               // reset en fin de cycle
  // Energie nn joules et Kilowattheures  
  E_seconde = (float)E_cycle* 0.008 ;                 // total pour une seconde * facteur pour obtenir valeur en joules
  E_cycle_affichage = E_seconde ;         // pour afficher la valeur  
  E_cumulee += (unsigned long) E_seconde ;    // energie cumulée depuis la mise en marche 
  E_kwh = E_cumulee / 3600000 ;               // E en Kwh  =  E cumulee  / (3600 * 1000)
  // Puissance
  puissance = E_seconde  ;  // puissance = energie / (temps * facteur)   // ici, divisé par 1
                                          // note; 1 watt = 1 joule par seconde
    I_efficace = puissance / V_eff  ;     //  I=P/U


  E_cycle = 0 ;  // reset 

if (duree_cycle < 15) {delay (12);} ; // on a perdu la synchro, on remet la phase a + 180 degres (+ 10 ms)

// pour lecture desynchronisée ;
 nb_boucles1l = nb_boucles1; nb_boucles2l = nb_boucles2 ;nb_boucles3l = nb_boucles3 ; nb_boucles4l = nb_boucles4 ;  // 
  // fin loop1
}
void Led_rouge () // pour afficher ce qui se passe si la led rouge s'allume
{
 Serial.print("LED:D=");
  Serial.print(duree_cycle);
  Serial.print(" PLL=");
  Serial.print(delayPLL);
  Serial.print(" nb_b=");
//  Serial.print(nb_boucles1l);
//  Serial.print("+");
//  Serial.print(nb_boucles2l);
//  Serial.print("+");
//  Serial.print(nb_boucles3l);
//  Serial.print("+");
//  Serial.print(nb_boucles4l);
//  Serial.print("=");
  Serial.println(nb_boucles_total);

}