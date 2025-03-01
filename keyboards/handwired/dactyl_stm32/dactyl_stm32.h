/* Copyright 2019
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "quantum.h"

#define LAYOUT_dactyl(                                                  \
                                                                        \
    k00,k01,k02,k03,k04,k05,                                            \
    k10,k11,k12,k13,k14,k15,                                            \
    k20,k21,k22,k23,k24,k25,                                            \
    k30,k31,k32,k33,k34,k35,                                            \
    k40,k41,k42,k43,k44,                                                \
                            k55,k50,                                    \
                                k54,                                    \
                        k53,k52,k51,                                    \
                                                                        \
            k06,k07,k08,k09,k0A,k0B,                                    \
            k16,k17,k18,k19,k1A,k1B,                                    \
            k26,k27,k28,k29,k2A,k2B,                                    \
            k36,k37,k38,k39,k3A,k3B,                                    \
                k47,k48,k49,k4A,k4B,                                    \
    k5B,k56,                                                            \
    k57,                                                                \
    k5A,k59,k58 )                                                       \
                                                                        \
   /* matrix positions */                                               \
   {                                                                    \
    { k00, k01, k02, k03, k04, k05 },                                   \
    { k10, k11, k12, k13, k14, k15 },                                   \
    { k20, k21, k22, k23, k24, k25 },                                   \
    { k30, k31, k32, k33, k34, k35 },                                   \
    { k40, k41, k42, k43, k44, KC_NO },                                 \
    { k50, k51, k52, k53, k54, k55 },                                   \
    { k06, k07, k08, k09, k0A, k0B },                                   \
    { k16, k17, k18, k19, k1A, k1B },                                   \
    { k26, k27, k28, k29, k2A, k2B },                                   \
    { k36, k37, k38, k39, k3A, k3B },					\
    { KC_NO, k47, k48, k49, k4A, k4B },					\
    { k56, k57, k58, k59, k5A, k5B },					\
   }
