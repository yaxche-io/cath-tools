/// \file
/// \brief The common type_aliases header

/// \copyright
/// Tony Lewis's Common C++ Library Code (here imported into the CATH Binaries project and then tweaked, eg namespaced in cath)
/// Copyright (C) 2007, Tony Lewis
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

#ifndef TYPE_ALIASES_H_INCLUDED
#define TYPE_ALIASES_H_INCLUDED

#include <boost/filesystem/path.hpp>
#include <boost/optional/optional_fwd.hpp>
#include <boost/tuple/tuple.hpp>

#include <deque>
#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>


namespace cath {

	using bool_size_str_tpl             = std::tuple<bool, size_t, std::string>;
	using bool_size_str_tpl_vec         = std::vector<bool_size_str_tpl>;
	using char_str_str_tpl              = std::tuple<char, std::string, std::string>;
	using char_str_str_tpl_vec          = std::vector<char_str_str_tpl>;
	using diff_diff_pair                = std::pair<ptrdiff_t, ptrdiff_t>;
	using size_size_doub_tpl            = std::tuple<size_t, size_t, double>;
	using size_size_doub_tpl_vec        = std::vector<size_size_doub_tpl>;
	using size_size_tpl                 = std::tuple<size_t, size_t>;
	using str_str_str_str_tpl           = std::tuple<std::string, std::string, std::string, std::string>;
	using str_str_str_str_tpl_vec       = std::vector<str_str_str_str_tpl>;

	using size_bool_pair                = std::pair<size_t, bool>;
	using size_bool_pair_vec            = std::vector<size_bool_pair>;

	using str_set                       = std::set<std::string>;

	using str_doub_pair                 = std::pair<std::string, double>;
	using str_doub_pair_vec             = std::vector<str_doub_pair>;

	using str_bool_pair                 = std::pair<std::string, bool>;
	using str_bool_pair_vec             = std::vector<str_bool_pair>;

	using bool_deq                      = std::deque<bool>;
	using bool_deq_itr                  = bool_deq::iterator;
	using bool_deq_citr                 = bool_deq::const_iterator;
	using bool_deq_vec                  = std::vector<bool_deq>;

	using bool_vec                      = std::vector<bool>;     // WARNING: Remember std has a stupid template specialisation for vector<bool>
	using bool_vec_vec                  = std::vector<bool_vec>; // WARNING: Remember std has a stupid template specialisation for vector<bool>

	using char_vec                      = std::vector<char>;

	using doub_vec                      = std::vector<double>;
	using doub_vec_vec                  = std::vector<doub_vec>;

	using diff_vec                      = std::vector<ptrdiff_t>;
	using diff_vec_vec                  = std::vector<diff_vec>;

	using path_vec                      = std::vector<boost::filesystem::path>;

	using opt_size                      = boost::optional<size_t>;
	using opt_size_vec                  = std::vector<boost::optional<size_t> >;

	//using uint_uint_pair                = std::pair<unsigned int, unsigned int>;
	//using uint_uint_pair_vec            = std::vector<uint_uint_pair>;

	using str_str_pair                  = std::pair<std::string, std::string>;
	using str_str_pair_vec              = std::vector<str_str_pair>;

	using str_str_pair_doub_map         = std::map  <str_str_pair, double>;
	using str_str_pair_doub_pair        = std::pair <str_str_pair, double>;

	using str_str_str_pair_map          = std::map<std::string, str_str_pair>;

	using size_size_pair                = std::pair<size_t, size_t>;
	using size_size_pair_vec            = std::vector<size_size_pair>;
	using size_size_pair_doub_map       = std::map<size_size_pair, double>;
	using size_size_pair_doub_map_value = size_size_pair_doub_map::value_type;

	using str_str_map                   = std::map<std::string, std::string>;

	using opt_size_size_pair            = boost::optional<size_size_pair>;

	using str_doub_map                  = std::map <std::string, double>;

	using str_size_pair                 = std::pair<std::string, size_t>;
	using str_size_map                  = std::map <std::string, size_t>;
	using str_size_pair_vec             = std::vector<str_size_pair>;

	using opt_str                       = boost::optional<std::string>;

	using doub_doub_pair                = std::pair<double, double>;
	using doub_doub_pair_vec            = std::vector<doub_doub_pair>;
	using doub_doub_pair_vec_itr        = doub_doub_pair_vec::iterator;
	using doub_doub_pair_vec_citr       = doub_doub_pair_vec::const_iterator;

	using doub_size_pair                = std::pair<double, size_t>;
	using size_doub_pair                = std::pair<size_t, double>;

	//using uint_vec                      = std::vector<unsigned int>;
	//using uint_vec_vec                  = std::vector<uint_vec>;

	using str_vec                       = std::vector<std::string>;
	using str_vec_citr                  = str_vec::const_iterator;
	using str_vec_vec                   = std::vector<str_vec>;

	using str_str_vec_pair              = std::pair<std::string, str_vec>;
	using str_str_vec_map               = std::map <std::string, str_vec>;

	using str_deq                       = std::deque<std::string>;
	using str_deq_citr                  = str_vec::const_iterator;

	using str_size_type                 = std::string::size_type;

	using size_vec                      = std::vector<size_t>;
	using size_vec_vec                  = std::vector<size_vec>;

	using size_vec_size_vec_pair        = std::pair<size_vec, size_vec>;

	using opt_char                      = boost::optional<char>;

	using int_vec                       = std::vector<int>;
	using int_vec_vec                   = std::vector<int_vec>;

	using size_set                      = std::set<size_t>;

	using size_size_vec_pair            = std::pair<size_t, size_vec>;
	using size_size_vec_map             = std::map<size_t, size_vec>;

	using size_size_size_size_tpl       = std::tuple<size_t, size_t, size_t, size_t>;
}

namespace cath {
	class protein;

	using float_score_type             = double;
	using float_score_vec              = std::vector<float_score_type>;
	using float_score_vec_vec          = std::vector<float_score_vec>;
	using float_score_float_score_pair = std::pair<float_score_type, float_score_type>;

	using opt_score                    = boost::optional<double>;
	using opt_score_vec                = std::vector<opt_score>;
	using opt_score_vec_vec            = std::vector<opt_score_vec>;

	using prot_prot_pair               = std::pair<protein, protein>;

	/// \brief The type of the scores used in aligning with dynamic-programming etc
	///
	/// \todo Switch this to double and if that is to broad-brush then templatise on score_type where appropriate
	using score_type                   = ptrdiff_t;
	using score_vec                    = std::vector<score_type>;
	using score_vec_vec                = std::vector<score_vec>;

	using str_str_score_tpl            = std::tuple<std::string, std::string, score_type>;
}

#endif
