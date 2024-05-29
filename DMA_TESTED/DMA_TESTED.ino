
#define NUM_LEDS 80

#include <DmaSpi.h>
#define DMASIZE NUM_LEDS * 4 + 4 + (NUM_LEDS / 4)  // (add start&end frame)

byte buffer[DMASIZE];
DMAMEM byte address[DMASIZE];

#define TSPEED 20000000  // 16Mhz , can edit this for differen results
SPISettings dotstar(TSPEED, MSBFIRST, SPI_MODE0);
ActiveLowChipSelect cs(0, dotstar);
// we dont need the cs here, but using the SPI_settings
DmaSpi1::Transfer trx(address, DMASIZE, nullptr, 0, &cs);

#include <FastLED.h>
// from https://github.com/FastLED/FastLED
CRGB leds[NUM_LEDS];

#define LEDMOS 14  // A0(14) for N-channel-MOSFET gate pin

void setup() {

  Serial.begin(115200);
  Serial.println("Started...");
  // put your setup code here, to run once:
  pinMode(LEDMOS, OUTPUT);
  digitalWrite(LEDMOS, HIGH);
  Serial.println(("Hello, this is apa102 SPI1 test"));
  SPI1.begin();
  DMASPI1.begin();
  DMASPI1.start();
  //FastLED.addLeds<APA102, 26, 27, BGR, DATA_RATE_MHZ(20)>(leds, NUM_LEDS);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(("TESTING SPI1"));
  fill_solid(&(leds[0]), NUM_LEDS, CRGB::Red);  //take some tests here;
  Spishow();
  Serial.println(("RED"));
  delay(2000);

  fill_solid(&(leds[0]), NUM_LEDS, CRGB::Green);  //take some tests here;
  Spishow();
  Serial.println(("GREEN"));
  delay(2000);

  fill_solid(&(leds[0]), NUM_LEDS, CRGB::Blue);  //take some tests here;
  Spishow();
  Serial.println(("BLUE"));
  delay(2000);

  fill_solid(&(leds[0]), NUM_LEDS, CRGB::Black);  //take some tests here;
  Spishow();
  Serial.println(("CLEAR"));
  delay(2000);

  Serial.println(("TESTING DMA"));
  fill_solid(&(leds[0]), NUM_LEDS, CRGB::Red);  //take some tests here;
  DMAshow();
  Serial.println(("RED_DMA"));
  delay(2000);

  fill_solid(&(leds[0]), NUM_LEDS, CRGB::Green);  //take some tests here;
  DMAshow();
  Serial.println(("GREEN_DMA"));
  delay(2000);

  fill_solid(&(leds[0]), NUM_LEDS, CRGB::Blue);  //take some tests here;
  DMAshow();
  Serial.println(("BLUE_DMA"));
  delay(2000);

  fill_solid(&(leds[0]), NUM_LEDS, CRGB::Black);  //take some tests here;
  DMAshow();
  Serial.println(("ALL_DMA"));
  delay(2000);
  Serial.println(("Done, rerun"));
}



void DMAshow() {

  memset(&buffer, 0xFF, DMASIZE);  //this also sets the end frames

  int dmaoffset;
  byte ploc;
  // start frames of apa102,
  buffer[0] = 0x00;
  buffer[1] = 0x00;
  buffer[2] = 0x00;
  buffer[3] = 0x00;

  for (int n = NUM_LEDS; n >= 1; n--)  // leave the 0-3 unchanged 0x00;
  {
    dmaoffset = n * 4;
    ploc = n - 1;
    //buffer[dmaoffset] = .... ;
    /*this is already 0xFF as memset above, or you
      can use buffer[dmaoffset] = 111abcde; with abcde = 1-31 to decide this pixel brightness
      not this uses a slower PWM, and shows unaceptable flickering if using with Persistence of Vision Projects.
    */

    buffer[dmaoffset] = 0xFF;                // do some corrections here if you wish
    buffer[dmaoffset + 1] = (leds[ploc].b);  // do some corrections here if you wish
    buffer[dmaoffset + 2] = (leds[ploc].g);  // do some corrections here if you wish
    buffer[dmaoffset + 3] = (leds[ploc].r);  // do some corrections here if you wish
  }

  //this controlls the FPS, if set.
  //nope, not in this example

  // wait, incase the last DMA launch isn't complete yet.
  while (trx.busy()) {}

  memcpy(address, buffer, DMASIZE);
  DMASPI1.registerTransfer(trx);
}


void Spishow() {

  SPI1.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));  // 37.5Mhz->38MHz  = 600/2/2/2/2
  memset(&buffer, 0xFF, DMASIZE);                                     //this also sets the end frames

  int dmaoffset;
  byte ploc;
  // start frames of apa102,
  buffer[0] = 0x00;
  buffer[1] = 0x00;
  buffer[2] = 0x00;
  buffer[3] = 0x00;

  for (int n = NUM_LEDS; n >= 1; n--)  // leave the 0-3 unchanged 0x00;
  {
    dmaoffset = n * 4;
    ploc = n - 1;
    //buffer[dmaoffset] = .... ;
    /*this is already 0xFF as memset above, or you
      can use buffer[dmaoffset] = 111abcde; with abcde = 1-31 to decide this pixel brightness
      not this uses a slower PWM, and shows unaceptable flickering if using with Persistence of Vision Projects.
    */

    buffer[dmaoffset] = 0xFF;                // do some corrections here if you wish
    buffer[dmaoffset + 1] = (leds[ploc].b);  // do some corrections here if you wish
    buffer[dmaoffset + 2] = (leds[ploc].g);  // do some corrections here if you wish
    buffer[dmaoffset + 3] = (leds[ploc].r);  // do some corrections here if you wish
  }

  for (int n = 0; n<DMASIZE ; n++) {
    SPI1.transfer(buffer[n]); 
  }
  //SPI1.transfer(buffer, nullptr, DMASIZE);



  SPI1.endTransaction();
}