/// \file
/// \brief The selection_policy_acquirer class header

/// \copyright
/// CATH Tools - Protein structure comparison tools such as SSAP and SNAP
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

#ifndef SELECTION_POLICY_ACQUIRER_H_INCLUDED
#define SELECTION_POLICY_ACQUIRER_H_INCLUDED

#include <boost/shared_ptr.hpp>

#include "alignment/alignment.h"
#include "common/type_aliases.h"
#include "structure/structure_type_aliases.h"

#include <utility>

namespace cath { namespace align { class common_atom_selection_policy; } }
namespace cath { namespace align { class common_residue_selection_policy; } }
namespace cath { namespace file { class pdb; } }
namespace cath { namespace file { class pdb_list; } }
namespace cath { namespace geom { class coord_list; } }

namespace cath {
	namespace opts {

		/// \brief TODOCUMENT
		class selection_policy_acquirer final {
			/// \brief TODOCUMENT
			std::shared_ptr<align::common_residue_selection_policy> comm_res_seln_pol_ptr;

			/// \brief TODOCUMENT
			std::shared_ptr<align::common_atom_selection_policy> comm_atom_seln_pol_ptr;

			const align::common_residue_selection_policy & get_comm_res_seln_pol_ptr_cref() const;
			const align::common_atom_selection_policy    & get_comm_atom_seln_pol_ptr_cref() const;

		public:
			selection_policy_acquirer(const align::common_residue_selection_policy &,
			                          const align::common_atom_selection_policy &);

			geom::coord_list_coord_list_pair get_common_coords(const align::alignment &,
			                                                   const file::pdb &,
			                                                   const file::pdb &,
			                                                   // const str_veSc &,
			                                                   const size_t &,
			                                                   const size_t &) const;
			std::string get_descriptive_name() const;
		};

		geom::coord_list_coord_list_pair get_common_coords(const selection_policy_acquirer &,
		                                                   const align::alignment &,
		                                                   const file::pdb_list &,
		                                                   // const str_vec &,
		                                                   const size_t &,
		                                                   const size_t &);
	}
}

#endif
