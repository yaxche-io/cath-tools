/// \file
/// \brief The sequence_string_dyn_prog_score_source class header

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

#ifndef SEQUENCE_STRING_DYN_PROG_SCORE_SOURCE_H_INCLUDED
#define SEQUENCE_STRING_DYN_PROG_SCORE_SOURCE_H_INCLUDED

#include "alignment/dyn_prog_align/dyn_prog_score_source/dyn_prog_score_source.h"

#include <string>

namespace cath {
	namespace align {
		class alignment;

		/// \brief TODOCUMENT
		class sequence_string_dyn_prog_score_source final : public dyn_prog_score_source {
		private:
			const std::string sequence_string_a;
			const std::string sequence_string_b;

			static void check_sequence_string(const std::string &);

			virtual size_t do_get_length_a() const override final;
			virtual size_t do_get_length_b() const override final;
			virtual score_type do_get_score(const size_t &,
			                                const size_t &) const override final;

		public:
			sequence_string_dyn_prog_score_source(const std::string &,
			                                      const std::string &);
			virtual ~sequence_string_dyn_prog_score_source() noexcept = default;
		};

	}
}

#endif
