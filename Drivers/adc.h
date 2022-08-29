
#include <configuration.h>


extern char Calib;
extern char __xdata ContCalib, CalibOK;
extern signed long __xdata AcumCalib;

extern void ad_init (void);
extern int lectura (char channel);

extern signed int Calibrar(signed long Acum, signed int ord, signed int pend);
extern void CalibInit(char Cal);
extern signed long Calibracion(signed int Valor, signed int ord);
void descarte (signed int *Res, signed int *Cuentas_Ant, signed char *Cont);
void InitAnalog(struct Analog_ *pAIn);