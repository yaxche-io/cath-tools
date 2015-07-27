/// \file
/// \brief The jmol_viewer class header

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

#ifndef JMOL_VIEWER_H_INCLUDED
#define JMOL_VIEWER_H_INCLUDED

#include "display/viewer/rasmol_style_viewer.h"

namespace cath {

	/// \brief TODOCUMENT
	class jmol_viewer final : public rasmol_style_viewer {
	private:
		virtual std::string do_default_executable() const override final;

		virtual void do_write_start(std::ostream &) const override final;
		virtual void do_write_load_pdbs(std::ostream &,
		                                const sup::superposition &,
		                                const file::pdb_list &,
		                                const str_vec &) const override final;
		virtual void do_define_colour(std::ostream &,
		                              const display_colour &,
		                              const std::string &) const override final;
		virtual void do_colour_pdb(std::ostream &,
		                           const std::string &,
		                           const std::string &) const override final;
		virtual void do_colour_pdb_residues(std::ostream &,
		                                    const std::string &,
		                                    const std::string &,
		                                    const residue_name_vec &) const override final;
		virtual void do_write_alignment_extras(std::ostream &,
		                                       const sup::superposition_context &) const override final;
		virtual void do_write_end(std::ostream &) const override final;

	public:
		virtual ~jmol_viewer() noexcept = default;
	};

}

#endif
