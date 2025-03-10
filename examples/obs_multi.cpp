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
#include <limbo/acqui/gp_ucb.hpp>
#include <limbo/bayes_opt/boptimizer.hpp>
#include <limbo/kernel/matern_five_halves.hpp>
#include <limbo/mean/data.hpp>
#include <limbo/model/gp.hpp>
#include <limbo/tools/macros.hpp>

using namespace limbo;

struct Params {
    struct acqui_gpucb : public defaults::acqui_gpucb {
    };

#ifdef USE_NLOPT
    struct opt_nloptnograd : public defaults::opt_nloptnograd {
    };
#elif defined(USE_LIBCMAES)
    struct opt_cmaes : public defaults::opt_cmaes {
    };
#else
    struct opt_gridsearch : public defaults::opt_gridsearch {
    };
#endif
    struct kernel : public defaults::kernel {
        BO_PARAM(double, noise, 0.001);
    };

    struct kernel_maternfivehalves : defaults::kernel_maternfivehalves{
        BO_PARAM(double, sigma_sq, 1);
        BO_PARAM(double, l, 0.2);
    };

    struct bayes_opt_boptimizer : public defaults::bayes_opt_boptimizer {
        BO_PARAM(bool, stats_enabled, true);
    };

    struct init_randomsampling : defaults::init_randomsampling {
        BO_PARAM(int, samples, 5);
    };

    struct stop_maxiterations : defaults::stop_maxiterations {
        BO_PARAM(int, iterations, 20);
    };
};

struct StateEval {
    BO_PARAM(size_t, dim_in, 2);

    std::tuple<EvaluationStatus, double> operator()(const Eigen::VectorXd& x) const
    {
        Eigen::VectorXd res(2);
        res(0) = 3 * x(0) + 5;
        res(1) = -5 * x(1) + 2;
        return { OK, res.sum() / res.size()};
    }
};

struct Average {
    using result_type = double;
    double operator()(const Eigen::VectorXd& x) const
    {
        return x.sum() / x.size();
    }
};

int main()
{
    using Kernel_t = kernel::MaternFiveHalves<Params::kernel, Params::kernel_maternfivehalves>;
    using Mean_t = mean::Data;
    using GP_t = model::GaussianProcess<Kernel_t, Mean_t>;
    using Acqui_t = acqui::GP_UCB<Params::acqui_gpucb, GP_t>;

    bayes_opt::BOptimizer<Params, GP_t, Acqui_t> opt(2);

    std::cout << "Optimize using  Average aggregator" << std::endl;
    opt.optimize(StateEval());

    auto const& gaussianProcess = opt.model();
    const auto max_e = std::max_element(gaussianProcess.observations().begin(), gaussianProcess.observations().end());
    const auto index = std::distance(gaussianProcess.observations().begin(), max_e);
    const double bestObs = gaussianProcess.observations()[index];
    const Eigen::VectorXd bestSample = gaussianProcess.samples()[index];

    std::cout << "best obs based on Average aggregator: " << bestObs << " res  " << bestSample.transpose() << std::endl;
    return 0;
}
