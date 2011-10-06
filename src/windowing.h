// -*- C++ -*-
/*!
 * @file  windowing.h
 * @author Yosuke Matsusaka <yosuke.matsusaka@aist.go.jp> and OpenHRI development team
 *
 * Copyright (C) 2010
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * @date  $Date$
 *
 * $Id$
 */

#ifndef WINDOWING_H
#define WINDOWING_H

#ifndef M_PI
#define M_PI 3.14159265
#endif

#include <stdlib.h>
#include <math.h>

enum WindowType
{
  Hamming,
  Hanning,
  Blackman,
  Gauss
};

double *CreateWindowDouble(int len, enum WindowType type);
float *CreateWindowFloat(int len, enum WindowType type);
double *CreateGaussWindowDouble(int len, int samplef, int centerf);
float *CreateGaussWindowFloat(int len, int samplef, int centerf);
void ApplyWindowDouble(int len, double *win, double *data);
void ApplyWindowFloat(int len, float *win, float *data);

#endif // WINDOWING_H
