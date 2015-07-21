/// \file
/// \brief The entry_querier test suite

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
#include "entry_querier.h"

#include <boost/test/unit_test.hpp>

#include "common/size_t_literal.h"
#include "structure/entry_querier/residue_querier.h"
#include "structure/entry_querier/sec_struc_querier.h"

using namespace cath;
using namespace cath::common;

namespace cath {
	namespace test {

		struct entry_querier_test_suite_fixture {
		protected:
			~entry_querier_test_suite_fixture() noexcept = default;

		public:
			const size_t            exclusion_value_test_max = 10;	
			const residue_querier   example_residue_querier{};
			const sec_struc_querier example_sec_struc_querier{};
		};

	}
}

BOOST_FIXTURE_TEST_SUITE(entry_querier_test_suite, cath::test::entry_querier_test_suite_fixture)

/// \brief TODOCUMENT
BOOST_AUTO_TEST_CASE(num_comparable) {
	BOOST_CHECK_EQUAL( 8930_z, _num_comparable_impl( 5,   100 ) );
	BOOST_CHECK_EQUAL(   90_z, _num_comparable_impl( 5,    15 ) );
	BOOST_CHECK_EQUAL(    6_z, _num_comparable_impl( 4,     7 ) );
}

/// \brief TODOCUMENT
BOOST_AUTO_TEST_CASE(exclusions_work_for_a_few_sec_struc_values) {
	for (size_t ctr_a = 0; ctr_a < exclusion_value_test_max; ++ctr_a) {
		for (size_t ctr_b = 0; ctr_b < exclusion_value_test_max; ++ctr_b) {
			const bool not_excluded = (ctr_a != ctr_b);
			BOOST_CHECK_EQUAL( not_excluded, pair_is_not_excluded(example_sec_struc_querier, ctr_a, ctr_b) );
			BOOST_CHECK_EQUAL( not_excluded, pair_is_not_excluded(example_sec_struc_querier, ctr_b, ctr_a) );
		}
	}
}

/// \brief TODOCUMENT
BOOST_AUTO_TEST_CASE(exclusions_work_for_a_few_residue_values) {
	for (size_t ctr_a = 0; ctr_a < exclusion_value_test_max; ++ctr_a) {
		for (size_t ctr_b = 0; ctr_b < exclusion_value_test_max; ++ctr_b) {
			const bool not_excluded = (ctr_b > ctr_a)
			                          ? (ctr_b - ctr_a) > 5
			                          : (ctr_a - ctr_b) > 5;
			BOOST_CHECK_EQUAL( not_excluded, pair_is_not_excluded(example_residue_querier, ctr_a, ctr_b) );
			BOOST_CHECK_EQUAL( not_excluded, pair_is_not_excluded(example_residue_querier, ctr_b, ctr_a) );
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
