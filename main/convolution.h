// Everything here is lifted from the convolution example file

#include <arm_math.h>
#include <arm_const_structs.h> // in the Teensy 4.0 audio library, the ARM CMSIS DSP lib is already a newer version
#include <utility/imxrt_hw.h> // necessary for setting the sample rate, thanks FrankB !



extern "C" uint32_t set_arm_clock(uint32_t frequency);

const double PROGMEM FHiCut = 2500.0;
const double PROGMEM FLoCut = -FHiCut;
// define your sample rate
const double PROGMEM SAMPLE_RATE = 44100;  

// the latency of the filter is meant to be the same regardless of the number of taps for the filter
// partition size of 128 translates to a latency of 128/sample rate, ie. to 2.9msec with 44.1ksps
// to this 2.9 ms figure, you have to add in latency involved in collecting the audio data into a 128 sample block, as well as in sending it out again in a block

#define partitionsize 128
#define BUFFER_SIZE 128

#define DEBUG
#define FOURPI  (2.0 * TWO_PI)
#define SIXPI   (3.0 * TWO_PI)
  
int32_t sum;  //used for elapsed time
int idx_t = 0;  //used for elapsed time
uint8_t PROGMEM FIR_filter_window = 1;
const uint32_t PROGMEM FFT_L = 2 * partitionsize; 
float32_t mean = 1;  // used for elapsed time
const uint32_t PROGMEM FFT_length = FFT_L;
const int PROGMEM nfor = nc / partitionsize; // number of partition blocks --> nfor = nc / partitionsize       ** nfor should not be greater than 140
float32_t DMAMEM maskgen[FFT_L * 2];            
float32_t DMAMEM  fftout[nfor][512];  // nfor should not exceed 140
const uint32_t N_B = FFT_L / 2 / BUFFER_SIZE;
uint32_t N_BLOCKS = N_B;

elapsedMillis msec = 0;


void setI2SFreq(int freq) {
  // thanks FrankB !
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  int n2 = 1 + (24000000 * 27) / (freq * 256 * n1);
  double C = ((double)freq * 256 * n1 * n2) / 24000000;
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2, true);
  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
       | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
       | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f 
  Serial.println(n1);
  Serial.println(n2);
  Serial.println(c0);
  Serial.println(c1);
  Serial.println(c2);

}


void flexRamInfo(void)
{ // credit to FrankB, KurtE and defragster !
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)
  int itcm = 0;
  int dtcm = 0;
  int ocram = 0;
  Serial.print("FlexRAM-Banks: [");
  for (int i = 15; i >= 0; i--) {
    switch ((IOMUXC_GPR_GPR17 >> (i * 2)) & 0b11) {
      case 0b00: Serial.print("."); break;
      case 0b01: Serial.print("O"); ocram++; break;
      case 0b10: Serial.print("D"); dtcm++; break;
      case 0b11: Serial.print("I"); itcm++; break;
    }
  }
  Serial.print("] ITCM: ");
  Serial.print(itcm * 32);
  Serial.print(" KB, DTCM: ");
  Serial.print(dtcm * 32);
  Serial.print(" KB, OCRAM: ");
  Serial.print(ocram * 32);
#if defined(__IMXRT1062__)
  Serial.print("(+512)");
#endif
  Serial.println(" KB");
  extern unsigned long _stext;
  extern unsigned long _etext;
  extern unsigned long _sdata;
  extern unsigned long _ebss;
  extern unsigned long _flashimagelen;
  extern unsigned long _heap_start;

  Serial.println("MEM (static usage):");
  Serial.println("RAM1:");

  Serial.print("ITCM = FASTRUN:      ");
  Serial.print((unsigned)&_etext - (unsigned)&_stext);
  Serial.print("   "); Serial.print((float)((unsigned)&_etext - (unsigned)&_stext) / ((float)itcm * 32768.0) * 100.0);
  Serial.print("%  of  "); Serial.print(itcm * 32); Serial.print("kb   ");
  Serial.print("  (");
  Serial.print(itcm * 32768 - ((unsigned)&_etext - (unsigned)&_stext));
  Serial.println(" Bytes free)");
 
  Serial.print("DTCM = Variables:    ");
  Serial.print((unsigned)&_ebss - (unsigned)&_sdata);
  Serial.print("   "); Serial.print((float)((unsigned)&_ebss - (unsigned)&_sdata) / ((float)dtcm * 32768.0) * 100.0);
  Serial.print("%  of  "); Serial.print(dtcm * 32); Serial.print("kb   ");
  Serial.print("  (");
  Serial.print(dtcm * 32768 - ((unsigned)&_ebss - (unsigned)&_sdata));
  Serial.println(" Bytes free)");

  Serial.println("RAM2:");
  Serial.print("OCRAM = DMAMEM:      ");
  Serial.print((unsigned)&_heap_start - 0x20200000);
  Serial.print("   "); Serial.print((float)((unsigned)&_heap_start - 0x20200000) / ((float)512 * 1024.0) * 100.0);
  Serial.print("%  of  "); Serial.print(512); Serial.print("kb");
  Serial.print("     (");
  Serial.print(512 * 1024 - ((unsigned)&_heap_start - 0x20200000));
  Serial.println(" Bytes free)");

  Serial.print("FLASH:               ");
  Serial.print((unsigned)&_flashimagelen);
  Serial.print("   "); Serial.print(((unsigned)&_flashimagelen) / (2048.0 * 1024.0) * 100.0);
  Serial.print("%  of  "); Serial.print(2048); Serial.print("kb");
  Serial.print("    (");
  Serial.print(2048 * 1024 - ((unsigned)&_flashimagelen));
  Serial.println(" Bytes free)");
  
#endif
}
