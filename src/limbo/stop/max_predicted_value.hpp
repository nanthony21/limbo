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
#ifndef LIMBO_STOP_MAX_PREDICTED_VALUE_HPP
#define LIMBO_STOP_MAX_PREDICTED_VALUE_HPP

#include <iostream>

#include <Eigen/Core>
#include <spdlog/fmt/fmt.h>
#include <limbo/tools/macros.hpp>
#include <limbo/tools/random_generator.hpp>

namespace limbo {
    namespace defaults {
        struct stop_maxpredictedvalue {
            ///@ingroup stop_defaults
            BO_PARAM(double, ratio, 0.9);
            BO_PARAM(bool, enabled, true);
        };
    }
    namespace stop {

        ///@ingroup stop
        ///Stop once the value for the best sample is above : ratio * (best value predicted by the model)
        ///
        ///Parameter: double ratio
        template <typename stop_maxpredictedvalue>
        struct MaxPredictedValue {

            MaxPredictedValue() {}

            template <concepts::BayesOptimizer BO>
            bool operator()(const BO& bo, std::string& stopMessage) const
            {
                // Prevent instantiation of GPMean if there are no observed samples
                if (bo.observations().size() == 0 || !stop_maxpredictedvalue::enabled())
                    return false;

                std::optional<std::vector<std::pair<double, double>>> parameterBounds = std::nullopt;
                if (bo.isBounded()) {
                    parameterBounds = std::vector<std::pair<double, double>>(bo.dim_in(), std::make_pair(0.0, 1.0));
                }
                Eigen::VectorXd starting_point = tools::random_vector(bo.model().dim_in());
                auto model_optimization = [&](const Eigen::VectorXd& x, bool g) { return opt::no_grad(afun(bo.model().mu(x))); };
                auto x = bo.optimizeFunction(model_optimization, starting_point, parameterBounds);
                double val = afun(bo.model().mu(x));

                if (afun(bo.best_observation()) <= stop_maxpredictedvalue::ratio() * val)
                    return false;
                else {
                	stopMessage = fmt::format("Stop caused by Max predicted value reached. Threshold: {} Max Observation: {}", stop_maxpredictedvalue::ratio() * val, bo.best_observation());
                    return true;
                }
            }
        };
    }
}

#endif
