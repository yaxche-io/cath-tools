/// \file
/// \brief The make_tuple_with_skips header

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

#ifndef _CATH_TOOLS_SOURCE_COMMON_TUPLE_MAKE_TUPLE_WITH_SKIPS_H
#define _CATH_TOOLS_SOURCE_COMMON_TUPLE_MAKE_TUPLE_WITH_SKIPS_H

#include <tuple>
#include <type_traits>

namespace cath {
	namespace common {

		/// \brief A type to indicate an element should be skipped in a call to make_tuple_with_skips()
		struct tpl_elmnt_skip_t {
			tpl_elmnt_skip_t            (const tpl_elmnt_skip_t & ) = delete;
			tpl_elmnt_skip_t            (      tpl_elmnt_skip_t &&) = delete;
			tpl_elmnt_skip_t & operator=(const tpl_elmnt_skip_t & ) = delete;
			tpl_elmnt_skip_t & operator=(      tpl_elmnt_skip_t &&) = delete;
		};

		namespace detail {

			/// \brief End-of-recursion implementation function for make_tuple_with_skips()
			template <typename... Ts>
			constexpr auto make_tuple_with_skips_recursive(const std::tuple<Ts...> &arg_ts_tpl ///< The tuple built so far
			                                               ) {
				return arg_ts_tpl;
			}

			// Declaration of function used in make_tuple_with_skips_recursive_tag_dispatch()
			template <typename... Ts, typename U, typename... Vs>
			constexpr auto make_tuple_with_skips_recursive(const std::tuple<Ts...> &,
			                                               U &&,
			                                               Vs &&...);

			/// \brief Tag-distpactch recursive implementation function for make_tuple_with_skips
			template <typename... Ts, typename U, typename... Vs>
			constexpr auto make_tuple_with_skips_recursive_tag_dispatch(const std::true_type     &,           ///< Tag to pick this overload for tpl_elmnt_skip_t U (after decaying U)
			                                                            const std::tuple<Ts...>  &arg_ts_tpl, ///< The tuple built so far
			                                                            U                       &&/*arg_u*/,  ///< The argument currently being processed
			                                                            Vs                      &&...arg_vs   ///< The remaining arguments to process
			                                                            ) {
				return make_tuple_with_skips_recursive(
					arg_ts_tpl,
					std::forward< Vs >( arg_vs )...
				);
			}

			/// \brief Tag-distpactch recursive implementation function for make_tuple_with_skips
			template <typename... Ts, typename U, typename... Vs>
			constexpr auto make_tuple_with_skips_recursive_tag_dispatch(const std::false_type    &,           ///< Tag to pick this overload for non-tpl_elmnt_skip_t U (after decaying U)
			                                                            const std::tuple<Ts...>  &arg_ts_tpl, ///< The tuple built so far
			                                                            U                       &&arg_u,      ///< The argument currently being processed
			                                                            Vs                      &&...arg_vs   ///< The remaining arguments to process
			                                                            ) {
				return make_tuple_with_skips_recursive(
					std::tuple_cat(
						arg_ts_tpl,
						std::make_tuple( std::forward< U  >( arg_u ) )
					),
					std::forward< Vs >( arg_vs )...
				);
			}

			/// \brief Recursive implementation function for make_tuple_with_skips
			template <typename... Ts, typename U, typename... Vs>
			constexpr auto make_tuple_with_skips_recursive(const std::tuple<Ts...>  &arg_ts_tpl, ///< The tuple built so far
			                                               U                       &&arg_u,      ///< The argument currently being processed
			                                               Vs                      &&...arg_vs   ///< The remaining arguments to process
			                                               ) {
				return make_tuple_with_skips_recursive_tag_dispatch(
					std::is_same< std::decay_t<U>, tpl_elmnt_skip_t >{},
					arg_ts_tpl,
					std::forward< U  >( arg_u  ),
					std::forward< Vs >( arg_vs )...
				);
			}

		} // namespace detail

		/// \brief Do the same as std::make_tuple() but skip any elements of type tpl_elmnt_skip_t
		template <typename... Vs>
		constexpr auto make_tuple_with_skips(Vs &&...arg_vs ///< The arguments from which to make the tuple
		                                     ) {
			return detail::make_tuple_with_skips_recursive(
				std::make_tuple(),
				std::forward<Vs>( arg_vs )...
			);
		}

	} // namespace common
} // namespace cath

#endif
