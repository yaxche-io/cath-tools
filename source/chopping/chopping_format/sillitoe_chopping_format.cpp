/// \file
/// \brief The sillitoe_chopping_format class definitions

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

#include "sillitoe_chopping_format.hpp"

#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string/join.hpp>

#include "chopping/domain/domain.hpp"
#include "common/boost_addenda/make_string_ref.hpp"
#include "common/clone/make_uptr_clone.hpp"
#include "common/cpp14/cbegin_cend.hpp"
#include "common/debug_numeric_cast.hpp"
#include "common/exception/invalid_argument_exception.hpp"

#include <cctype>

using namespace cath;
using namespace cath::chop;
using namespace cath::common;
using namespace std::literals::string_literals;

using boost::adaptors::transformed;
using boost::algorithm::join;
using boost::string_ref;
using std::find;
using std::isdigit;
using std::next;
using std::pair;
using std::prev;
using std::string;
using std::unique_ptr;

/// \brief A standard do_clone method.
unique_ptr<chopping_format> sillitoe_chopping_format::do_clone() const {
	return { make_uptr_clone( *this ) };
}

/// \brief Return whether this format represents fragments
bool sillitoe_chopping_format::do_represents_fragments() const {
	return false;
}

/// \brief Parse a domain string to the start of the regions information
///
/// \returns A pair of:
///           * a (possibly empty) string_ref containing any name
///           * an iterator to the start of the regions part of the string
pair<string_ref, str_citr> sillitoe_chopping_format::parse_to_start_of_regions(const string &prm_domain_chopping_string ///< The domain chopping string to parse
                                                                               ) {
	const auto begin_itr = common::cbegin( prm_domain_chopping_string );
	const auto end_itr   = common::cend  ( prm_domain_chopping_string );

	// Check the first character and return if not 'D'
	if ( prm_domain_chopping_string.empty() ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format domain from an empty"));
	}
	if ( prm_domain_chopping_string.front() != 'D' ) {
		return make_pair( string_ref{}, begin_itr );
	}

	// Check the second character and return if not '['
	if ( prm_domain_chopping_string.size() <= 1 ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format domain from a 'D'-prefixed string with no other characters"));
	}
	const auto begin_plus_one_itr = next( begin_itr );
	if ( *begin_plus_one_itr != '[' ) {
		return make_pair( string_ref{}, begin_plus_one_itr );
	}

	// Check the name
	if ( prm_domain_chopping_string.size() <= 2 ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format domain from a 'D['-prefixed string with no other characters"));
	}
	const auto begin_plus_two_itr = next( begin_plus_one_itr );
	const auto end_of_name_itr = find( begin_plus_two_itr, end_itr, ']' );
	if ( end_of_name_itr == end_itr ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format domain from a 'D['-prefixed string that has no ']' character to terminate the name"));
	}
	const auto start_of_regions_itr = next( end_of_name_itr );
	if ( start_of_regions_itr == end_itr ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format domain from a 'D['-prefixed string that has no ']' character to terminate the name"));
	}

	// Return the name and the start of regions part of the string
	return make_pair( make_string_ref( begin_plus_two_itr, end_of_name_itr ), start_of_regions_itr);
}

/// \brief TODOCUMENT
domain sillitoe_chopping_format::do_parse_domain(const string &prm_domain_chopping_string ///< TODOCUMENT
                                                 ) const {
	// Parse up to the start of the regions
	const auto  parsed_start      = parse_to_start_of_regions( prm_domain_chopping_string );
	const auto &name_str_ref      = parsed_start.first;
	const auto &regions_begin_itr = parsed_start.second;
	const auto  end_itr           = common::cend    ( prm_domain_chopping_string );
	

	// Parse each of the regions
	region_vec segments;
	auto region_begin_itr = regions_begin_itr;
	auto region_end_itr   = find( region_begin_itr, end_itr, ',' );
	while ( region_begin_itr != end_itr ) {
		segments.push_back( parse_segment( make_string_ref( region_begin_itr, region_end_itr ) ) );
		region_begin_itr = ( region_end_itr == end_itr ) ? region_end_itr : next( region_end_itr );
		region_end_itr   = find( region_begin_itr, end_itr, ',' );
	}

	// Return a domain with the parsed segments and the name if one was parsed
	return name_str_ref.empty()
		? domain{ segments }
		: domain{ segments, name_str_ref.to_string() };
}

/// \brief Concrete definition of this chopping_format writes a region to a string
string sillitoe_chopping_format::do_write_region(const region &prm_region ///< The region to write to a string
                                                 ) const {
	const chain_label_opt &opt_chain_label = prm_region.get_opt_chain_label();

	return
		(
			prm_region.has_starts_stops()
			? (
				  to_string( get_residue_name( prm_region.get_start_residue() ) )
				+ "-"
				+ to_string( get_residue_name( prm_region.get_stop_residue()  ) )
			)
			: ""s
		)
		+ (
			opt_chain_label
			? ( ":" + opt_chain_label->to_string() )
			: ""s
		);
}

/// \brief Concrete definition of this chopping_format writes a domain to a string
string sillitoe_chopping_format::do_write_domain(const domain &prm_domain ///< The domain to write to a string
                                                 ) const {
	return
		"D"
		+ (
			prm_domain.get_opt_domain_id()
			? ( "[" + *prm_domain.get_opt_domain_id() + "]" )
			: ""s
		)
		+ join(
			prm_domain
				| transformed( [&] (const region &x) { return write_region( x ); } ),
			","
		);
}

/// \brief Parse a segment from the specified segment string
///
/// Example valid inputs: "7-232:K", "1B-99C:S"
region sillitoe_chopping_format::parse_segment(const string_ref &prm_segment_string ///< The string from which to parse the segment
                                               ) const {
	constexpr char   CHAIN_DELIM_COLON      = ':';
	constexpr char   RESIDUE_NAME_DELIM     = '-';
	constexpr size_t CHAIN_DELIM_NEG_OFFSET = 2;

	const auto length = prm_segment_string.length();
	if ( length < 2 ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format segment from a string of fewer than two characters"));
	}
	if ( prm_segment_string[ length - CHAIN_DELIM_NEG_OFFSET ] != CHAIN_DELIM_COLON ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format segment from a string that doesn't have a colon in the penultimate character"));
	}

	const chain_label the_chain_label{ prm_segment_string.back() };

	if ( length == 2 ) {
		/// \todo Come C++17, if Herb Sutter has gotten his way (n4029), just use braced list here
		return region{ the_chain_label };
	}

	const auto begin_itr          = common::cbegin( prm_segment_string );
	const auto end_itr            = common::cend  ( prm_segment_string );
	const auto begin_plus_one_itr = next( begin_itr );
	const auto res_end_itr        = next( end_itr, 0 - static_cast<int>( CHAIN_DELIM_NEG_OFFSET ) );
	const auto dash_itr           = find( begin_plus_one_itr, res_end_itr, RESIDUE_NAME_DELIM );

	if ( dash_itr == res_end_itr ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format segment from a string with no dash separating residue names"));
	}

	const auto dash_plus_one_itr = next( dash_itr );

	return {
		the_chain_label,
		parse_residue( make_string_ref( begin_itr,         dash_itr    ) ),
		parse_residue( make_string_ref( dash_plus_one_itr, res_end_itr ) )
	};
}

/// \brief Parse a residue from the specified residue string
///
/// Example valid inputs: "232", "99C"
residue_name sillitoe_chopping_format::parse_residue(const string_ref &prm_string_ref ///< The string from which to parse the residue
                                                     ) const {
	if ( prm_string_ref.empty() ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format residue from an empty string"));
	}
	const auto begin_itr   = common::cbegin( prm_string_ref );
	const auto end_itr     = common::cend  ( prm_string_ref );
	if ( isdigit( prm_string_ref.back() ) != 0 ) {
		/// \todo Come C++17, if Herb Sutter has gotten his way (n4029), just use braced list here
		return residue_name{ stoi( string{ begin_itr, end_itr } ) };
	}
	if ( prm_string_ref.length() <= 1 ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot parse sillitoe-chopping-format residue from a string containing a single, non-numeric character"));
	}
	return {
		stoi( string{ begin_itr, prev( end_itr ) } ),
		prm_string_ref.back()
	};
}
