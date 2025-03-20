ISR(TIMER1_COMPA_vect){

  cont++;
  if(cont>=8000){
    cont=0;
    contOff++;
    configurationRadio();
    if(contOff>=4){
      contOff=0;
      digitalWrite(Bomba, LOW); 
      digitalWrite(ActivadoV, HIGH); 
      digitalWrite(ActivadoR, HIGH);
      analogWrite(RED, 0);
      analogWrite(GREEN,0 );
    }
  }

  
  
  
}
