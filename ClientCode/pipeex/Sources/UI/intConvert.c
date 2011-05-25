/* Integer to ASCII */
void Itoa(int Number, char Digits, char Decimal, char *String)
{
 /* Get starting position in string array */
 char *PositionSave, *Position = String + Digits;
  /* Negative flag */
 char NegativeFlag = FALSE;
 /* Charactor buffer */
 char Chr;
 /* Working UINT16 */
 unsigned int uInteger;
 /* Loop index */
 int Index;
 /* Number of digits converted */
 int DigitCount = 0;

 /* Make sure we are converting more than one digit */
 if(!Digits)
  return;

 /* Are we working with a negative number— */
 if (Number < 0) {
  /* It is a negative number */
  NegativeFlag = TRUE;
  /* Remove the sign bit from the integer */
  uInteger = ((unsigned int)(-(1 + Number))) + 1;
 } else {
  /* Not a negative number */
  uInteger = Number;
 }

 /* NULL terminate the string */
 *Position = 0;

 /* Do the conversion */
 do {
  /* Decrement the digits counter */
  Digits--;
  /* Get the ASCII charactor */
  *--Position = '0' + (uInteger % 10);
  /* Divide by 10 */
  uInteger /= 10;
  /* Have we reached the end of the number– */
  if(!uInteger)
   break;
  /* Increment the digit counter */
  DigitCount++;
 } while (Digits);

 /* Do we need to add a decimal point˜ */
 if(Decimal && Digits) {
  /* Remove another digit position */
  Digits--;

  /* Do we have enough space for a leading 0™ */
  if(Digits && (DigitCount < Decimal)){
   for(Index = Decimal - DigitCount; Index > 0; Index--) {
    *--Position = '0';
    DigitCount++;
    Digits--;
    if(!Digits)
     break;
   }
  }

  /* Decrement and save the pointer */
  Position--;
  PositionSave = Position;

  /* Move the charactors */
  for(Index = DigitCount - Decimal; Index >= 0; Index--) {
   Chr = Position[1];
   Position[0] = Chr;
   Position++;
  }
  /* Insert the decimal point */
  *Position = '.';
  /* Restore the pointer */
  Position = PositionSave;
 }

 /* Do we need to add the negative sign? */
 if (NegativeFlag && Digits) {
  /* Add the negative charator */
  *--Position = '-';
  /* Reduce the digit count */
  Digits--;
 }

 /* Do we need to pad any charactor positions? */
 for(Index = Digits; Index != 0; Index--)
   *--Position = ' ';
}

/* ItoaMsg */
void ItoaMsg(char *Buffer, char *Msg, int Number, char Digits, char Decimal) {
 /* Local variables */
 char Array[11];

 /* Convert Number to string */
 Itoa(Number, Digits, Decimal, &Array[0]);

 /* Copy Msg to Buffer */
 (void) strcpy(Buffer, Msg);

 /* Add Number string to Buffer */
 (void) strcat(Buffer, &Array[0]);
}
