/// \file
/// \brief The eigen class definitions

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

#include "pca.hpp"

#include <boost/range/irange.hpp>

#include "common/gsl/gsl_matrix_wrp.hpp"
#include "common/gsl/gsl_vector_wrp.hpp"
#include "common/size_t_literal.hpp"
#include "structure/geometry/coord.hpp"
#include "structure/geometry/coord_list.hpp"

#include <gsl/gsl_linalg.h>

using namespace cath;
using namespace cath::common::literals;
using namespace cath::geom;
using namespace cath::geom::detail;

using boost::irange;

/// \brief Build a matrix of the specified points, offset by subtracting their specified centre-of-gravity
///
/// This is for use in line_of_best_fit
doub_vec cath::geom::detail::build_matrix_of_coords(const coord_list &arg_coords, ///< The points for which to build a matrix
                                                    const coord      &arg_cog     ///< The centre-of-gravity of the points
                                                    ) {
	doub_vec matrix;
	matrix.reserve( 3 * arg_coords.size() );
	for (const coord &the_coord : arg_coords) {
		matrix.push_back( the_coord.get_x() - arg_cog.get_x() );
		matrix.push_back( the_coord.get_y() - arg_cog.get_y() );
		matrix.push_back( the_coord.get_z() - arg_cog.get_z() );
	}
	return matrix;
}

/// \brief Get a line-of-best-fit through the specified points
coord_coord_pair cath::geom::line_of_best_fit(const coord_list &arg_coords ///< The list of points through which to put a line-of-best-fit
                                              ) {
	const auto      cog    = centre_of_gravity     ( arg_coords );
	doub_vec        matrix = build_matrix_of_coords( arg_coords, cog );
	gsl_matrix_view A      = gsl_matrix_view_array ( &matrix.front(), arg_coords.size(), 3 );

	gsl_matrix_wrp  V    { 3, 3 };
	gsl_vector_wrp  S    { 3    };
	gsl_vector_wrp  work { 3    };

	gsl_linalg_SV_decomp(
		&A.matrix,
		V.get_ptr(),
		S.get_ptr(),
		work.get_ptr()
	);

	return {
		cog,
		{
			gsl_matrix_get( V.get_ptr(), 0, 0 ),
			gsl_matrix_get( V.get_ptr(), 1, 0 ),
			gsl_matrix_get( V.get_ptr(), 2, 0 )
		}
	};
}
