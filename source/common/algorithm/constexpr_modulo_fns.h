/// \file
/// \brief The constexpr_modulo_fns header

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

#ifndef CONSTEXPR_MODULO_FNS_H_INCLUDED
#define CONSTEXPR_MODULO_FNS_H_INCLUDED

#include "common/algorithm/constexpr_integer_rounding.h"
#include "common/c++14/constexpr_min_max.h"
#include "common/type_aliases.h"

#include <type_traits>
	
namespace cath {
	namespace common {
		template <typename T>
		inline constexpr T constexpr_gcd(T, T);
		template <typename T>
		inline constexpr T constexpr_lcm(T, T);

		namespace detail {

			/// \brief TODOCUMENT
			template <typename T, typename U>
			inline constexpr std::pair<U, T> flip_pair(const std::pair<T, U> &arg_pair ///< TODOCUMENT
			                                           ) {
				return std::make_pair( arg_pair.second, arg_pair.first );
			}

			/// \brief TODOCUMENT
			///
			/// \pre arg_r_i >= arg_r_i_plus_one
			template <typename T, typename U>
			inline constexpr std::pair<U, U> extended_euclid_algo_impl(const T &arg_r_i,          ///< TODOCUMENT
			                                                           const U &arg_s_i,          ///< TODOCUMENT
			                                                           const U &arg_t_i,          ///< TODOCUMENT
			                                                           const T &arg_r_i_plus_one, ///< TODOCUMENT
			                                                           const U &arg_s_i_plus_one, ///< TODOCUMENT
			                                                           const U &arg_t_i_plus_one  ///< TODOCUMENT
			                                                           ) {
				static_assert( std::is_unsigned<T>::value, "constexpr_gcd() must be performed on an unsigned integral type" );
				return ( arg_r_i_plus_one == 0 ) ? throw("Cannot perform extended_euclid_algo_impl with a 0") :
				       ( arg_r_i_plus_one == 1 ) ? std::make_pair( arg_s_i_plus_one, arg_t_i_plus_one ) :
				                                   extended_euclid_algo_impl(
				                                   	arg_r_i_plus_one,
				                                   	arg_s_i_plus_one,
				                                   	arg_t_i_plus_one,
				                                   	static_cast<T>( arg_r_i % arg_r_i_plus_one ),
				                                   	static_cast<U>( arg_s_i - ( arg_s_i_plus_one * static_cast<U>( arg_r_i / arg_r_i_plus_one) ) ),
				                                   	static_cast<U>( arg_t_i - ( arg_t_i_plus_one * static_cast<U>( arg_r_i / arg_r_i_plus_one) ) )
				                                   );
			}

			/// \brief TODOCUMENT
			template <typename T>
			inline constexpr auto extended_euclid_algo(const T &arg_a, ///< TODOCUMENT
			                                           const T &arg_b  ///< TODOCUMENT
			                                           ) {
				return ( arg_a >= arg_b )
				? extended_euclid_algo_impl(
					arg_a, static_cast<std::make_signed_t<T>>( 1 ), static_cast<std::make_signed_t<T>>( 0 ),
					arg_b, static_cast<std::make_signed_t<T>>( 0 ), static_cast<std::make_signed_t<T>>( 1 )
				)
				: flip_pair( extended_euclid_algo_impl(
					arg_b, static_cast<std::make_signed_t<T>>( 1 ), static_cast<std::make_signed_t<T>>( 0 ),
					arg_a, static_cast<std::make_signed_t<T>>( 0 ), static_cast<std::make_signed_t<T>>( 1 )
				) );
			}

			/// \brief TODOCUMENT
			template <typename T>
			inline constexpr auto extended_euclid_algo_products(const T &arg_a, ///< TODOCUMENT
			                                                    const T &arg_b  ///< TODOCUMENT
			                                                    ) {
				return std::make_pair(
					detail::extended_euclid_algo( arg_a, arg_b ).first  * static_cast<std::make_signed_t<T>>( arg_a ),
					detail::extended_euclid_algo( arg_a, arg_b ).second * static_cast<std::make_signed_t<T>>( arg_b )
				);
			}

			/// \brief TODOCUMENT
			template <typename T>
			inline constexpr std::make_signed_t<T> chinese_remainder_coprime_pair_num(const T &arg_index_a, ///< TODOCUMENT
			                                                                          const T &arg_index_b, ///< TODOCUMENT
			                                                                          const T &arg_mod_a,   ///< TODOCUMENT
			                                                                          const T &arg_mod_b    ///< TODOCUMENT
			                                                                          ) {
				return (
					( detail::extended_euclid_algo_products( arg_mod_a, arg_mod_b ).first  * static_cast<std::make_signed_t<T>>( arg_index_a ) )
					+
					( detail::extended_euclid_algo_products( arg_mod_a, arg_mod_b ).second * static_cast<std::make_signed_t<T>>( arg_index_b ) )
				) % static_cast<std::make_signed_t<T>>( constexpr_lcm( arg_mod_a, arg_mod_b ) );
			}

			/// \brief TODOCUMENT
			template <typename T>
			inline constexpr T chinese_remainder_coprime_pair_num_above(const T &arg_index_a, ///< TODOCUMENT
			                                                            const T &arg_index_b, ///< TODOCUMENT
			                                                            const T &arg_mod_a,   ///< TODOCUMENT
			                                                            const T &arg_mod_b    ///< TODOCUMENT
			                                                            ) {
				return
					(
						static_cast<std::make_signed_t<T>>( arg_index_a + arg_index_b )
						>
						chinese_remainder_coprime_pair_num( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b )
					)
					? static_cast<T>(
						chinese_remainder_coprime_pair_num( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b )
						+
						(
							round_up_mod(
								static_cast<std::make_signed_t<T>>( arg_index_a + arg_index_b )
								-
								chinese_remainder_coprime_pair_num( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b ),
								static_cast<std::make_signed_t<T>>( constexpr_lcm( arg_mod_a, arg_mod_b ) )
							)
						)
					)
					: static_cast<T>(
						chinese_remainder_coprime_pair_num( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b )
					);
			}

//			/// \brief TODOCUMENT
//			template <typename T>
//			inline constexpr T chinese_remainder_coprime_pair_num_above(const T &arg_index_a, ///< TODOCUMENT
//			                                                            const T &arg_index_b, ///< TODOCUMENT
//			                                                            const T &arg_mod_a,   ///< TODOCUMENT
//			                                                            const T &arg_mod_b    ///< TODOCUMENT
//			                                                            ) {
//				return
//					(
//						static_cast<std::make_signed_t<T>>( arg_index_a + arg_index_b )
//						>
//						chinese_remainder_coprime_pair_num( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b )
//					)
//					? static_cast<T>(
//						chinese_remainder_coprime_pair_num( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b )
//						+
//						(
//							static_cast<std::make_signed_t<T>>( constexpr_lcm( arg_mod_a, arg_mod_b ) )
//							*
//							(
//								static_cast<std::make_signed_t<T>>( 1 )
//								+
//								(
//									(
//										static_cast<std::make_signed_t<T>>( arg_index_a + arg_index_b )
//										-
//										chinese_remainder_coprime_pair_num( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b )
//									)
//									/
//									static_cast<std::make_signed_t<T>>( constexpr_lcm( arg_mod_a, arg_mod_b ) )
//								)
//							)
//						)
//					)
//					: static_cast<T>(
//						chinese_remainder_coprime_pair_num( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b )
//					);
//			}

			/// \brief TODOCUMENT
			template <typename T>
			inline constexpr std::pair<T, T> chinese_remainder_coprime_pair(const T &arg_index_a, ///< TODOCUMENT
			                                                                const T &arg_index_b, ///< TODOCUMENT
			                                                                const T &arg_mod_a,   ///< TODOCUMENT
			                                                                const T &arg_mod_b    ///< TODOCUMENT
			                                                                ) {
				return std::make_pair(
					chinese_remainder_coprime_pair_num_above( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b ) - arg_index_b,
					chinese_remainder_coprime_pair_num_above( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b ) - arg_index_a
				);
			}
		}

		/// \brief TODOCUMENT
		///
		/// \todo A better constexpr GCD function is likely to get added to
		///       the C++ standard library (see N3913) so switch to that when it's available
		template <typename T>
		inline constexpr T constexpr_gcd(T a, ///< TODOCUMENT
		                                 T b  ///< TODOCUMENT
		                                 ) {
			static_assert( std::is_unsigned<T>::value, "constexpr_gcd() must be performed on an unsigned integral type" );
			return ( b == 0 ) ? a
			                  : constexpr_gcd( b, a % b );
		}

		/// \brief TODOCUMENT
		///
		/// \todo A better constexpr LCM function is likely to get added to
		///       the C++ standard library (see N3913) so switch to that when it's available
		template <typename T>
		inline constexpr T constexpr_lcm(T a, ///< TODOCUMENT
		                                 T b  ///< TODOCUMENT
		                                 ) {
			static_assert( std::is_unsigned<T>::value, "constexpr_lcm() must be performed on an unsigned integral type" );
			return ( a != 0 && b != 0 ) ? ( a / constexpr_gcd( a, b ) ) * b
			                            : 0;
		}

		// template <typename T> class TD;

		/// \brief TODOCUMENT
		template <typename T>
		inline constexpr std::pair<T, T> chinese_remainder_coprime_pair(const T &arg_index_a, ///< TODOCUMENT
		                                                                const T &arg_index_b, ///< TODOCUMENT
		                                                                const T &arg_mod_a,   ///< TODOCUMENT
		                                                                const T &arg_mod_b    ///< TODOCUMENT
		                                                                ) {
			return detail::chinese_remainder_coprime_pair( arg_index_a, arg_index_b, arg_mod_a, arg_mod_b );
		}

	}
}

#endif
