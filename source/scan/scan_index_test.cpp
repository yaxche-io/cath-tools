/// \file
/// \brief The scan_index test suite

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

#include <boost/test/auto_unit_test.hpp>

#include <boost/log/trivial.hpp> /// ***** TEMPORARY *****

#include "common/chrono/duration_to_seconds_string.h"
//#include "scan/detail/res_pair_dirn/res_pair_dirn.h"
#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_from_phi_keyer_part.h"
#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_from_psi_keyer_part.h"
#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_index_dirn_keyer_part.h"
//#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_orient_keyer_part.h"
#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_to_phi_keyer_part.h"
#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_to_psi_keyer_part.h"
#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_view_x_keyer_part.h"
#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_view_y_keyer_part.h"
#include "scan/res_pair_keyer/res_pair_keyer_part/res_pair_view_z_keyer_part.h"
#include "scan/scan_action/populate_matrix_scan_action.h" // ***** TEMPORARY? *****
#include "scan/scan_action/record_scores_scan_action.h"   // ***** TEMPORARY? *****
#include "scan/scan_index.h"
#include "scan/scan_query_set.h"
#include "structure/geometry/angle.h"
#include "structure/protein/protein.h"
#include "structure/protein/protein_source_file_set/protein_source_from_pdb.h"
#include "structure/protein/sec_struc.h"
#include "structure/protein/sec_struc_planar_angles.h"
#include "test/global_test_constants.h"

#include <chrono> // ***** TEMPORARY? *****

using namespace cath;
using namespace cath::common;
using namespace cath::geom;
using namespace cath::scan;
using namespace std;
using namespace std::chrono; // ***** TEMPORARY? *****

namespace cath {
	namespace test {

		/// \brief The scan_index_test_suite_fixture to assist in testing scan_index
		struct scan_index_test_suite_fixture : protected global_test_constants {
		protected:
			~scan_index_test_suite_fixture() noexcept = default;
		};

	}
}

/// \brief TODOCUMENT
BOOST_FIXTURE_TEST_SUITE(scan_index_test_suite, cath::test::scan_index_test_suite_fixture)

/// \brief TODOCUMENT
BOOST_AUTO_TEST_CASE(basic) {
	BOOST_CHECK_EQUAL(0, 0);
	ostringstream parse_ss;

	// 1fseB00 has 70 residues so the best possible score should be something like 70*70*2
	const auto read_pdbs_starttime = high_resolution_clock::now();
//	const protein  protein_a = read_protein_from_files( protein_source_from_pdb(), TEST_SOURCE_DATA_DIR(), "1fseB00", parse_ss );
//	const protein &protein_b = protein_a;

	// const protein protein_a = read_protein_from_files( protein_source_from_pdb(), TEST_SOURCE_DATA_DIR(), "1n3lA01", parse_ss );
	// const protein protein_b = read_protein_from_files( protein_source_from_pdb(), TEST_SOURCE_DATA_DIR(), "1r6xA02", parse_ss );

	const protein protein_a   = read_protein_from_files( protein_source_from_pdb(), TEST_SOURCE_DATA_DIR(), "1fseB00", parse_ss );
	const protein protein_b   = read_protein_from_files( protein_source_from_pdb(), TEST_SOURCE_DATA_DIR(), "1avyA00", parse_ss );

	const auto read_pdbs_durn = high_resolution_clock::now() - read_pdbs_starttime;

	BOOST_LOG_TRIVIAL( warning ) << "Read two PDBs - took " << durn_to_seconds_string( read_pdbs_durn ) << " (" << durn_to_rate_per_second_string( read_pdbs_durn ) << ")";
	const auto angle_radius = make_angle_from_degrees<scan::detail::angle_base_type>( 120 );
	const auto the_scan_policy = make_scan_policy(
		make_res_pair_keyer(
			res_pair_from_phi_keyer_part  { angle_radius },
			res_pair_from_psi_keyer_part  { angle_radius },
			res_pair_to_phi_keyer_part    { angle_radius },
			res_pair_to_psi_keyer_part    { angle_radius },
			res_pair_index_dirn_keyer_part{},
//			res_pair_orient_keyer_part    {},
			res_pair_view_x_keyer_part    { 12.65f },
			res_pair_view_y_keyer_part    { 12.65f },
			res_pair_view_z_keyer_part    { 12.65f }
		),
		make_default_quad_criteria(),
		scan_stride{ 2, 2, 1, 1 }
	);
//	BOOST_LOG_TRIVIAL( warning ) << "About to make scan_query_set";
	const auto make_scan_query_set_starttime = high_resolution_clock::now();
	auto the_scan_query_set = make_scan_query_set( the_scan_policy );
	const auto make_scan_query_set_durn      = high_resolution_clock::now() - make_scan_query_set_starttime;
	BOOST_LOG_TRIVIAL( warning ) << "Made scan_query_set - took " << durn_to_seconds_string( make_scan_query_set_durn ) << " (" << durn_to_rate_per_second_string( make_scan_query_set_durn ) << ")";


//	BOOST_LOG_TRIVIAL( warning ) << "About to make scan_index";
	const auto make_scan_index_starttime = high_resolution_clock::now();
	auto the_scan_index     = make_scan_index    ( the_scan_policy );
	const auto make_scan_index_durn      = high_resolution_clock::now() - make_scan_index_starttime;
	BOOST_LOG_TRIVIAL( warning ) << "Made scan_index - took " << durn_to_seconds_string( make_scan_index_durn ) << " (" << durn_to_rate_per_second_string( make_scan_index_durn ) << ")";

//	BOOST_LOG_TRIVIAL( warning ) << "About to add structure to query_set";
	const auto add_protein_a_starttime = high_resolution_clock::now();
	the_scan_query_set.add_structure( protein_a );
	const auto add_protein_a_durn      = high_resolution_clock::now() - add_protein_a_starttime;
	BOOST_LOG_TRIVIAL( warning ) << "Added structure to query_set - took " << durn_to_seconds_string( add_protein_a_durn ) << " (" << durn_to_rate_per_second_string( add_protein_a_durn ) << ")";

//	BOOST_LOG_TRIVIAL( warning ) << "About to add structure to index";
	const auto add_protein_b_starttime = high_resolution_clock::now();
	the_scan_index.add_structure    ( protein_b );
	const auto add_protein_b_durn      = high_resolution_clock::now() - add_protein_b_starttime;
	BOOST_LOG_TRIVIAL( warning ) << "Added structure to index - took " << durn_to_seconds_string( add_protein_b_durn ) << " (" << durn_to_rate_per_second_string( add_protein_b_durn ) << ")";

//	BOOST_LOG_TRIVIAL( warning ) << "About to add structure to index";
//	log_scan_action the_action;
//	auto the_action = make_record_scores_scan_action( the_scan_query_set, the_scan_index );
	auto the_action = make_populate_matrix_scan_action( the_scan_query_set, the_scan_index, 0, 0 );
	const auto do_magic_starttime = high_resolution_clock::now();
	the_scan_query_set.do_magic( the_scan_index, the_action );
	const auto do_magic_durn      = high_resolution_clock::now() - do_magic_starttime;
	BOOST_LOG_TRIVIAL( warning ) << "Did magic - took " << durn_to_seconds_string( do_magic_durn ) << " (" << durn_to_rate_per_second_string( do_magic_durn ) << ")";
//	BOOST_LOG_TRIVIAL( warning ) << "num_matches is : " << the_action.num_matches;
//	BOOST_LOG_TRIVIAL( warning ) << "The final score is " << the_action.get_score( 0, 0 );
	gnuplot_to_file( the_action, "/tmp/matrix" );
}

BOOST_AUTO_TEST_SUITE_END()
