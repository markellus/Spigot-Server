# 
# This file is part of De.Markellus.Math (https://github.com/markellus/Spigot-Server).
# Copyright (c) 2019 Marcel Bulla.
# 
# This program is free software: you can redistribute it and/or modify  
# it under the terms of the GNU General Public License as published by  
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 3.1)
project (Spigot)

set (CMAKE_CXX_STANDARD 17)

set(SpigotFiles
../spigot/consts.cpp ../spigot/erf.cpp ../spigot/exp.cpp ../spigot/expr.cpp ../spigot/gamma.cpp ../spigot/arithmetic.cpp ../spigot/io.cpp
../spigot/iocli.cpp ../spigot/monotone.cpp ../spigot/noexcept.cpp
../spigot/baseout.cpp ../spigot/spigot.cpp ../spigot/sqrt.cpp ../spigot/trig.cpp ../spigot/unary.cpp ../spigot/lambertw.cpp
../spigot/algebraic.cpp ../spigot/expint.cpp ../spigot/trigint.cpp ../spigot/zeta.cpp ../spigot/enforce.cpp ../spigot/misc.cpp
../spigot/cfracout.cpp ../spigot/holefiller.cpp ../spigot/error.cpp ../spigot/hypergeom.cpp
../spigot/bigint.h ../spigot/cr.h ../spigot/error.h ../spigot/expr.h ../spigot/funcs.h ../spigot/baseout.h
../spigot/spigot.h ../spigot/bi_gmp.h ../spigot/bi_internal.h ../spigot/version.h ../spigot/cfracout.h ../spigot/holefiller.h
../spigot/io.h
Library/exports.h Library/exports.cpp)

add_library(spgt-lib SHARED ${SpigotFiles})

add_library(spgt-wrp MODULE MultiWrapper/SpgtInstance.h MultiWrapper/SpgtInstance.cpp MultiWrapper/exports.h MultiWrapper/exports.cpp)

add_executable(spgt-cmd main.cpp)