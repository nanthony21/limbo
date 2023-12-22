//| Copyright Inria May 2015
//| This project has received funding from the European Research Council (ERC) under
//| the European Union's Horizon 2020 research and innovation programme (grant
//| agreement No 637972) - see http://www.resibots.eu
//|
//| Contributor(s):
//|   - Jean-Baptiste Mouret (jean-baptiste.mouret@inria.fr)
//|   - Antoine Cully (antoinecully@gmail.com)
//|   - Konstantinos Chatzilygeroudis (konstantinos.chatzilygeroudis@inria.fr)
//|   - Federico Allocati (fede.allocati@gmail.com)
//|   - Vaios Papaspyros (b.papaspyros@gmail.com)
//|   - Roberto Rama (bertoski@gmail.com)
//|
//| This software is a computer library whose purpose is to optimize continuous,
//| black-box functions. It mainly implements Gaussian processes and Bayesian
//| optimization.
//| Main repository: http://github.com/resibots/limbo
//| Documentation: http://www.resibots.eu/limbo
//|
//| This software is governed by the CeCILL-C license under French law and
//| abiding by the rules of distribution of free software.  You can  use,
//| modify and/ or redistribute the software under the terms of the CeCILL-C
//| license as circulated by CEA, CNRS and INRIA at the following URL
//| "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and  rights to copy,
//| modify and redistribute granted by the license, users are provided only
//| with a limited warranty  and the software's author,  the holder of the
//| economic rights,  and the successive licensors  have only  limited
//| liability.
//|
//| In this respect, the user's attention is drawn to the risks associated
//| with loading,  using,  modifying and/or developing or reproducing the
//| software by the user in light of its specific status of free software,
//| that may mean  that it is complicated to manipulate,  and  that  also
//| therefore means  that it is reserved for developers  and  experienced
//| professionals having in-depth computer knowledge. Users are therefore
//| encouraged to load and test the software's suitability as regards their
//| requirements in conditions enabling the security of their systems and/or
//| data to be ensured and,  more generally, to use and operate it in the
//| same conditions as regards security.
//|
//| The fact that you are presently reading this means that you have had
//| knowledge of the CeCILL-C license and that you accept its terms.
//|
#ifndef LIMBO_HPP
#define LIMBO_HPP

#ifdef EIGEN_CORE_H
#error Must include this file before including Eigen
#endif

#ifdef LIMBO_USE_INTEL_MKL
// #ifndef EIGEN_USE_MKL_ALL
// #error If using the LIMBO_USE_INTEL_MKL option then you must also define EIGEN_USE_MKL_ALL in any source file before including limbo and Eigen. This is compile definition is not included with the CMAKE target since it can lead to slow compile times.
// #endif
#define EIGEN_USE_MKL_ALL
#endif
#ifdef LIMBO_USE_AOCL
// This must be included before including Eigen. In many cases it isn't
// actually needed, but if building with OpenBlas or AOCL on MSVC then compilation will fail
// without these typedefs


#if defined(_MSC_VER) && !defined(__clang__)
#include <complex.h>
#define LAPACK_COMPLEX_CUSTOM
using lapack_complex_float = std::_Fcomplex_value;
using lapack_complex_double = std::_Dcomplex_value;
#endif
#define EIGEN_USE_BLAS
#define EIGEN_USE_LAPACKE
// #ifndef EIGEN_USE_BLAS
// #error If using the LIMBO_USE_AOCL option then you must also define EIGEN_USE_BLAS in any source file before including limbo and Eigen. This is compile definition is not included with the CMAKE target since it can lead to slow compile times.
// #endif
// #ifndef EIGEN_USE_LAPACKE
// #error If using the LIMBO_USE_AOCL option then you must also define EIGEN_USE_LAPACKE in any source file before including limbo and Eigen. This is compile definition is not included with the CMAKE target since it can lead to slow compile times.
// #endif
#endif


#include <limbo/acqui.hpp>
#include <limbo/bayes_opt/boptimizer.hpp>
#include <limbo/init.hpp>
#include <limbo/kernel.hpp>
#include <limbo/mean.hpp>
#include <limbo/model.hpp>
#include <limbo/opt.hpp>
#include <limbo/stat.hpp>
#include <limbo/stop.hpp>
#include <limbo/tools.hpp>

#endif
