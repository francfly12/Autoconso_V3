
/******************************************************************************
V4_0 - dual core - suite
Programme pour une petite unité de transmission des infos de  consommation et de production d energie solaire
Utilise un Raspberry Pi Pico qui est en 3,3 V
Fait la lecture des valeurs tension, puissance du point de mesure

inspiré de la librairie EmonLib
https://docs.openenergymonitor.org/electricity-monitoring/index.html
Schema de cablage adapté au RPI 
pour une lecture de U par transfo et de I par CT  , 
I maxi 50 A
U connecté en A0 , ADC0, pin 31
I connecté en A1 , ADC1, pin 32
Transmet ensuite les infos sur un can bus vers les autres utilisateurs (Affichage ou pilotage ECS)
By FSA
Free software
*******************************************************************************
*/
//libraries
#include "pico/stdlib.h"  //  pour les fonctions specifiques rp2040, double core, etc...
#include "Arduino.h"
#define READVCC_CALIBRATION_CONST 1126400L  // voir librairie,
#define ADC_BITS 12
#define ADC_COUNTS (1 << ADC_BITS)

#define debug ;  // true pour debogage = mode verbeux, comment// pour mode normal

//--------------------------------------------------------------------------------------
// Variable declaration
//--------------------------------------------------------------------------------------
long sampleV;  //sample_ holds the raw analog read value
long sampleI;

double lastFilteredV, filteredV;  //Filtered_ is the raw analog value minus the DC offset
double filteredI;
double offsetV;                                                   //Low-pass filter output
double offsetI;                                                   //Low-pass filter output
double phaseShiftedV;                                             //Holds the calibrated phase shifted voltage.
double sqV, sumV, sqI, sumI, instP, sumP;                         //sq = squared, sum = Sum, inst = instantaneous
int startV;                                                       //Instantaneous voltage at start of sample window.
bool lastVCross, checkVCross;                                     //Used to measure number of times threshold is crossed.
bool condition1, condition2, condition3, condition4, condition5;  // divers booleens dans les algorithmes
int SupplyVoltage = 3300;
unsigned long StartupMillis, temps1;                      // doivent etre des "unsigned long"
unsigned long temps2, SumVread, nbloop1, nbloop2;         // utilisé pour le test V0 , temps2 pour compter 20 ms et SumVread pour totaliser les valeurs d'ADC, Vzero valeur en unités de la tension zero moyenne
unsigned long temps10,temps11,temps12 ;                   // pour des timers
bool Vpositif = true, Vnegatif = false;                   // tension , on est dans la demi boucle positive ou negative
bool climb = true;                                        // direction de la pente deltaV , negatif entre T1 et T3, positif en dehors
bool Zero;                                                // on vient de trouver la tension zero
int  Vzero = 2048;                                        // valeur moyenne de la tension, en unités de mesure
int  Izero = 2048;                                        // idem pour Intensité
int epsilon = 3 ;                                         // utilisé pour calcul Vzero, Izero
int  Vread, VreadT0, VreadT1, VreadT2, VreadT3, VreadT4;  // valeur courante de la lecture ADC(0)
int  Vread_new, Vread_delta, Vread_inst,Vread_instp, VreadT5;  // pour diviser et filtrer
int  V0,V1,V2,V3,V4 ;                                        // divers pour tests
long Sum_Vread_inst ;                                     // cumul pour calculer Vzero
long Sum_Iread_inst , IreadT2l;                                     // cumul pour calculer Izero
int  Iread, IreadT1, IreadT0l, IreadT1l;  // valeur courante de la lecture ADC(1)
int  Iread_affichage ;   // pour diviser et filtrer
int  Iread_new, Iread_instl ;
float Iread_delta , Iread_inst= 2048,IreadT3l, IreadT4l;
float Iread_newf, IreadT5l, Iinstant, Iinstantp ;         // Iinstant= valeur finale I filtrée ;Iinstantp pour la valeur absolue
long T0;                                                  // temps au pied de la courbe, en U=0 V
long T1;                                                  // temps au sommet de l'alternance positive , passage a Vmax
long T2;                                                  // temps en milieu de courbe, normalement passage à 0 Volts
long T3;                                                  // idem, passage à Vmin
long T4;                                                  // fin de courbe, passage à 0 Volts
long duree_cycle;                                         // pour connaitre la durée d'un cycle
long deltaV;                                              // delta de tension autour de zero
long timerPLL;                                            // timer PLL, represente la durée du cycle de 20ms
long Vmin = 0;                                            // tension mini rencontrée (en unités de mesure, pas en volts)
long Vmax = 0;                                            // tension maxi rencontrée
long Imin = 0;                                            // idem pour l intensité
long Imax = 0;                                            // idem
float I_efficace ;                                        // intensité en watts efficaces
float Vmedian = 0;                                        // tension mediane = Vmax-Vmin ; est un float sinon anomalies de calcul
float Imedian = 0;                                        // idem pour l'intensité
long E_cycle = 0;                                         // energie pour un cycle (20ms)
unsigned long E_cumulee = 0;                              // energie totale depuis la mise en marche  , ici un unsigned long car un float n'a que 6/7digits de precision
unsigned long E_kwh = 0;                                  // energie cumulée en Kwh
float puissance;                                           // puissance instantanée = E/t en Watts efficaces
float V_eff, Vreadsqr ;                                     // tension efficace, somme des tensions au carré 
long power_factor = 100000   ;                             // puissance = E/(60 * constante)
float E_cycle_affichage;                                   // pour affichage stable E_cycle
float E_seconde;                                           // energie en une seconde
long E_inst ;                                             // energie instantanée; pour un  microcyle
int delayPLL = 85;                               // pour obtenir in fine un delai de 50 microsecondes * 100 = 5 millisecondes
int i1max = 50;                                           //nombre de micro boucles dans un quart de l'onde = 100-temps de calcul
int i2max = 2 * i1max;                  // pour avoir une durée de 10 ms environ
int i3max;
int nb_boucles_total,nb_boucles1, nb_boucles2,nb_boucles3,nb_boucles4, nb_boucles5  ;  // compte le nombre de micro boucles en une période (environ 400 en 20ms )
int nb_boucles_totall,nb_boucles1l, nb_boucles2l,nb_boucles3l,nb_boucles4l, nb_boucles5l  ;  // pour lecture desynchronisée
bool currentflow = true;           // donne le signe de la puissance, true = U et I en phase, false sinon, signe - devant la puissance
float valeur ;  // pour la simulation 50 Hz
double hzsimule ;  // pour simulation 50 hZ

void setup() {  // *******************************************************************************
  Serial.begin(230400);
//  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(26, INPUT);        // pour eviter que le pin pour l ADC0 soit en sortie pull up active
  pinMode(27, INPUT);        // idem ADC 1
  pinMode(28, INPUT);        // idem ADC 2
  analogReadResolution(12);  // ADC 12 bits
  delay(500);                // pour laisser charger les capas de l alimentation du point milieu
  while (!Serial)
    ;  // attends que le port serie soit démarré


  StartupMillis = millis();           //temps au startup
  temps1 = millis() - StartupMillis;  // valeur actuelle du temps
  Serial.println();                   // ligne vide
  Serial.print("\n");                 // retour a la ligne
  Serial.println("demarrage");
  Serial.print("\n");  // retour a la ligne
  Serial.print("\n");  // retour a la ligne
  delay(500);          //  stabiliser les alims
  temps11 = millis();  // init
  temps12 = millis();  // init
}
// new loop
void loop() {
  
/*if (currentflow = true) {
    Serial.print(" +");
  } else {
   Serial.print(" -");
  };
*/ 
  Serial.print("V_eff=");
  Serial.print(V_eff);
  Serial.print ("V   Ecyc=");
  Serial.print(E_cycle_affichage); 
//  Serial.print(" Iread_instl = ");
//  Serial.print(Iread_instl);
  Serial.print("   E_cumul=");
  Serial.print(E_cumulee);
  Serial.print("Joules   Puiss=");
  if (currentflow == true) {Serial.print("+");} else {Serial.print("-");} ;
  Serial.print(puissance);
  Serial.print("W   Energie=");
  Serial.print(E_kwh);
  Serial.print("Kwh  ou ");
  Serial.print (E_cumulee / 3600); 
  Serial.print ("Wattheures    Intensité=");
  Serial.print(I_efficace);
  Serial.print ("A");
// Serial.print(IreadT1l);
// Serial.print (" IT2l=");
// Serial.print(IreadT2l);
// Serial.print (" Delta=");
// Serial.print(IreadT3l);
// Serial.print (" IT4l=");
// Serial.print(IreadT4l);
// Serial.print (" IT5l=");
// Serial.print(IreadT5l);
  Serial.print("  PLL=");
  Serial.print(delayPLL);

  Serial.print("\n");  // retour a la ligne
 
  delay(100);  // un certain temps

  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on, petit flash
  delay(1);                         // petit flash
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
// 
 
}
//  junk code :
// Serial.print("  V_eff=");
// Serial.print(V_eff);
//  Serial.print (" Vread_inst = ");
//  Serial.print(Vread_inst);
 
//  Serial.print (" E_inst = ");
//  Serial.print(E_inst); 
//  Serial.print(" Iread_instl = ");
//  Serial.print(Iread_instl);
//  Serial.print(" E_cumulee = ");
//  Serial.print(E_cumulee);
//  Serial.print(" puissance = ");
//  Serial.print(puissance);
//  Vread = (analogRead(A0));
//  Serial.print("Vmin=");
//  Serial.print(Vmin);
//  Serial.print(" Vmax=");
//  Serial.print(Vmax);
 // Serial.print(" Vmedian = ");
 // Serial.print(Vmedian);
 // Serial.print(" Vzero=");
 // Serial.print(Vzero);
 // Serial.print("Izero=");
 // Serial.print(Izero);
 
 //  Serial.print ("   IreadT1=");
 // Serial.print(IreadT1);
 //  Serial.print ("   IreadT2=");
 // Serial.print(IreadT2);
 //  Serial.print ("   IreadT3=");
 // Serial.print(IreadT3);
//   Serial.print ("   IreadT4=");
//  Serial.print(IreadT4);
//   Serial.print ("   IreadT5=");
//  Serial.print(IreadT5);

 //   Serial.print("  Sum_Iread_inst = ");
 // Serial.print( Sum_Iread_inst);
 // Serial.print(" V0=");
 // Serial.print(V0);
 //   Serial.print("  V1=");
 // Serial.print(V1);
 // Serial.print("  V2=");
 // Serial.print(V2);
 //  Serial.print("  V3=");
 // Serial.print(V3);
 //  Serial.print("  V4=");
//  Serial.print(V4);
//  Serial.print(IreadT1);
//  Serial.print(" I=");
//  Serial.print(Iread_instl);
 
//  Serial.print(" VreadT3 = ");
//  Serial.print(VreadT3);
//  Serial.print(" VreadT4 = ");
//  Serial.print(VreadT4);
/*  // old loop
void loop() {  //  *******************************************************************************
 // core 0 dédié à l'affichage et aux transmissions
/*  
// affichage de l intensité sur le plotter
Serial.print (Iread_inst);
//Serial.println (",");
//Serial.print (Vread_new);
Serial.print("\n");
*/
/*
//  Controle des boucles temporisées
    temps10 = millis();
  //  if ((temps11 - temps10) >= 50) {temps11=temps10;Every50ms();}  // a faire toute les 50 ms
    if ((temps12 - temps10) >= 1000) {temps12=temps10; Every1000ms();}  // a faire toute les secondes


  // Serial.print("  V_eff=");
  // Serial.print(V_eff);
  Serial.print(" D=");
  Serial.print(duree_cycle);
  Serial.print(" PLL=");
  Serial.print(delayPLL);
  Serial.print("  nb_boucles=");
  Serial.print(nb_boucles1l);
  Serial.print("+");
  Serial.print(nb_boucles2l);
  Serial.print("+");
  Serial.print(nb_boucles3l);
  Serial.print("+");
  Serial.print(nb_boucles4l);
  Serial.print("=");
  Serial.print(nb_boucles_total);
  if (currentflow = true) {
    Serial.print(" +");
  } else {
   Serial.print(" -");
  };
//  Serial.print (" Vread_inst = ");
//  Serial.print(Vread_inst);
 
//  Serial.print (" E_inst = ");
//  Serial.print(E_inst); 
//  Serial.print(" E_cycle = ");
//  Serial.print(E_cycle_affichage);
//  Serial.print(" E_cumulee = ");
//  Serial.print(E_cumulee);
//  Serial.print(" puissance = ");
//  Serial.print(puissance);

  Serial.print("\n");  // retour a la ligne
                       //  Serial.print(" Vmin = ");
                       //  Serial.print(Vmin);
                       //  Serial.print(" Vmax = ");
                       //  Serial.print(Vmax);


  delay(1000);  // un certain temps

  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on, petit flash
  delay(1);                         // petit flash
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
//  Serial.print(".");                //

  
}  // fin loop  *********************************************************
*/