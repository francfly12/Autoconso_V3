void setup1 ()
{ 
   pinMode(14, OUTPUT);   // led verte
   pinMode(15, OUTPUT);   // led bleue
   pinMode(16, OUTPUT);   // led bleue   
   delay(1000); // attends le core 0
trouve_Vzero();  // trouve V0
teste_t_zero();  // teste le temps de passage à T0


}