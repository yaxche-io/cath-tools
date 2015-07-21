/// \file
/// \brief The alignment_refiner class header

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

#ifndef ALIGNMENT_REFINER_H_INCLUDED
#define ALIGNMENT_REFINER_H_INCLUDED

#include "alignment/align_type_aliases.h"
#include "common/type_aliases.h"

namespace cath { namespace align { class alignment; } }
namespace cath { namespace align { namespace detail { class alignment_split; } } }
namespace cath { namespace align { namespace detail { class alignment_split_list; } } }
namespace cath { namespace align { namespace gap { class gap_penalty; } } }
namespace cath { class protein_list; }
namespace cath { namespace index { class view_cache_list; } }

namespace cath {
	namespace align {

		/// \brief TODOCUMENT
		class alignment_refiner final {
		private:
			float_score_vec_vec from_alignment_scores;
			float_score_vec_vec to_alignment_scores;

			detail::bool_aln_pair iterate_step(const alignment &,
			                                   const protein_list &,
			                                   const index::view_cache_list &,
			                                   const gap::gap_penalty &);

			detail::bool_aln_pair iterate_step_for_alignment_split_list(const alignment &,
			                                                            const protein_list &,
			                                                            const index::view_cache_list &,
			                                                            const gap::gap_penalty &,
			                                                            const detail::alignment_split_list &);

			detail::bool_aln_pair iterate_step_for_alignment_split(const alignment &,
			                                                       const protein_list &,
			                                                       const index::view_cache_list &,
			                                                       const gap::gap_penalty &,
			                                                       const detail::alignment_split &);

		public:
			alignment iterate(const alignment &,
			                  const protein_list &,
			                  const gap::gap_penalty &);

			alignment iterate(const alignment &,
			                  const protein_list &,
			                  const index::view_cache_list &,
			                  const gap::gap_penalty &);

		};
	}

}

#endif
