int ledRed = 3;
int ledGreen = 9;
int i = 0;
int j = 0;
int k = 0;
int l = 0;
int m = 0;
char Signal[30];
bool readCommand = false;
char command[30];
char storeCommand[30];
char compare[30];
char card[30];
char cardlist[10][7];
char cardtest[30];

void setup() {
  Serial.begin(9600);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  //digitalWrite(ledRed, HIGH); 
}

void loop() {
  if (Serial.available() > 0)
  {
    Signal[0] = (char)Serial.read();

    if (Signal[0] == '>')
    {
      readCommand == false;
      for(j=0;j<i;j++)
      {
        command[j] = storeCommand[j];
        command[j+1] = '\0';
      }
      Serial.println(command);
 
    }
    if (readCommand == true)
    {
      storeCommand[i] = Signal[0];
      i++;
    }
    if (Signal[0] == '<')
    {
      i=0;
      command[0]='\0';
      storeCommand[0]='\0';
      readCommand = true;
    }
  }
  if (strcmp(command, "CLEARCARDS") == 0)
  {
    for (l=0;l<k;l++)
    {
    cardlist[l][0]='\0';
    }
    Serial.println("clearcards");
    command[0]='\0';
    k=0;
  }
  
  if (strcmp(command, "OPENDOOR") == 0)
  {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
    delay(3000);
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
    command[0]='\0';
  }
  
  if (strcmp(strncpy(compare,command,7), "ADDCARD") == 0)
  {
    strncpy(card,command+7,6);
    card[6]='\0';
    strcpy(cardlist[k], card);
    for (l=0;l<k+1;l++)
    {
    Serial.println(cardlist[l]);
    }
    k++;
    card[0]='\0';
    command[0]='\0';
  }
  
  if (strcmp(strncpy(compare,command,8), "FAKETEST") == 0)
  {
    for (m = 0; m < k; m++)
    {
      if (strcmp(strncpy(cardtest,command+8,6),cardlist[m]) == 0)
      {
        Serial.println("ENTER");
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, HIGH);
        delay(3000);
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
      }
    }
    command[0]='\0';
  }
}
