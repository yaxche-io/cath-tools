/// \file
/// \brief The score_name_helper class definitions

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

#include "score_name_helper.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/range/adaptor/filtered.hpp>

#include "common/algorithm/transform_build.h"
#include "exception/out_of_range_exception.h"

using namespace cath::common;
using namespace cath::score::detail;
using namespace std;

using boost::adaptors::filtered;
using boost::algorithm::is_space;
using boost::algorithm::join;

/// \brief TODOCUMENT
string score_name_helper::build_short_name(const string  &arg_id_name, ///< TODOCUMENT
                                           const str_vec &arg_suffixes ///< TODOCUMENT
                                           ) {
	// Sanity check that the id_name isn't empty
	if ( arg_id_name.empty() ) {
		BOOST_THROW_EXCEPTION(out_of_range_exception("A score's id_name must not be empty"));
	}
	// Sanity check that the id_name contains no space characters
	if ( ! all( arg_id_name, ! is_space() ) ) {
		BOOST_THROW_EXCEPTION(out_of_range_exception(
			"A score's id_name mustn't contain any space characters (name was \""
			+ arg_id_name
			+ "\""
		));
	}
	// Sanity check that the suffixes contain no space characters
	for (const string &suffix : arg_suffixes) {
		if ( ! all( suffix, ! is_space() ) ) {
			BOOST_THROW_EXCEPTION(out_of_range_exception(
				"A score's short name suffix mustn't contain any space characters (name was \""
				+ suffix
				+ "\""
			));
		}
	}

	// Return the id_name plus any suffixes separated by full stops
	const auto suffix_string = arg_suffixes.empty() ? string()
	                                                : ( "." + join( arg_suffixes, "." ) );
	return arg_id_name + suffix_string;
}

/// \brief TODOCUMENT
///
/// \post The returned string will be non-empty and will contain no spaces
///       (else an out_of_range_exception will be thrown)
string score_name_helper::human_friendly_short_name(const string            &arg_id_name, ///< TODOCUMENT
                                                    const str_bool_pair_vec &arg_suffixes ///< TODOCUMENT
                                                    ) {
	const auto fitered_suffixes = transform_build<str_vec>(
		arg_suffixes | filtered( [] (const str_bool_pair &x) { return x.second; } ),
		[] (const str_bool_pair &x) { return x.first; }
	);
	return build_short_name( arg_id_name, fitered_suffixes );
}

/// \brief TODOCUMENT
///
/// \post The returned string will be non-empty and will contain no spaces
///       (else an out_of_range_exception will be thrown)
string score_name_helper::full_short_name(const string            &arg_id_name, ///< TODOCUMENT
                                          const str_bool_pair_vec &arg_suffixes ///< TODOCUMENT
                                          ) {
	const auto all_suffixes = transform_build<str_vec>(
		arg_suffixes,
		[] (const str_bool_pair &x) { return x.first; }
	);
	return build_short_name( arg_id_name, all_suffixes );
}
