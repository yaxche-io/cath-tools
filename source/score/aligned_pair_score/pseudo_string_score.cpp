/// \file
/// \brief The pseudo_string_score class definitions

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

#include "pseudo_string_score.h"

//#include <boost/lexical_cast.hpp>
//#include <boost/logic/tribool.hpp>
//#include <boost/range/numeric.hpp>
//#include <boost/numeric/conversion/cast.hpp>
//#include <boost/range/join.hpp>

//#include "alignment/alignment.h"
//#include "alignment/common_atom_selection_policy/common_atom_selection_policy.h"
//#include "alignment/common_residue_selection_policy/common_residue_selection_policy.h"
//#include "common/algorithm/copy_build.h"
#include "common/clone/make_uptr_clone.h"
//#include "common/difference.h"
#include "common/less_than_helper.h"
//#include "common/size_t_literal.h"
#include "exception/invalid_argument_exception.h"
#include "exception/not_implemented_exception.h"
//#include "exception/out_of_range_exception.h"
//#include "structure/geometry/coord.h"
//#include "structure/geometry/coord_list.h"

//#include <iostream> // ***** TEMPORARY *****
//#include <numeric>

using namespace boost::logic;
using namespace cath;
using namespace cath::align;
using namespace cath::common;
//using namespace cath::geom;
using namespace cath::score;
//using namespace cath::score::detail;
using namespace std;

//using boost::accumulate;
//using boost::lexical_cast;
//using boost::none;
//using boost::numeric_cast;
//using boost::range::join;

//BOOST_CLASS_EXPORT(pseudo_string_score)

/// \brief A standard do_clone method.
unique_ptr<aligned_pair_score> pseudo_string_score::do_clone() const {
	return { make_uptr_clone( *this ) };
}

/// \brief Concrete implementation that records that a lower SAS is generally better
tribool pseudo_string_score::do_higher_is_better() const {
	return higher_is_better_value;
}

/// \brief Concrete implementation for calculating the SAS of an alignment
score_value pseudo_string_score::do_calculate(const alignment &,
                                              const protein   &,
                                              const protein   &
                                              ) const {
	BOOST_THROW_EXCEPTION(invalid_argument_exception("Unable to calculate for pseudo_string_scores"));
}

/// \brief Concrete implementation that describes what this score means
string pseudo_string_score::do_description() const {
	return "This is a pseudo_string_score that is being used to represent the score" + score_name;
}

/// \brief TODOCUMENT
string pseudo_string_score::do_id_name() const {
	return score_name;
}

/// \brief TODOCUMENT
str_bool_pair_vec pseudo_string_score::do_short_name_suffixes() const {
	return {};
}

/// \brief Concrete implementation providing long name
string pseudo_string_score::do_long_name() const {
	return "pseudo_string_score["
	       + id_name()
	       + "";
}

/// \brief Concrete implementation for providing a reference to a publication describing this score
string pseudo_string_score::do_reference() const {
	return "There is no reference for a pseudo_string_score";
}

/// \brief TODOCUMENT
bool pseudo_string_score::do_less_than_with_same_dynamic_type(const aligned_pair_score &arg_aligned_pair_score ///< TODOCUMENT
                                                     ) const {
	const auto &casted_aligned_pair_score = dynamic_cast< decltype( *this ) >( arg_aligned_pair_score );
	return ( *this < casted_aligned_pair_score );
}

/// \brief Ctor for pseudo_string_score that uses the defaults for score_common_coord_handler (selecting CA atoms for all aligned residues)
pseudo_string_score::pseudo_string_score(const string  &arg_score_name,      ///< TODOCUMENT
                                         const tribool &arg_higher_is_better ///< TODOCUMENT
                                         ) : score_name            ( arg_score_name       ),
                                             higher_is_better_value( arg_higher_is_better ) {
}

/// \brief TODOCUMENT
///
/// \relates pseudo_string_score
bool cath::score::operator<(const pseudo_string_score &arg_pseudo_string_score_a, ///< TODOCUMENT
                            const pseudo_string_score &arg_pseudo_string_score_b  ///< TODOCUMENT
                            ) {
	auto the_helper = make_less_than_helper( arg_pseudo_string_score_a, arg_pseudo_string_score_b );
	the_helper.register_comparison_field( &pseudo_string_score::id_name          );
	the_helper.register_comparison_field( [] (const pseudo_string_score &x) { return static_cast<bool>( x.higher_is_better() ); } );
	the_helper.register_comparison_field( [] (const pseudo_string_score &x) { return indeterminate    ( x.higher_is_better() ); } );
	return final_less_than_result( the_helper );
}
