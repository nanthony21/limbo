#pragma once
#include <limbo/stat/stat_base.hpp>
#include <limbo/tools.hpp>
#include <limbo/serialize/text_archive.hpp>

namespace limbo::stat
{


	struct ModelExport : StatBase
	{
		template <typename BO, typename AggFunc>
		void operator()(BO const& bo, AggFunc const& agg)
		{
			assert(bo.model().dim_in() == 1);
			std::ofstream f(dir_ / (std::to_string(bo.current_iteration()) + ".dat"));
			using acqFuncT = typename std::decay_t<decltype(bo)>::acquisition_function_t;
			acqFuncT acquisitionFunction(bo.model(), bo.current_iteration());
			for (int i = 0; i < 100; ++i) {
				Eigen::VectorXd v = tools::make_vector(i / 99.0);
				auto [mu, sigma_sq] = bo.model().query(v);
				auto [acqVal, grad] = acquisitionFunction(v, agg, false);
				f << v.transpose() << " " << mu[0] << " " << std::sqrt(sigma_sq) << " " << acqVal << std::endl;
			}
			bo.model().save(serialize::TextArchive((dir_ / (std::to_string(bo.current_iteration()) + ".model")).string()));
		}
	};
}