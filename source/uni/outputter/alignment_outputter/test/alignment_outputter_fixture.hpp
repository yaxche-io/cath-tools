/// \file
/// \brief The alignment_outputter_fixture class declarations

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

#ifndef _CATH_TOOLS_SOURCE_UNI_OUTPUTTER_ALIGNMENT_OUTPUTTER_TEST_ALIGNMENT_OUTPUTTER_FIXTURE_H
#define _CATH_TOOLS_SOURCE_UNI_OUTPUTTER_ALIGNMENT_OUTPUTTER_TEST_ALIGNMENT_OUTPUTTER_FIXTURE_H

#include <boost/filesystem/path.hpp>

#include "common/file/temp_file.hpp"

#include <sstream>

namespace cath { namespace align { class alignment_context; } }

namespace cath {
	namespace test {

		/// \brief Fixture to help in testing alignment_outputter classes
		class alignment_outputter_fixture {
		protected:
			~alignment_outputter_fixture();

			/// \brief A temp_file temporary file to which output can be written
			const common::temp_file       out_temp_file{ "cath_tools_test_temp_file.alignment_outputter.%%%%" };

			/// \brief The path of the temp_file
			const boost::filesystem::path out_file     { get_filename( out_temp_file ) };

			/// \brief An output stream, which gets checked for emptiness at the end of the test
			std::ostringstream            out_ss;

			align::alignment_context get_example_alignment_context() const;
		};

	} // namespace test
} // namespace cath
#endif
