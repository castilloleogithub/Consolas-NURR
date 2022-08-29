
extern char Calib;
extern char ContCalib, CalibOK;
extern signed long AcumCalib;

void adc_Init (unsigned char CS, unsigned char Ref);
void ad_reinit (void);
int lectura (unsigned char SS, unsigned char canal);

uint16_t Calibrar(uint16_t Acum, int16_t ord, int16_t pend);
void CalibInit(char Cal);
signed long Calibracion(signed int Valor, signed int ord);
void descarte (int16_t *Res, int16_t *Cuentas_Ant, signed char *Cont);

void ProcAnalog(struct Analog_ *pAIn, int NEnt, uint16_t *ord, uint16_t *pend);
void InitAnalog(struct Analog_ *pAIn);


