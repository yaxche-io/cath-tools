/// \file
/// \brief The classn_stat_plotter_spec class header

/// \copyright
/// CATH Binaries - Protein structure comparison tools such as SSAP and SNAP
/// Copyright (C) 2011, Orengo Group, University College London
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef CLASSN_STAT_PLOTTER_SPEC_H_INCLUDED
#define CLASSN_STAT_PLOTTER_SPEC_H_INCLUDED

#include <boost/optional.hpp>

#include "common/type_aliases.h"

namespace cath { namespace score { class classn_stat_pair_series_list; } }

namespace cath {
	namespace score {

		/// \brief TODOCUMENT
		///
		/// \todo Consider allowing this to be configured via named parameters the Boost Parameter Library
		class classn_stat_plotter_spec final {
		private:
			/// \brief TODOCUMENT
			str_vec pre_plot_strs;

			/// \brief TODOCUMENT
//			boost::optional<size_bool_pair_vec> series_to_plot;
			std::vector<std::pair<std::string, opt_str>> series_to_plot;

			/// \brief TODOCUMENT
			bool tidy_up_score_based_legends = true;

		public:
			classn_stat_plotter_spec(const str_vec &,
			                         const std::vector<std::pair<std::string, opt_str>> &,
			                         const bool &);

			const str_vec & get_pre_plot_strs() const;
			const std::vector<std::pair<std::string, opt_str>> & get_series_to_plot() const;
			const bool & get_tidy_up_score_based_legends() const;
		};

		std::vector<std::pair<std::string, opt_str>> get_series_to_plot_or_make_default(const classn_stat_plotter_spec &,
		                                                                                const classn_stat_pair_series_list &);

		classn_stat_plotter_spec make_standard_score_roc_plotter_spec(const std::vector<std::pair<std::string, opt_str>> &);
		classn_stat_plotter_spec make_standard_score_precision_recall_plotter_spec(const std::vector<std::pair<std::string, opt_str>> &);

	}
}

#endif
