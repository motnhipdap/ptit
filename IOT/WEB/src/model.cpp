#include "model.h"
// temp(F = C * 1.8 + 32) hum(%) pres(inch Hg = hPa / 33.8639) thu xuan he dong
double score(double * input) {
    double var0;
    if (input[0] < -0.10715482) {
        if (input[1] < -0.743462) {
            var0 = -0.07366831;
        } else {
            var0 = -0.05403349;
        }
    } else {
        if (input[1] < 1.5565627) {
            var0 = 0.050275024;
        } else {
            var0 = -0.025574638;
        }
    }
    double var1;
    if (input[5] < 1.0) {
        if (input[4] < 1.0) {
            var1 = -0.029195884;
        } else {
            var1 = 0.026120441;
        }
    } else {
        if (input[1] < 1.5565627) {
            var1 = 0.04826243;
        } else {
            var1 = -0.030740593;
        }
    }
    double var2;
    if (input[0] < -0.10715482) {
        if (input[1] < 0.7355907) {
            var2 = -0.048177503;
        } else {
            var2 = -0.072930165;
        }
    } else {
        if (input[1] < 1.5174688) {
            var2 = 0.048861306;
        } else {
            var2 = -0.017927408;
        }
    }
    double var3;
    if (input[0] < -0.10715482) {
        if (input[0] < -0.23159954) {
            var3 = -0.053170428;
        } else {
            var3 = -0.02683689;
        }
    } else {
        if (input[0] < 0.79015714) {
            var3 = 0.052649416;
        } else {
            var3 = 0.026354315;
        }
    }
    double var4;
    if (input[0] < -0.10715482) {
        if (input[0] < -0.43464094) {
            var4 = -0.053934913;
        } else {
            var4 = -0.031417612;
        }
    } else {
        if (input[0] < 0.8360052) {
            var4 = 0.04901115;
        } else {
            var4 = 0.029514818;
        }
    }
    double var5;
    if (input[6] < 1.0) {
        if (input[3] < 1.0) {
            var5 = 0.026196625;
        } else {
            var5 = -0.007883947;
        }
    } else {
        if (input[2] < 2.0828598) {
            var5 = -0.045639705;
        } else {
            var5 = 0.0063113957;
        }
    }
    double var6;
    if (input[6] < 1.0) {
        if (input[3] < 1.0) {
            var6 = 0.027620345;
        } else {
            var6 = -0.0059624687;
        }
    } else {
        var6 = -0.03904342;
    }
    double var7;
    if (input[0] < -0.10715482) {
        if (input[0] < -0.65078175) {
            var7 = -0.055144448;
        } else {
            var7 = -0.03158747;
        }
    } else {
        if (input[0] < 0.79015714) {
            var7 = 0.049019545;
        } else {
            var7 = 0.026549852;
        }
    }
    double var8;
    if (input[0] < -0.10715482) {
        if (input[2] < 1.4668354) {
            var8 = -0.04396161;
        } else {
            var8 = -0.009845238;
        }
    } else {
        if (input[0] < 0.64606327) {
            var8 = 0.051352825;
        } else {
            var8 = 0.03282911;
        }
    }
    double var9;
    if (input[0] < -0.10715482) {
        if (input[2] < -0.10122635) {
            var9 = -0.054738887;
        } else {
            var9 = -0.034931023;
        }
    } else {
        if (input[2] < -2.5093212) {
            var9 = -0.0056481366;
        } else {
            var9 = 0.03738943;
        }
    }
    return var0 + var1 + var2 + var3 + var4 + var5 + var6 + var7 + var8 + var9;
}
