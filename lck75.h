#pragma once

#define _x_ KC_NO

#include "quantum.h"

#define LAYOUT_default(\
    K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C,      K0E, K0F, \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C,      K1E, K1F, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C,      K2E, K2F, \
    K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B,           K3E, K3F, \
    K40,      K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C,      K4E, K4F, \
    K50, K51, K52,                K56,                K5A, K5B, K5C,      K5E, K5F \
) \
{ \
  { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, _x_, K0E, K0F}, \
  { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, _x_, K1E, K1F}, \
  { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, _x_, K2E, K2F}, \
  { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, _x_, _x_, K3E, K3F}, \
  { K40, _x_, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, _x_, K4E, K4F}, \
  { K50, K51, K52, _x_, _x_, _x_, K56, _x_, _x_, _x_, K5A, K5B, K5C, _x_, K5E, K5F} \
}

