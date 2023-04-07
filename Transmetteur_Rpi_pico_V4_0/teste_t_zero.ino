


void teste_t_zero() {      // *************************************************************************
  // but : determiner le moment du passage de la tension par zero, dans le sens croissant, positif
  // premier cas, on se trouve dans la partie positive de la courbe, il faut attendre a passer coté negatif
  nbloop1 = 0;
  temps1 = millis()+12;  // temps du cycle est de 20ms, 12 ms assure un demi-cycle
  
   while ((millis() < (temps1))&&(analogRead(A0)> Vzero ) ){  // tant que le temps est inferieur à 12 ms et que la tension est positive, on attend
    nbloop1++ ; // pour les tests
   };
   
// **************************************
// arrivé ici, on est forcément sur la partie negative de la sinusoide
// on cherche à nouveau le moment ou la tension sera Vzero mais en croissant cette fois-ci
// si la tension est juste de Vzero, il faut attendre un peu pour etre sur de passer dans la partie negative ;
//if ((analogRead(A0)>= Vzero )) {delayMicroseconds(10);}  // tout petit delai 
nbloop1 = 0;
temps1 = millis()+12;  // temps du cycle est de 20ms, 12 ms assure un demi-cycle
 while ((millis() < (temps1))&&(analogRead(A0)< Vzero ) ){  // tant que le temps est inferieur à 12 ms et que la tension est negative, on attend
    nbloop1++ ; // pour les tests
   };
Vnegatif = true ;
Vpositif = false ;
climb = true ;

/*
 #ifdef debug  // debut debug
 Vread = (analogRead(A0)) ;
  Serial.print("  teste_t_zero : ");
  Serial.print(" Vread = ");
  Serial.print(Vread);
  Serial.print(" Vzero = ");
  Serial.print(Vzero);
  Serial.print(" nbloop1 = ");
  Serial.print(nbloop1);
  Serial.print("\n");  // retour a la ligne
#endif                 // fin debug \
*/
   }





/*
    if ((climb == true) && ((abs(deltaV) <= 25))) {  // on est à O volts et synchro +- 25 unites
      Vpositif = true;
      Vnegatif = false;
      T0 = temps1;
      Zero = true;          
      return;
    }  //  si Vread est proche de zero, ok, retour
       // sinon, on n'a pas trouvé de zero ou on n'est pas en montant
if (Vnegatif == true) {    // on monte mais on est trop loin de la cible Vzero, il faut adapter la boucle PLL
                          if (deltaV >= 0) { // Vread supérieur à Vzero , on est en avance sur la boucle, pas encore à zero
                           timerPLL++ ;
                          }
                          else {  // on monte mais on est en retard de phase PLL, deja passé par zero
                          }
                          timerPLL-- ;
                        }  
// on est dans la partie positive de la courbe

  }  // Wend
  // on a passé 20 ms et pas de passage par zéro
  Serial.println("pas de signal") ;
  */

