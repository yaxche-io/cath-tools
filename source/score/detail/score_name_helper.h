/// \file
/// \brief The score_name_helper class header

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

#ifndef SCORE_NAME_HELPER_H_INCLUDED
#define SCORE_NAME_HELPER_H_INCLUDED

#include "common/type_aliases.h"

namespace cath {
	namespace score {
		namespace detail {

			/// \brief TODOCUMENT
			class score_name_helper final {
			private:
				score_name_helper() = delete;

			public:
				static std::string build_short_name(const std::string &,
				                                    const str_vec &);

				static std::string human_friendly_short_name(const std::string &,
				                                             const str_bool_pair_vec &);

				static std::string full_short_name(const std::string &,
				                                   const str_bool_pair_vec &);
			};

		}
	}
}

#endif
