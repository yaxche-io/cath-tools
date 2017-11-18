/// \file
/// \brief The accumulate_proj header

/// \copyright
/// Tony Lewis's Common C++ Library Code (here imported into the CATH Tools project and then tweaked, eg namespaced in cath)
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

#ifndef _CATH_TOOLS_SOURCE_COMMON_BOOST_ADDENDA_RANGE_ACCUMULATE_PROJ_H
#define _CATH_TOOLS_SOURCE_COMMON_BOOST_ADDENDA_RANGE_ACCUMULATE_PROJ_H

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/numeric.hpp>

#include "common/boost_addenda/range/range_concept_type_aliases.hpp"
#include "common/cpp17/invoke.hpp"
#include "common/function/ident.hpp"

#include <utility>

namespace cath {
	namespace common {

		/// \brief Wrap boost::accumulate() and add support for a projection function, a la Eric Niebler's range-v3 library
		//
		/// \todo Roll this out to simplify code
		template <typename Rng,
		          typename T,
		          typename Op = std::plus<>,
		          typename Proj = ident>
		inline auto accumulate_proj(Rng  &&arg_range,          ///< The range to query
		                            T    &&arg_init,           ///< The initial value from which to start the accumulation
		                            Op   &&arg_op    = Op{},   ///< The operation to apply the value-so-far and new-value
		                            Proj &&arg_proj  = Proj{}  ///< The projection fuction
		                            ) {
			/// \todo Come Clang (>= 3.7?) with fix, drop this type alias and use generic lambdas
			using const_reference_type = common::range_const_reference_t<Rng>;

			return boost::accumulate(
				std::forward<Rng>( arg_range )
					| boost::adaptors::transformed( [&] (const_reference_type x) {
						return invoke( std::forward<Proj>( arg_proj ), x );
					} ),
				std::forward< T  >( arg_init ),
				std::forward< Op >( arg_op   )
			);
		}

	} // namespace common
} // namespace cath

#endif