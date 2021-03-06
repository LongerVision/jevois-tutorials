// JeVois + Arduino blink for X

// Pin for LED, will turn on as we detect the desired object:
#define LEDPIN 17

// Serial port to use: on chips with USB (e.g., 32u4), that usually is Serial1.
// On chips without USB, use Serial:
#define SERIAL Serial1

// Buffer for received serial port bytes:
#define INLEN 256
char instr[INLEN + 1];

// Our desired object: should be one of the 1000 ImageNet category names
#define CATEGORY "computer_keyboard"

void setup()
{
  SERIAL.begin(115200);
  SERIAL.setTimeout(500);
  
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);
}

void loop()
{
  byte len = SERIAL.readBytesUntil('\n', instr, INLEN);
  instr[len] = 0;

  char * tok = strtok(instr, " \r\n");
  int state = 0, i; float score;
  
  while (tok)
  {
    // State machine:
    // 0: start parsing; if we get DO, move to state 1, otherwise state 1000
    // 1: got DO, decode category name; if it is the one we want, move to state 2, otherwise stay in state 1
    // 2: got DO followed by the category we are interested in - it's a hit!
    // 1000: did not get DO, we stay in this state until we run out of tokens
    switch (state)
    {
      // First token should be: DO
      case 0:
        if (strcmp(tok, "DO") == 0) state = 1; else state = 1000;
        // We are done with this token. Break from the switch() statement
        break;

      // Second token should be: category:score
      case 1:
        // Find the ':' between category and score:
        i = strlen(tok) - 1;
        while (i >= 0 && tok[i] != ':') --i;
        
        // If i is >= 0, we found a ':'; terminate the tok string at that ':':
        if (i >= 0)
        {
          tok[i] = '\0';
 
          // Note: we don't care about score here, but it could be obtained as:
          score = atof(&tok[i+1]);
         }
        
        // Is the category name what we want?
        if (strcmp(tok, CATEGORY) == 0) state = 2;
        
        // We are done with this token. Break from the switch() statement
        break;
      
      // In any state other than 0 or 1: do nothing
      default:
        break;
    }
    
    // Move to the next token:
    tok = strtok(0, " \r\n");
  }

  // If any of the category names in the message was the one we want, then we are in state 2 now.
  // Otherwise we are in some other state (most likely 1).
  if (state == 2)
    digitalWrite(LEDPIN, LOW); // turn LED on (it has inverted logic)
  else
    digitalWrite(LEDPIN, HIGH); // turn LED off
}
