/// \file
/// \brief The entry_querier class definitions

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

#include "entry_querier.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>

using namespace cath;
using namespace std;

constexpr size_t entry_querier::INTEGER_SCALING;

/// \brief TODOCUMENT
///
/// \relates entry_querier
string cath::get_plural_name(const entry_querier &prm_entry_querier ///< The entry_querier to query either residues or secondary structures
                             ) {
	return prm_entry_querier.get_entry_name() + "s";
}

/// \brief TODOCUMENT
///
/// \relates entry_querier
double cath::get_gap_penalty(const entry_querier &prm_entry_querier ///< The entry_querier to query either residues or secondary structures
                             ) {
	return ( prm_entry_querier.get_gap_penalty_ratio() * prm_entry_querier.optimum_single_score() );
}

/// \brief Whether a pair would avoid being excluded under an entry_querier's num_excluded_on_either_size()
///
/// \relates entry_querier
///
/// This operation should be symmetric.
bool cath::pair_is_not_excluded(const entry_querier &prm_entry_querier, ///< The entry_querier defining the num_excluded_on_either_size()
                                const size_t        &prm_index1,        ///< One of the indices
                                const size_t        &prm_index2         ///< The other one of the indices
                                ) {
	return pair_is_not_excluded( prm_entry_querier.num_excluded_on_either_size(), prm_index1, prm_index2 );
}

/// \brief Whether a pair would avoid being excluded under an entry_querier's num_excluded_on_either_size()
///
/// \relates entry_querier
///
/// This operation should be symmetric.
bool cath::pair_is_not_excluded(const size_t &num_excluded_on_either_size, ///< The number excluded on either size
                                const size_t &prm_index1,                  ///< One of the indices
                                const size_t &prm_index2                   ///< The other one of the indices
                                ) {
	const size_t max_index      = max(prm_index1, prm_index2);
	const size_t min_index      = min(prm_index1, prm_index2);
	const size_t abs_index_diff = max_index - min_index;
	return ( abs_index_diff > num_excluded_on_either_size );
}

/// \brief Return the maximum number of comparable pairs there could be in two structures of length
///        prm_length that use prm_entry_querier's num_excluded_on_either_side policy.
///
/// \relates entry_querier
///
/// This just grabs the num_excluded_on_either_size() value from prm_entry_querier
/// and then passes the values on to _num_comparable_impl() to do the calculation.
/// See the documentation for _num_comparable_impl() for more information.
size_t cath::num_comparable(const entry_querier &prm_entry_querier, ///< The entry_querier, which is used to provide the num_excluded_on_either_size()
                            const size_t        &prm_length         ///< The length of the structure
                            ) {
	return _num_comparable_impl(
		prm_entry_querier.num_excluded_on_either_size(),
		prm_length
	);
}

/// \brief Implementation for calculating the maximum number of comparable pairs there could be
///        in two structures of length prm_length when excluding prm_num_excluded on either side.
///
/// \relates entry_querier
///
/// The question is: if two structures of length prm_length were compared, what is the maximum
/// number of compared pairs that would be used in SSAP scoring an alignment?
///
/// If it weren't for exclusions, this would just be prm_length * prm_length because: the alignment would
/// just be the canonical 1-1 alignment and then, for each position in the alignment, the views would
/// be considered from the two aligned residues and the scores would be summed to each of the positions in the
/// alignment.
///
/// Unfortunately, the SSAP score involves excluding the view to some of the closest neighbouring entries.
/// Since this calculation is considering the canonical 1-1 alignment, that can just be interpreted
/// as excluding neighbouring alignment positions.
///
/// To understand how to handle this, it helps to look at some examples showing which alignment
/// positions would be summed for each of the alignment positions. For example,
/// say the length is 15 and pairs that are 5 apart or less are excluded, then the pattern is as
/// follows (where '.' is a pair that's excluded and 'x' is a pair that isn't) :
///
///     . . . . . . x x x x x x x x x
///     . . . . . . . x x x x x x x x
///     . . . . . . . . x x x x x x x
///     . . . . . . . . . x x x x x x
///     . . . . . . . . . . x x x x x
///     . . . . . . . . . . . x x x x
///     x . . . . . . . . . . . x x x
///     x x . . . . . . . . . . . x x
///     x x x . . . . . . . . . . . x
///     x x x x . . . . . . . . . . .
///     x x x x x . . . . . . . . . .
///     x x x x x x . . . . . . . . .
///     x x x x x x x . . . . . . . .
///     x x x x x x x x . . . . . . .
///     x x x x x x x x x . . . . . .
///
/// The aim is to find the number of 'x' symbols. The old code's approach to calculating this was:
///  - start with the area of the square: 15 * 15 = 225
///  - remove the 11 (5 + 1 + 5) dots excluded from each row: 15 * (5 + 1 + 5) = 165
///  - add back in dots that shouldn't have been removed for rows without 11 dots: 2 * (5+4+3+2+1) = 30
/// ...which gives a correct result of 90.
///
/// However this reasoning is a bit complicated and gets trickier if the length is small compared to the number excluded.
/// For example, with length 7 and pairs excluded that are 4 apart or less :
///
///     . . . . . x x
///     . . . . . . x
///     . . . . . . .
///     . . . . . . .
///     . . . . . . .
///     x . . . . . .
///     x x . . . . .
///
/// Now, it becomes trickier to work out the numbers to remove and to add back in.
///
/// A simpler way to reason is to observe that the crosses form two triangles that both have width and height
/// of (length - num_excluded - 1).
/// Using the standard equation \f$ 1 + 2 + \ldots + k = \frac{k(k+1)}{2} \f$, and defining \f$ l \f$ as the length
/// and \f$ n \f$ ad the number excluded on each side, the total is: \f$ 2 * \frac{ (l - n -1)(l - n - 1 + 1) } { 2 } \f$
/// which equals \f$ (l - n) * (l - n - 1) \f$.
size_t cath::_num_comparable_impl(const size_t &prm_num_excluded, ///< The number of residues that are excluded on either side of each residue
                                  const size_t &prm_length        ///< The length of the structure
                                  ) {
	// If the length is too short to accommodate any comparable pairs, just return 0
	// (which avoids strange behaviour on size_ts becoming negative)
	if (prm_length <= prm_num_excluded + 1) {
		return 0;
	}

	// Return the result of the calculation, documented above
	return (prm_length - prm_num_excluded) * (prm_length - prm_num_excluded - 1);
}

