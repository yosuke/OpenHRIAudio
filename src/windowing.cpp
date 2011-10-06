// -*- C++ -*-
/*!
 * @file  windowing.cpp
 * @author Yosuke Matsusaka <yosuke.matsusaka@aist.go.jp> and OpenHRI development team
 *
 * Copyright (C) 2010
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * @date $Date$
 *
 * $Id$
 */

#include "windowing.h"

double *CreateWindowDouble(int len, enum WindowType type)
{
  double winValue = 0.0;
  double x;
  double *data;
  data = (double *)malloc(sizeof(double) * len);
  double dlen = (double)(len - 1);

  for (int i = 0; i < len; i++) {
    switch (type) {
    case Hamming:
      winValue = 0.54 - 0.46 * cos(2.0 * M_PI * i / dlen);
      break;
    case Hanning:
      winValue = 0.5 - 0.5 * cos(2.0 * M_PI * i / dlen);
      break;
    case Blackman:
      winValue = 0.42 - 0.5 * cos(2.0 * M_PI * i / dlen) + 0.08 * cos(4.0 * M_PI * i / dlen);
      break;
    case Gauss:
      x = (double)i - dlen / 2.0;
      winValue = exp(-2.0 * x * x / dlen);
      break;
    default:
      //! rectangular window
      winValue = 1.0;
    }
    data[i] = winValue;
  }
  return data;
}

float *CreateWindowFloat(int len, enum WindowType type)
{
  float winValue = 0.0;
  double x;
  float *data;
  data = (float *)malloc(sizeof(float) * len);
  double dlen = (double)(len - 1);

  for (int i = 0; i < len; i++) {
    switch (type) {
    case Hamming:
      winValue = (float)(0.54 - 0.46 * cos(2.0 * M_PI * i / dlen));
      break;
    case Hanning:
      winValue = (float)(0.5 - 0.5 * cos(2.0 * M_PI * i / dlen));
      break;
    case Blackman:
      winValue = (float)(0.42 - 0.5 * cos(2.0 * M_PI * i / dlen) + 0.08 * cos(4.0 * M_PI * i / dlen));
      break;
    case Gauss:
      x = (double)i - dlen / 2.0;
      winValue = (float)exp(-2.0 * x * x / dlen);
      break;
    default:
      //! rectangular window
      winValue = 1.0;
    }
    data[i] = winValue;
  }
  return data;
}

double *CreateGaussWindowDouble(int len, int samplef, int centerf)
{
  double winValue = 0.0;
  double x;
  double *data;
  data = (double *)malloc(sizeof(double) * len);

  double df = (double)(samplef / len);
  for (int i = 0; i < len; i++) {
    x = (double)( i * df - centerf);
    winValue = exp(-1 * x * x / (samplef*df) );
    data[i] = winValue;
  }
  return data;
}

float *CreateGaussWindowFloat(int len, int samplef, int centerf)
{
  float winValue = 0.0;
  double x;
  float *data;
  data = (float *)malloc(sizeof(float) * len);

  double df = (double)(samplef / len);
  for (int i = 0; i < len; i++) {
    x = (double)( i * df - centerf);
    winValue = (float)exp(-1 * x * x / (samplef*df) );
    data[i] = winValue;
  }
  return data;
}

void ApplyWindowDouble(int len, double *win, double *data)
{
  for (int i = 0; i < len; i++)
    data[i] *= win[i];
}

void ApplyWindowFloat(int len, float *win, float *data)
{
  for (int i = 0; i < len; i++)
    data[i] *= win[i];
}
