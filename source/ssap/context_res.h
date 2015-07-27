/// \file
/// \brief The context_res header

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

#ifndef CONTEXT_RES_H_INCLUDED
#define CONTEXT_RES_H_INCLUDED

#include "common/debug_numeric_cast.h"
#include "ssap/distance_score_formula.h"
#include "structure/entry_querier/entry_querier.h"
#include "structure/entry_querier/residue_querier.h"
#include "structure/geometry/coord.h"
#include "structure/protein/protein.h"
#include "structure/protein/residue.h"

namespace cath {

	template <distance_score_formula F = distance_score_formula::USED_IN_PREVIOUS_CODE>
	inline float_score_type score_of_squared_distance(const float_score_type &arg_scaled_squared_distance, ///< TODOCUMENT
	                                                  const float_score_type &arg_int_scaling_float_score  ///< TODOCUMENT
													  ) {
		//
		const float_score_type &residue_max_dist_sq_cutoff = residue_querier::RESIDUE_MAX_DIST_SQ_CUTOFF;
		const float_score_type &residue_a_value            = residue_querier::RESIDUE_A_VALUE;
		const float_score_type &residue_b_value            = residue_querier::RESIDUE_B_VALUE;

		switch ( F ) {
			case( distance_score_formula::FROM_SSAP_PAPER       ) : {
				const float_score_type distance = sqrt( arg_scaled_squared_distance ) / arg_int_scaling_float_score;
				if ( distance >= sqrt( residue_max_dist_sq_cutoff ) ) {
					return 0.0;
				}
				return residue_a_value / ( distance + residue_b_value );
				break;
			}
			case( distance_score_formula::SIMPLIFIED            ) : {

				// The USED_IN_PREVIOUS_CODE cutoff is at a distance of x = 2 * sqrt( b )
				// Plugging that back into the desired y = 1 -x/7, gives that the SIMPLIFIED crosses
				// USED_IN_PREVIOUS_CODE AT y = 1 - (2 / 7) * sqrt( b ) = 0.096492097094
				const float_score_type distance = sqrt( arg_scaled_squared_distance ) / arg_int_scaling_float_score;
				const float_score_type slope    = -1.0 / sqrt( 4.9 * residue_b_value );
				const float_score_type final    = std::max( 0.0, (residue_a_value / residue_b_value) * ( 1.0 + slope * distance) );
				if ( std::isnan( final ) || final < 0.0 ) {
					std::cerr << "distance is  : " << distance << std::endl;
					std::cerr << "slope is     : " << slope    << std::endl;
					std::cerr << "distance     : " << distance << std::endl;
	//				std::cerr << "should be 50 : " << (residue_a_value / residue_b_value)                             << std::endl;
	//				std::cerr << "should be 0  : " << (residue_a_value / residue_b_value) * ( 1.0 + slope * 7.0)      << std::endl;
					std::cerr << "final        : " << final << std::endl;
				}
				return final;
				break;
			}
			case( distance_score_formula::USED_IN_PREVIOUS_CODE ) : {
				const float_score_type scaled_a = residue_a_value * arg_int_scaling_float_score * arg_int_scaling_float_score;
				const float_score_type scaled_b = residue_b_value * arg_int_scaling_float_score * arg_int_scaling_float_score;
				if ( arg_scaled_squared_distance >= residue_max_dist_sq_cutoff * arg_int_scaling_float_score * arg_int_scaling_float_score ) {
					return 0.0;
				}
				return scaled_a / ( arg_scaled_squared_distance + scaled_b );
				break;
			}

		}
	}

	/// \brief Compares vectors/scalars/Hbonds/SSbonds between residues in the two proteins
	///
	/// This code uses debug_numeric_casts rather than numeric_casts for the sake of speed
	/// (this was indicated by profiling and its effect was confirmed with direct tests).
	template <bool arg_int_rounding, distance_score_formula F = distance_score_formula::USED_IN_PREVIOUS_CODE>
	inline float_score_type context_res_vec(const geom::coord &arg_i_beta_from_a_beta_view, ///< The view from one residue to another in one protein
	                                        const geom::coord &arg_j_beta_from_b_beta_view  ///< The view from one residue to another in the other protein
	                                        ) {
		//
		const float_score_type int_scaling_float_score = debug_numeric_cast<float_score_type>( entry_querier::INTEGER_SCALING );
		geom::coord int_scaled_i_beta_from_a_beta = arg_int_rounding ? int_cast_copy( int_scaling_float_score * arg_i_beta_from_a_beta_view )
		                                                             :              ( int_scaling_float_score * arg_i_beta_from_a_beta_view );
		geom::coord int_scaled_j_beta_from_b_beta = arg_int_rounding ? int_cast_copy( int_scaling_float_score * arg_j_beta_from_b_beta_view )
		                                                             :              ( int_scaling_float_score * arg_j_beta_from_b_beta_view );
		//
		const float_score_type x_diff = arg_int_rounding ? debug_numeric_cast<int>(int_scaled_i_beta_from_a_beta.get_x()) - debug_numeric_cast<int>(int_scaled_j_beta_from_b_beta.get_x())
		                                                 :                         int_scaled_i_beta_from_a_beta.get_x()  -                         int_scaled_j_beta_from_b_beta.get_x();
		const float_score_type y_diff = arg_int_rounding ? debug_numeric_cast<int>(int_scaled_i_beta_from_a_beta.get_y()) - debug_numeric_cast<int>(int_scaled_j_beta_from_b_beta.get_y())
		                                                 :                         int_scaled_i_beta_from_a_beta.get_y()  -                         int_scaled_j_beta_from_b_beta.get_y();
		const float_score_type z_diff = arg_int_rounding ? debug_numeric_cast<int>(int_scaled_i_beta_from_a_beta.get_z()) - debug_numeric_cast<int>(int_scaled_j_beta_from_b_beta.get_z())
		                                                 :                         int_scaled_i_beta_from_a_beta.get_z()  -                         int_scaled_j_beta_from_b_beta.get_z();

		//
		const float_score_type x_diff_squared   = x_diff * x_diff;
		const float_score_type y_diff_squared   = y_diff * y_diff;
		const float_score_type z_diff_squared   = z_diff * z_diff;
		const float_score_type squared_distance = x_diff_squared + y_diff_squared + z_diff_squared;

		//
//		const float_score_type residue_max_dist_sq_cutoff = residue_querier::RESIDUE_MAX_DIST_SQ_CUTOFF;
//		if ( squared_distance >= residue_max_dist_sq_cutoff * int_scaling_float_score * int_scaling_float_score ) {
//			return 0;
//		}

//		const float_score_type residue_a_value = residue_querier::RESIDUE_A_VALUE;
//		const float_score_type residue_b_value = residue_querier::RESIDUE_B_VALUE;
//		const float_score_type SCALED_A = residue_a_value * int_scaling_float_score * int_scaling_float_score;
//		const float_score_type SCALED_B = residue_b_value * int_scaling_float_score * int_scaling_float_score;

//		return SCALED_A / ( squared_distance + SCALED_B );

		return score_of_squared_distance<F>( squared_distance, int_scaling_float_score );
	}

	/// \brief Compares vectors/scalars/Hbonds/SSbonds between residues in the two proteins
	///
	/// This code uses debug_numeric_casts rather than numeric_casts for the sake of speed
	/// (this was indicated by profiling and its effect was confirmed with direct tests).
	///
	/// \todo Consider removing this and replacing with context_res_vec<false, distance_score_formula::SIMPLIFIED>
	inline float_score_type simplified_context_res_vec(const geom::coord &arg_i_beta_from_a_beta_view, ///< The view from one residue to another in one protein
	                                                   const geom::coord &arg_j_beta_from_b_beta_view  ///< The view from one residue to another in the other protein
	                                                   ) {
		//
		const float_score_type x_diff = arg_i_beta_from_a_beta_view.get_x() - arg_j_beta_from_b_beta_view.get_x();
		const float_score_type y_diff = arg_i_beta_from_a_beta_view.get_y() - arg_j_beta_from_b_beta_view.get_y();
		const float_score_type z_diff = arg_i_beta_from_a_beta_view.get_z() - arg_j_beta_from_b_beta_view.get_z();

		//
		const float_score_type x_diff_squared   = x_diff * x_diff;
		const float_score_type y_diff_squared   = y_diff * y_diff;
		const float_score_type z_diff_squared   = z_diff * z_diff;
		const float_score_type squared_distance = x_diff_squared + y_diff_squared + z_diff_squared;

		//
		const float_score_type residue_max_dist_sq_cutoff = residue_querier::RESIDUE_MAX_DIST_SQ_CUTOFF;
		if ( squared_distance >= residue_max_dist_sq_cutoff ) {
			return 0;
		}

		return residue_querier::RESIDUE_A_VALUE / ( squared_distance + residue_querier::RESIDUE_B_VALUE );
	}

	/// \brief TODOCUMENT
	inline geom::coord view_vector_of_residue_pair(const residue &arg_from_res, ///< The "from" residue in the protein
	                                               const residue &arg_to_res    ///< The "to"   residue in the protein
	                                               ) {
		return rotate_copy(
			arg_from_res.get_frame(),
			arg_to_res.get_carbon_beta_coord() - arg_from_res.get_carbon_beta_coord()
		);
	}

	/// \brief Compares vectors/scalars/Hbonds/SSbonds between residues in the two proteins
	template <bool arg_int_rounding, distance_score_formula F = distance_score_formula::USED_IN_PREVIOUS_CODE>
	inline float_score_type context_res(const residue &arg_from_res_a, ///< The "from" residue in the first  protein
	                                    const residue &arg_from_res_b, ///< The "from" residue in the second protein
	                                    const residue &arg_to_res_a,   ///< The "to"   residue in the first  protein
	                                    const residue &arg_to_res_b    ///< The "to"   residue in the second protein
	                                    ) {
		return context_res_vec<arg_int_rounding, F>(
			view_vector_of_residue_pair( arg_from_res_a, arg_to_res_a ),
			view_vector_of_residue_pair( arg_from_res_b, arg_to_res_b )
		);
	}

	/// \brief TODOCUMENT
	inline float_score_type context_res(const residue                &arg_from_res_a,                                               ///< The "from" residue in the first  protein
	                                    const residue                &arg_from_res_b,                                               ///< The "from" residue in the second protein
	                                    const residue                &arg_to_res_a,                                                 ///< The "to"   residue in the first  protein
	                                    const residue                &arg_to_res_b,                                                 ///< The "to"   residue in the second protein
	                                    const bool                   &arg_rounding  = false,                                        ///< TODOCUMENT
	                                    const distance_score_formula &arg_dist_form = distance_score_formula::USED_IN_PREVIOUS_CODE ///< TODOCUMENT
	                                    ) {
		return arg_rounding
			? ( ( arg_dist_form == distance_score_formula::FROM_SSAP_PAPER ) ? context_res<true,  distance_score_formula::FROM_SSAP_PAPER      > ( arg_from_res_a, arg_from_res_b, arg_to_res_a, arg_to_res_b ) :
			    ( arg_dist_form == distance_score_formula::SIMPLIFIED      ) ? context_res<true,  distance_score_formula::SIMPLIFIED           > ( arg_from_res_a, arg_from_res_b, arg_to_res_a, arg_to_res_b ) :
			                                                                   context_res<true,  distance_score_formula::USED_IN_PREVIOUS_CODE> ( arg_from_res_a, arg_from_res_b, arg_to_res_a, arg_to_res_b ) )
			: ( ( arg_dist_form == distance_score_formula::FROM_SSAP_PAPER ) ? context_res<false, distance_score_formula::FROM_SSAP_PAPER      > ( arg_from_res_a, arg_from_res_b, arg_to_res_a, arg_to_res_b ) :
			    ( arg_dist_form == distance_score_formula::SIMPLIFIED      ) ? context_res<false, distance_score_formula::SIMPLIFIED           > ( arg_from_res_a, arg_from_res_b, arg_to_res_a, arg_to_res_b ) :
			                                                                   context_res<false, distance_score_formula::USED_IN_PREVIOUS_CODE> ( arg_from_res_a, arg_from_res_b, arg_to_res_a, arg_to_res_b ) );
	}

	/// \brief TODOCUMENT
	inline float_score_type context_res(const protein                &arg_protein_a,                                                ///< TODOCUMENT
	                                    const protein                &arg_protein_b,                                                ///< TODOCUMENT
	                                    const size_t                 &arg_a_from_index,                                             ///< TODOCUMENT
	                                    const size_t                 &arg_b_from_index,                                             ///< TODOCUMENT
	                                    const size_t                 &arg_a_to_index,                                               ///< TODOCUMENT
	                                    const size_t                 &arg_b_to_index,                                               ///< TODOCUMENT
	                                    const bool                   &arg_rounding  = false,                                        ///< TODOCUMENT
	                                    const distance_score_formula &arg_dist_form = distance_score_formula::USED_IN_PREVIOUS_CODE ///< TODOCUMENT
	                                    ) {
		return context_res(
			arg_protein_a.get_residue_ref_of_index( arg_a_from_index ),
			arg_protein_b.get_residue_ref_of_index( arg_b_from_index ),
			arg_protein_a.get_residue_ref_of_index( arg_a_to_index   ),
			arg_protein_b.get_residue_ref_of_index( arg_b_to_index   ),
			arg_rounding,
			arg_dist_form
		);
	}
}

#endif
