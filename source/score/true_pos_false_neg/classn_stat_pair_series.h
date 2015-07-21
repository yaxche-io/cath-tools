/// \file
/// \brief The classn_stat_pair_series class header

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

#ifndef CLASSN_STAT_PAIR_SERIES_H_INCLUDED
#define CLASSN_STAT_PAIR_SERIES_H_INCLUDED

#include "score/true_pos_false_neg/classn_stat.h"

namespace cath {
	namespace score {
		class classn_stat_plotter;

		/// \brief TODOCUMENT
		class classn_stat_pair_series final {
		private:
			friend class classn_stat_plotter;

			/// \brief TODOCUMENT
			doub_doub_pair_vec data;

			/// \brief Name for this series of data
			///
			/// This will often be the algorithm or scoring-scheme that generated the scores
			std::string name;

		public:
			using const_iterator = doub_doub_pair_vec_citr;

			explicit classn_stat_pair_series(const doub_doub_pair_vec &,
			                                 const std::string &);

			bool empty() const;
			size_t size() const;

			const doub_doub_pair & operator[](const size_t &) const;

			const_iterator begin() const;
			const_iterator end() const;

//			const doub_doub_pair_vec & get_doub_doub_data() const;

			const std::string & get_name() const;
		};

		double area_under_curve(const classn_stat_pair_series &);
	}
}

#endif
