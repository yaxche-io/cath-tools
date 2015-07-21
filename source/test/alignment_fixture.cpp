/// \file
/// \brief The alignment_fixture class definitions

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

#include "alignment_fixture.h"

#include "alignment/align_type_aliases.h"
#include "common/size_t_literal.h"
#include "structure/entry_querier/residue_querier.h"

using namespace cath;
using namespace cath::align;
using namespace cath::common;
using namespace std;

using boost::none;

const opt_aln_posn_vec alignment_fixture::aln_list_a     = { opt_size( 0_z ), 1, 2, 3       };
const opt_aln_posn_vec alignment_fixture::aln_list_b     = { opt_size( 0_z ), 1, 2, none    };
const opt_aln_posn_vec alignment_fixture::aln_list_long  = { opt_size( 0_z ), 1, 2, 3, 4, 5 };

const alignment        alignment_fixture::aln_a_a        ( { aln_list_a,    aln_list_a    } );
const alignment        alignment_fixture::aln_a_b        ( { aln_list_a,    aln_list_b    } );
const alignment        alignment_fixture::aln_b_a        ( { aln_list_b,    aln_list_a    } );
const alignment        alignment_fixture::aln_long_long  ( { aln_list_long, aln_list_long } );

const opt_score_vec    alignment_fixture::example_scores = { 3.6, 6.8, 2.1, 999.999 };

