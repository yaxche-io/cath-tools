/// \file
/// \brief The view_cache_index_dim_linear class header

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

#ifndef VIEW_CACHE_INDEX_DIM_LINEAR_Z_H_INCLUDED
#define VIEW_CACHE_INDEX_DIM_LINEAR_Z_H_INCLUDED

#include "structure/view_cache/index/detail/dims/detail/vci_linear_dim_spec_view_axis.h"
#include "structure/view_cache/index/detail/dims/detail/view_cache_index_dim_linear.h"

namespace cath {
	namespace index {
		namespace detail {
			namespace detail {

				/// \brief TODOCUMENT
				struct vci_linear_view_z_getter final {

					/// \brief TODOCUMENT
					std::string get_name() const {
						return "z-axis";
					}

					/// \brief TODOCUMENT
					auto operator()(const view_cache_index_entry &arg_entry ///< TODOCUMENT
					                )->decltype( get_view_z( arg_entry ) ) {
						return get_view_z( arg_entry );
					}
				};

				using vci_linear_dim_spec_view_z = vci_linear_dim_spec_view_axis<vci_linear_view_z_getter>;
			}

			/// \brief TODOCUMENT
			using view_cache_index_dim_linear_z = detail::view_cache_index_dim_linear< detail::vci_linear_dim_spec_view_z >;
		}
	}
}

#endif

