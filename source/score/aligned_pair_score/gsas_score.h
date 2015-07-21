/// \file
/// \brief The gsas_score class header

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

#ifndef GSAS_SCORE_H_INCLUDED
#define GSAS_SCORE_H_INCLUDED

#include "score/aligned_pair_score/aligned_pair_score.h"
#include "score/aligned_pair_score/length_score.h"
#include "score/aligned_pair_score/rmsd_score.h"
#include "score/length_getter/num_aligned_length_getter.h"

namespace cath {
	namespace score {

		/// \brief Calculate (and represent) gapped structural alignment score (GSAS),
		///        a measure that attempts to balance the RMSD based on the number of
		///        aligned residues and gaps in the alignment.
		///
		/// \ingroup cath_score_aligned_pair_score_group
		class gsas_score : public aligned_pair_score {
		private:
			friend class boost::serialization::access;

			template<class archive> void serialize(archive &ar,
			                                       const size_t /*version*/
			                                       ) {
				ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( aligned_pair_score );
				ar & BOOST_SERIALIZATION_NVP( rmsd );
				ar & BOOST_SERIALIZATION_NVP( num_aligned_residues );
			}

			/// \brief TODOCUMENT
			rmsd_score   rmsd;

			/// \brief TODOCUMENT
			length_score num_aligned_residues { num_aligned_length_getter() };

			virtual std::unique_ptr<aligned_pair_score> do_clone() const override final;

			virtual boost::logic::tribool do_higher_is_better() const override final;
			virtual score_value do_calculate(const align::alignment &,
			                                 const protein &,
			                                 const protein &) const override final;
			virtual std::string do_description() const override final;
			virtual std::string do_id_name() const override final;
			virtual str_bool_pair_vec do_short_name_suffixes() const override final;
			virtual std::string do_long_name() const override final;
			virtual std::string do_reference() const override final;

//			virtual std::unique_ptr<aligned_pair_score> do_build_from_short_name_spec(const std::string &) const override final;

			virtual bool do_less_than_with_same_dynamic_type(const aligned_pair_score &) const override final;

		public:
			/// \brief Default ctor for gsas_score that uses the defaults for rmsd_score and length_score (selecting CA atoms for all aligned residues)
			gsas_score() = default;
			gsas_score(const align::common_residue_selection_policy &,
			           const align::common_atom_selection_policy &);
			virtual ~gsas_score() noexcept = default;

			const rmsd_score & get_rmsd_score() const;
			const length_score & get_num_aligned_residues() const;
		};

		bool operator<(const gsas_score &,
		               const gsas_score &);

	}
}
#endif
