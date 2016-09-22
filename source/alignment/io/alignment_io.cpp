/// \file
/// \brief The alignment_io class definitions

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

#include "alignment_io.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "alignment/align_type_aliases.h"
#include "alignment/pair_alignment.h"
#include "common/boost_addenda/string_algorithm/split_build.h"
#include "common/c++14/cbegin_cend.h"
#include "common/file/open_fstream.h"
#include "common/type_aliases.h"
#include "exception/invalid_argument_exception.h"
#include "exception/runtime_error_exception.h"
#include "file/pdb/pdb.h"
#include "file/pdb/pdb_atom.h"
#include "file/pdb/pdb_list.h"
#include "file/pdb/pdb_residue.h"
#include "structure/protein/protein.h"
#include "structure/protein/protein_list.h"
#include "structure/protein/residue.h"
#include "structure/protein/sec_struc.h"
#include "structure/protein/sec_struc_planar_angles.h"

#include <fstream>
#include <iostream>

using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace boost::log;
using namespace cath;
using namespace cath::align;
using namespace cath::common;
using namespace cath::file;
using namespace std;

using boost::algorithm::join;
using boost::algorithm::is_any_of;
using boost::algorithm::is_space;
using boost::algorithm::starts_with;
using boost::algorithm::trim_copy;
using boost::lexical_cast;
using boost::none;
using boost::numeric_cast;

const double MIN_FRAC_OF_PDB_RESIDUES_IN_SEQ( 0.7 );

/// \brief Read a SSAP legacy alignment format from a file using two proteins as guides
///
/// \relates alignment
/// \relatesalso protein
///
/// The proteins are used for extracting the lists of residues, which are used for finding the indices of residues.
alignment cath::align::read_alignment_from_cath_ssap_legacy_format(const path    &arg_alignment_file, ///< TODOCUMENT
                                                                   const protein &arg_protein_a,      ///< TODOCUMENT
                                                                   const protein &arg_protein_b,      ///< TODOCUMENT
                                                                   ostream       &arg_stderr          ///< TODOCUMENT
                                                                   ) {
	ifstream alignment_ifstream;
	open_ifstream( alignment_ifstream, arg_alignment_file );
	const alignment new_alignment = read_alignment_from_cath_ssap_legacy_format(
		alignment_ifstream,
		arg_protein_a,
		arg_protein_b,
		arg_stderr
	);
	alignment_ifstream.close();
	return new_alignment;
}

/// \brief Read a SSAP legacy alignment format from an istream using two proteins as guides
///
/// \relates alignment
/// \relatesalso protein
///
/// The proteins are used for extracting the lists of residues, which are used for finding the indices of residues.
alignment cath::align::read_alignment_from_cath_ssap_legacy_format(istream       &arg_istream,   ///< TODOCUMENT
                                                                   const protein &arg_protein_a, ///< TODOCUMENT
                                                                   const protein &arg_protein_b, ///< TODOCUMENT
                                                                   ostream       &arg_stderr     ///< TODOCUMENT
                                                                   ) {
	return read_alignment_from_cath_ssap_legacy_format(
		arg_istream,
		get_residue_names( arg_protein_a ),
		get_residue_names( arg_protein_b ),
		arg_stderr
	);
}

/// \brief Read a SSAP legacy alignment format from an istream using two pdbs as guides
///
/// \relates alignment
/// \relatesalso pdb
///
/// The pdbs are used for extracting the lists of residues, which are used for finding the indices of residues.
alignment cath::align::read_alignment_from_cath_ssap_legacy_format(istream   &arg_istream, ///< TODOCUMENT
                                                                   const pdb &arg_pdb_a,   ///< TODOCUMENT
                                                                   const pdb &arg_pdb_b,   ///< TODOCUMENT
                                                                   ostream   &arg_stderr   ///< TODOCUMENT
                                                                   ) {
	return read_alignment_from_cath_ssap_legacy_format(
		arg_istream,
		arg_pdb_a.get_residue_names_of_first_chain__backbone_unchecked(),
		arg_pdb_b.get_residue_names_of_first_chain__backbone_unchecked(),
		arg_stderr
	);
}

/// \brief TODOCUMENT
///
/// \relates alignment
///
/// The lists of residues are used for finding the indices of residues.
///
/// Should this parse based on:
///  - exact column positions (hence breaking if an extra space is added) or
///  - whitespace splitting (hence breaking if a column is missing, eg with an insert as a space rather than a 0)
alignment cath::align::read_alignment_from_cath_ssap_legacy_format(istream                &arg_istream,     ///< TODOCUMENT
                                                                   const residue_name_vec &arg_res_names_a, ///< TODOCUMENT
                                                                   const residue_name_vec &arg_res_names_b, ///< TODOCUMENT
                                                                   ostream                &arg_stderr       ///< TODOCUMENT
                                                                   ) {
	arg_istream.exceptions( ios::badbit );

	alignment new_alignment( alignment::NUM_ENTRIES_IN_PAIR_ALIGNMENT );

	string line_string;
	size_t pos_a(0);
	size_t pos_b(0);
	opt_score_vec scores;
	while ( getline(arg_istream, line_string ) ) {
		const int             res_num_a = lexical_cast<int>(    trim_copy( line_string.substr(  0, 4 ))); // Column 1: Protein 1 PDB residue number (excluding insert character)
//		const char          sec_struc_a =                                  line_string.at(      5    )  ; // Column 2: Protein 1 Secondary structure character
		const char             insert_a =                                  line_string.at(      7    )  ; // Column 3: Protein 1 PDB residue insert character
		const char         amino_acid_a =                                  line_string.at(      9    )  ; // Column 4: Protein 1 Residue Code (One letter code)
		const size_t              score = lexical_cast<size_t>( trim_copy( line_string.substr( 12, 3 ))); // Column 5: SSAP residue score (0-100)
		const char         amino_acid_b =                                  line_string.at(     17    )  ; // Column 6: Protein 2 Residue Code (One letter code)
		const char             insert_b =                                  line_string.at(     19    )  ; // Column 7: Protein 2 PDB residue insert character
//		const char          sec_struc_b =                                  line_string.at(     21    )  ; // Column 8: Protein 2 Secondary structure character
		const int             res_num_b = lexical_cast<int>(    trim_copy( line_string.substr( 23, 4 ))); // Column 9: Protein 2 PDB residue number (excluding insert character)

		// For each side, move the PDB position forward if necessary
		const residue_name res_name_a = make_residue_name_with_non_insert_char( res_num_a, insert_a, '0');
		const residue_name res_name_b = make_residue_name_with_non_insert_char( res_num_b, insert_b, '0' );
		const opt_size find_a_result = search_for_residue_in_residue_names( pos_a, arg_res_names_a, amino_acid_a, res_name_a, arg_stderr );
		const opt_size find_b_result = search_for_residue_in_residue_names( pos_b, arg_res_names_b, amino_acid_b, res_name_b, arg_stderr );
		pos_a = find_a_result ? *find_a_result : pos_a;
		pos_b = find_b_result ? *find_b_result : pos_b;

		if ( find_a_result && find_b_result ) {
			append_position_both_offset_1( new_alignment, ( *find_a_result ) + 1, ( *find_b_result ) + 1 );
			scores.push_back( numeric_cast<double>( score ) );
		}
		else {
			scores.push_back( none );
			if ( find_a_result ) {
				append_position_a_offset_1( new_alignment, ( *find_a_result ) + 1 );
			}
			else if ( find_b_result ) {
				append_position_b_offset_1( new_alignment, ( *find_b_result ) + 1 );
			}
			else {
				BOOST_THROW_EXCEPTION(runtime_error_exception("Alignment file contains entry with neither residue populated"));
			}
		}
	}

	set_pair_alignment_duplicate_scores( new_alignment, scores );
	return new_alignment;
}

/// \brief TODOCUMENT
///
/// \relates alignment
///
/// CORA file format
///
///  The header consists of the following
///  - One format line '#FM CORA_FORMAT 1.1'
///  - Any number of comment lines '#CC'
///  - Total number of proteins in the alignment
///  - All CATH domain names in the alignment
///  - Total number of alignment positions
///
/// For example:
///
///     #FM CORA_FORMAT 1.1
///     #CC
///     #CC Any number of comment lines (200 characters max per line)
///     #CC
///     #CC
///     3
///     6insE0 1igl00 1bqt00
///     73
///
/// The body consists of the following:
///
///          START       PROT 1     PROT 2     PROT N         END
///     <------------><---------><---------><---------><---------------->
///     ddddxddddxddddxddddcxcxxcxddddcxcxxcxddddcxcxxcxxxcxddddxddddxxdd
///
///        1    0    1    0  0  0    1  A  0    0  0  0   0    0    0   0
///        2    0    1    0  0  0    2  Y  0    0  0  0   0    0    0   0
///        3    0    2    1B F  0    3  R  0    0  0  0   0    0    0   0
///        4    0    3    2B V  H    4  P  0    1  G  0   0    1    0   2
///        5    1    3    3B N  H    5  S  0    2  P  0   0    1    0   6
///        6    0    3    4B Q  H    6  E  0    3  E  0   0    1    0   2
///
/// START (14 characters) :
///   - Column 1: Alignment Position (dddd)
///   - Column 2: No. of position selected for structural template (dddd)
///   - Column 3: No. of proteins aligned at this position (dddd)
///
/// PROT 1,2,3... (11 characters per protein)
///   - Column 4 (7,10 etc): Residue number in PDB file (ddddc) 4 digit number
///   -    + 1 character insert code
///   -    Importantly the insert code is always in the same position not within
///   -    the 4 characters reserved for the pdb number (see below)
///   - Column 5 (8,11 etc): Amino Acid Code (c)
///   - Column 6 (9,12 etc): Secondary Structure Assignment (c)
///
/// END (18 characters)
///   - Last Column-3: Consensus Secondary Structure Assignment (c)
///   - Last Column-2: No. of alpha residues at this position (dddd)
///   - Last Column-1: No. of beta  residues at this position (dddd)
///   - Last Column: Structural Conservation Score (dd)
alignment cath::align::read_alignment_from_cath_cora_legacy_format(istream        &arg_istream, ///< TODOCUMENT
                                                                   const pdb_list &arg_pdbs,    ///< TODOCUMENT
                                                                   ostream        &arg_stderr   ///< TODOCUMENT
                                                                   ) {
	const size_t CHARS_IN_MAIN_DATA_LINE_START = 14;
	const size_t CHARS_IN_MAIN_DATA_LINE_PROT  = 11;
	const size_t CHARS_IN_MAIN_DATA_LINE_END   = 18;

	if (arg_pdbs.empty()) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot load a CORA legacy alignment with 0 PDB entries"));
	}

	arg_istream.exceptions(ios::badbit);
	try {
		residue_name_vec_vec residue_names_of_first_chains;
		for (const pdb &arg_pdb : arg_pdbs) {
			residue_names_of_first_chains.push_back( arg_pdb.get_residue_names_of_first_chain__backbone_unchecked() );
		}

		// Check the first line is the file format line
		string line_string;
		getline(arg_istream, line_string);
		if (!starts_with(line_string, "#FM CORA_FORMAT ")) {
			BOOST_THROW_EXCEPTION(runtime_error_exception("No CORA header file format line"));
		}

		// Skip any comment lines
		while (getline(arg_istream, line_string) && starts_with(line_string, "#CC")) {
		}

		// Grab the number of proteins and ensure the alignment matches
		const size_t num_proteins = lexical_cast<size_t>(line_string);
		if (num_proteins != arg_pdbs.size()) {
			BOOST_THROW_EXCEPTION(invalid_argument_exception("Number of PDBs in CORA file is " + lexical_cast<string>(num_proteins) + ", which does not match " + lexical_cast<string>(arg_pdbs.size())));
		}
		const size_t num_chars_in_main_data_line = CHARS_IN_MAIN_DATA_LINE_START + (num_proteins * CHARS_IN_MAIN_DATA_LINE_PROT) + CHARS_IN_MAIN_DATA_LINE_END;

		// Grab the protein names
		getline( arg_istream, line_string );
		trim( line_string );
		const str_vec names = split_build<str_vec>( line_string, is_space() );
		if ( names.size() != num_proteins ) {
			BOOST_THROW_EXCEPTION(runtime_error_exception("Splitting on space does not give " + lexical_cast<string>(num_proteins) + " entries in CORA alignment names line: \"" + line_string + "\""));
		}

		// Grab the total number of alignment positions
		getline(arg_istream, line_string);
		const size_t num_positions = lexical_cast<size_t>(line_string);

		// Prepare the data structures to populate
		aln_posn_vec posns( num_proteins, 0 );
		opt_score_vec scores;
		scores.reserve( num_positions );
		opt_aln_posn_vec_vec data( num_proteins );
		for (opt_aln_posn_vec &data_col : data) {
			data_col.reserve( num_positions );
		}
		// Loop over the main data section
		while (getline(arg_istream, line_string)) {
			// Check the line is of the correct length
			if (line_string.length() != num_chars_in_main_data_line) {
				BOOST_THROW_EXCEPTION(runtime_error_exception("Number of characters in main data line does not equal " + lexical_cast<string>(num_chars_in_main_data_line)));
			}

			// Grab the global details from start of this line
			const size_t      alignment_posn = lexical_cast<size_t>( trim_copy( line_string.substr(  0, 4 ))); // Column 1: Alignment Position (dddd)
//			const size_t num_entries_in_temp = lexical_cast<size_t>( trim_copy( line_string.substr(  5, 4 ))); // Column 2: No. of position selected for structural template (dddd)
			const size_t num_entries_in_posn = lexical_cast<size_t>( trim_copy( line_string.substr( 10, 4 ))); // Column 2: No. of position selected for structural template (dddd)

			if (alignment_posn != data.front().size() + 1) {
				BOOST_THROW_EXCEPTION(runtime_error_exception("Alignment position counter " + lexical_cast<string>(alignment_posn) + " does not match " + lexical_cast<string>(data.front().size() + 1)));
			}

			// Loop over the indices of the proteins
			size_t num_present_posns(0);
			for (size_t prot_ctr = 0; prot_ctr < num_proteins; ++prot_ctr) {
				// Prepare string and other data for this protein
				const size_t        prot_string_offset = CHARS_IN_MAIN_DATA_LINE_START + prot_ctr * CHARS_IN_MAIN_DATA_LINE_PROT;
				const string        prot_string        = line_string.substr( prot_string_offset, CHARS_IN_MAIN_DATA_LINE_PROT );
				opt_aln_posn_vec   &data_col           = data [ prot_ctr ];
				aln_posn_type       &posn              = posns[ prot_ctr ];

				// Grab the details for this protein
				const int              residue_num = lexical_cast<int>(          trim_copy( prot_string.substr(  1, 4 ))); // Column 4 (7,10 etc): Residue number in PDB file (ddddc) 4 digit number
				const char             insert_code =                                        prot_string.at(      5    )  ; //    + 1 character insert code
				const char              amino_acid =                                        prot_string.at(      7    )  ; // Column 5 (8,11 etc): Amino Acid Code (c)
//				const char               sec_struc =                                        prot_string.at(     10    )  ; // Column 6 (9,12 etc): Secondary Structure Assignment (c)

				// Find the residue in the list of this PDB's residue names
				const residue_name_vec &residues_names = residue_names_of_first_chains[ prot_ctr ];
				const residue_name      res_name       = make_residue_name_with_non_insert_char( residue_num, insert_code, ' ' );
				const opt_aln_posn      find_result    = search_for_residue_in_residue_names(
					posn,
					residues_names,
					amino_acid,
					res_name,
					arg_stderr
				);
				data_col.push_back( find_result ? opt_aln_posn( ( *find_result ) + 1 ) : opt_aln_posn( none ) );
				if ( find_result ) {
					posn = *find_result;
					++num_present_posns;
				}
			}
			if (num_present_posns != num_entries_in_posn) {
				BOOST_THROW_EXCEPTION(runtime_error_exception(
						"Number of positions for alignment_posn " + lexical_cast<string>(alignment_posn)
						+ " was " + lexical_cast<string>(num_present_posns)
						+ " not " + lexical_cast<string>(num_entries_in_posn)
				));
			}

			// Prepare the string for the global details at the end of this line
			const size_t end_string_offset = CHARS_IN_MAIN_DATA_LINE_START + num_proteins * CHARS_IN_MAIN_DATA_LINE_PROT;
			const string end_string = line_string.substr( end_string_offset, CHARS_IN_MAIN_DATA_LINE_END );

			// Grab the global details from start of this line
//			const size_t      cons_sec_struc =                                         end_string.at(      3    )  ; // Last Column-3: Consensus Secondary Structure Assignment (c)
//			const size_t       num_alpha_res = lexical_cast<size_t>( trim_copy(  end_string.substr(  5, 4 ))); // Last Column-2: No. of alpha residues at this position (dddd)
//			const size_t        num_beta_res = lexical_cast<size_t>( trim_copy(  end_string.substr( 10, 4 ))); // Last Column-1: No. of beta residues at this position (dddd)
			const size_t          cons_score = lexical_cast<size_t>( trim_copy(  end_string.substr( 16, 2 ))); // Last Column: Structural Conservation Score (dd)

			scores.push_back( numeric_cast<double>( cons_score ) );
//			// If there are multiple entries in this position then store the score
//			if (num_entries_in_posn > 1) {
////				cerr << "Adding score for " << alignment_posn-1 << endl;
//				scores.push_back(cons_score);
//			}
		}

		if ( num_positions != data.front().size() ) {
			BOOST_THROW_EXCEPTION(runtime_error_exception(
				"CORA legacy alignment number of positions was "
				+ lexical_cast<string>( data.front().size() )
				+ " not "
				+ lexical_cast<string>( num_positions )
			) );
		}

		alignment new_alignment = alignment_offset_1_factory( data );

		// Create a scores matrix and then empty any cells that are absent from the alignment
		opt_score_vec_vec all_scores( new_alignment.num_entries(), scores );
		for (size_t entry = 0; entry < new_alignment.num_entries(); ++entry) {
			for (size_t index = 0; index < new_alignment.length(); ++index) {
				if ( ! has_position_of_entry_of_index( new_alignment, entry, index ) ) {
					all_scores[ entry ][ index ] = none;
				}
			}
		}
		set_scores( new_alignment, all_scores);
		return new_alignment;
	}
	// Catch any I/O exceptions
	catch (const std::exception &ex) {
		const string error_message(string("Cannot read CORA legacy alignment file [") + ex.what() + "] ");
		perror(error_message.c_str());
		BOOST_THROW_EXCEPTION(runtime_error_exception(error_message));
	};
}

/// \brief Parse a FASTA format input into a vector of pairs of strings (one for id, one for sequence)
str_str_pair_vec cath::align::read_ids_and_sequences_from_fasta(istream &arg_istream ///< The istream from which to read the FASTA input for parsing
                                                                ) {
	arg_istream.exceptions( ios::badbit );
	str_str_pair_vec sequence_of_id;

	// Loop over the lines in the input
	string line_string;
	while ( getline( arg_istream, line_string ) ) {
		// If there are any non-printing characters, throw an exception
		if ( ! all( line_string, is_print() ) ) {
			BOOST_THROW_EXCEPTION(runtime_error_exception("Line in FASTA input contains non-printing characters"));
		}
		// If this line doesn't start with a '>' and it's the first line (nothing yet in sequence_of_id) then throw an exception
		if ( sequence_of_id.empty() && ! starts_with( line_string, ">" ) ) {
			BOOST_THROW_EXCEPTION(runtime_error_exception("Line in FASTA input expected to be header doesn't begin with '>'"));
		}

		// If this line starts with a >, then treat as a header
		if ( starts_with( line_string, ">" ) ) {
			// Remove the first character
			line_string = line_string.substr( 1 );

			// If the rest of the line is empty then throw an exception
			if ( line_string.empty() ) {
				BOOST_THROW_EXCEPTION(runtime_error_exception("Header line in FASTA doesn't have any characters after initial '>'"));
			}

			// Add a new entry at the back of sequence_of_id with this id and an empty string
			sequence_of_id.push_back( make_pair( line_string, string("") ) );
		}
		// Otherwise this is a line of sequence data
		else {
			// Remove all spaces from the string
			find_format_all( line_string, token_finder( is_space() ), empty_formatter( line_string ) );

			// If any of the (remaining) characters aren't alpha characters or '-'s then throw an exception
			if ( ! all( line_string, is_alpha() || is_any_of( "-" ) ) ) {
				BOOST_THROW_EXCEPTION(runtime_error_exception("Sequence line in FASTA input contains non-space characters that are neither letters nor '-'"));
			}

			// Convert the string to upper case and append it to the back of the sequence string for the most recent entry
			to_upper( line_string );
			assert( ! sequence_of_id.empty() );
			sequence_of_id.back().second += line_string;
		}
	}

	// Return the results of this parsing
	return sequence_of_id;
}

/// \brief Align a sequence against a corresponding pdb
///        (broadly handling residues missing in the sequence but not extra residues)
///
/// \returns A vector of opt_aln_posns corresponding to the letters of the sequence. Each is:
///           * none if the entry is a '-' character
///           * the index of the corresponding residue in arg_pdb otherwise
opt_aln_posn_vec cath::align::align_sequence_to_amino_acids(const string         &arg_sequence_string, ///< The raw sequence string (no headers; no whitespace) to be aligned
                                                            const amino_acid_vec &arg_amino_acids,     ///< The PDB against which the sequence is to be aligned
                                                            const string         &arg_name,            ///< The name of the entry to use in warnings / errors
                                                            ostream              &/*arg_stderr*/       ///< The ostream to which warnings should be output
                                                            ) {
	const size_t sequence_length  = arg_sequence_string.length();
	const size_t num_pdb_residues = arg_amino_acids.size();

	// Prepare the variables to be populated when looping through the sequence
	str_vec skipped_residues;
	opt_aln_posn_vec new_posns;
	new_posns.reserve( sequence_length );
	size_t pdb_ctr         = 0;

	// Loop along the sequence
	for (size_t seq_ctr = 0; seq_ctr < sequence_length; ++seq_ctr) {
		const char &sequence_char = arg_sequence_string[ seq_ctr ];

		// If this is a '-' character then add none to the back of new_posns
		if ( sequence_char == '-' ) {
			new_posns.push_back( none );
		}
		// Otherwise, it's an amino-acid letter
		else {
			// Continue searching pdb_ctr through the PDB until it matches this letter
			while ( sequence_char != arg_amino_acids[ pdb_ctr ].get_letter() ) {
				skipped_residues.push_back( lexical_cast<string>( pdb_ctr ) );

				// Increment pdb_ctr
				++pdb_ctr;

				// If pdb_ctr has overrun the end, then throw an exception
				if ( pdb_ctr >= num_pdb_residues ) {
					BOOST_THROW_EXCEPTION(runtime_error_exception(
						"When aligning a sequence to a PDB for "
						+ arg_name
						+ ", could not find match in PDB for residue  '"
						+ sequence_char
						+ "' at position "
						+ lexical_cast<string>( seq_ctr )
					));
				}
			}

			// Add the found PDB position to the back of new_positions
			new_posns.push_back( pdb_ctr );

			// Increment the pdb_ctr to the next residue
			++pdb_ctr;
		}
	}

	const size_t num_posns_skipped = skipped_residues.size();
	const size_t num_posns_found   = num_pdb_residues - num_posns_skipped;
	if ( num_posns_skipped > num_pdb_residues ) {
		BOOST_THROW_EXCEPTION(runtime_error_exception("The number of residues skipped exceeds the total number of residues"));
	}

	// If the number of residues found is an unacceptably low fraction of residues in the PDB,
	// then throw an exception
	const double fraction_pdb_residues_found = numeric_cast<double>( num_posns_found ) / numeric_cast<double>( num_pdb_residues );
	if ( fraction_pdb_residues_found < MIN_FRAC_OF_PDB_RESIDUES_IN_SEQ ) {
		BOOST_THROW_EXCEPTION(runtime_error_exception(
			"When aligning a sequence to a PDB for "
			+ arg_name
			+ ", only found matches for "
			+ lexical_cast<string>( num_posns_found )
			+ " of the "
			+ lexical_cast<string>( pdb_ctr )
			+ " residues in the PDB"
		));
	}
	// If not all residues were found, then output a warning about
	if ( num_posns_found < num_pdb_residues ) {
		BOOST_LOG_TRIVIAL( warning ) << "When aligning a sequence to a PDB for \""
		                             << arg_name
		                             << "\", "
		                             << lexical_cast<string>( num_pdb_residues - num_posns_found )
		                             << " of the PDB's "
		                             << lexical_cast<string>( num_pdb_residues )
		                             << " residues were missing in the sequence and had to be inserted (residue indices, using offset of 0 : "
		                             << join( skipped_residues, ", " )
		                             << ")";
	}

	// Return the result of this work
	return new_posns;
}

/// \brief Parse a FASTA format input into an alignment
///
/// This version of read_alignment_from_fasta() doesn't take names to find within the parsed IDS
/// so it is less safe than the other version.
///
/// \relates alignment
alignment cath::align::read_alignment_from_fasta_file(const path     &arg_fasta_file, ///< The file from which to read the FASTA input for parsing
                                                      const pdb_list &arg_pdbs,       ///< The PDBs that TODOCUMENT
                                                      ostream        &arg_stderr      ///< An ostream to which any warnings should be output (currently unused)
                                                      ) {
	return read_alignment_from_fasta_file( arg_fasta_file, arg_pdbs, str_vec( arg_pdbs.size() ), arg_stderr );
}

/// \brief Parse a FASTA format input into an alignment
///
/// \relates alignment
alignment cath::align::read_alignment_from_fasta_file(const path     &arg_fasta_file, ///< The file from which to read the FASTA input for parsing
                                                      const pdb_list &arg_pdbs,       ///< The PDBs that TODOCUMENT
                                                      const str_vec  &arg_names,      ///< A vector of names, each of which should be found within the corresponding sequence's ID
                                                      ostream        &arg_stderr      ///< An ostream to which any warnings should be output (currently unused)
                                                      ) {
	// Construct an alignment from the FASTA alignment file
	ifstream my_aln_stream;
	open_ifstream( my_aln_stream, arg_fasta_file );
	const alignment new_alignment = read_alignment_from_fasta( my_aln_stream, get_amino_acid_lists( arg_pdbs ), arg_names, arg_stderr );
	my_aln_stream.close();
	return new_alignment;
}

/// \brief Parse a FASTA format input into an alignment
///
/// This version of read_alignment_from_fasta() doesn't take names to find within the parsed IDS
/// so it is less safe than the other version.
///
/// \relates alignment
alignment cath::align::read_alignment_from_fasta(istream        &arg_istream, ///< The istream from which to read the FASTA input for parsing
                                                 const pdb_list &arg_pdbs,    ///< The PDBs that TODOCUMENT
                                                 ostream        &arg_stderr   ///< An ostream to which any warnings should be output (currently unused)
                                                 ) {
	return read_alignment_from_fasta( arg_istream, get_amino_acid_lists( arg_pdbs ), str_vec( arg_pdbs.size() ), arg_stderr );
}

/// \brief Parse a FASTA format input into an alignment
///
/// This version of read_alignment_from_fasta() doesn't take names to find within the parsed IDS
/// so it is less safe than the other version.
///
/// \relates alignment
alignment cath::align::read_alignment_from_fasta_file(const path         &arg_fasta_file, ///< The file from which to read the FASTA input for parsing
                                                      const protein_list &arg_proteins,   ///< TODOCUMENT
                                                      ostream            &arg_stderr      ///< An ostream to which any warnings should be output (currently unused)
                                                      ) {
	return read_alignment_from_fasta_file( arg_fasta_file, arg_proteins, str_vec( arg_proteins.size() ), arg_stderr );
}

/// \brief Parse a FASTA format input into an alignment
///
/// \relates alignment
alignment cath::align::read_alignment_from_fasta_file(const path         &arg_fasta_file, ///< The file from which to read the FASTA input for parsing
                                                      const protein_list &arg_proteins,   ///< TODOCUMENT
                                                      const str_vec      &arg_names,      ///< A vector of names, each of which should be found within the corresponding sequence's ID
                                                      ostream            &arg_stderr      ///< An ostream to which any warnings should be output (currently unused)
                                                      ) {
	// Construct an alignment from the FASTA alignment file
	ifstream my_aln_stream;
	open_ifstream( my_aln_stream, arg_fasta_file );
	const alignment new_alignment = read_alignment_from_fasta( my_aln_stream, get_amino_acid_lists( arg_proteins ), arg_names, arg_stderr );
	my_aln_stream.close();
	return new_alignment;
}

/// \brief Parse a FASTA format input into an alignment
///
/// This version of read_alignment_from_fasta() doesn't take names to find within the parsed IDS
/// so it is less safe than the other version.
///
/// \relates alignment
alignment cath::align::read_alignment_from_fasta(istream            &arg_istream,  ///< The istream from which to read the FASTA input for parsing
                                                 const protein_list &arg_proteins, ///< TODOCUMENT
                                                 ostream            &arg_stderr    ///< An ostream to which any warnings should be output (currently unused)
                                                 ) {
	return read_alignment_from_fasta( arg_istream, get_amino_acid_lists( arg_proteins ), str_vec( arg_proteins.size() ), arg_stderr );
}

/// \brief Parse a FASTA format input into an alignment
///
/// At present, each sequences must contain all of the residues of the corresponding PDB
/// (because the index in the PDB is required in the alignment).
///
/// !!Case insensitive!!
///
/// \todo !URGENT! Test what this does when given structures with incomplete residues near the start.
///       It looks like it gives indices in the PDB, rather than in the protein (which only
///       contains backbone-complete residues). This is a serious issue!
///
/// \todo Generalise this so that it's possible to read alignments against a pdb_list or a protein_list
///
/// The code will attempt to handle missing residues with a warning if there are a small number.
/// It will fail if the percentage is too low.
///
/// \relates alignment
alignment cath::align::read_alignment_from_fasta(istream                  &arg_istream,          ///< The istream from which to read the FASTA input for parsing
                                                 const amino_acid_vec_vec &arg_amino_acid_lists, ///< TODOCUMENT
                                                 const str_vec            &arg_names,            ///< A vector of names, each of which should be found within the corresponding sequence's ID
                                                 ostream                  &arg_stderr            ///< An ostream to which any warnings should be output (currently unused)
                                                 ) {
		if ( arg_amino_acid_lists.empty() ) {
			BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot load a FASTA alignment with 0 PDB entries"));
		}
		const size_t num_entries = arg_amino_acid_lists.size();
		if ( arg_names.size() != num_entries ) {
			BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot load a FASTA alignment with a different number of names and PDB entries"));
		}

		arg_istream.exceptions( ios::badbit );

		try {
			const str_str_pair_vec sequence_of_id = read_ids_and_sequences_from_fasta( arg_istream );
			const size_t num_sequences = sequence_of_id.size();
			if ( num_entries != num_sequences ) {
				BOOST_THROW_EXCEPTION(runtime_error_exception(
					"Number of sequences parsed from FASTA ("
					+ lexical_cast<string>( num_sequences )
					+ ") doesn't match the number of PDBs/names ("
					+ lexical_cast<string>( num_entries   )
					+ ")"
				));
			}

			const size_t sequence_length = sequence_of_id.front().second.length();

			opt_aln_posn_vec_vec positions;
			positions.reserve( num_entries );
			for (size_t entry_ctr = 0; entry_ctr < num_entries; ++entry_ctr) {
				const amino_acid_vec &amino_acids     = arg_amino_acid_lists      [ entry_ctr ];
				const string         &name            = arg_names     [ entry_ctr ];
				const str_str_pair   &id_and_sequence = sequence_of_id[ entry_ctr ];
				const string         &id              = id_and_sequence.first;
				const string         &sequence        = id_and_sequence.second;

				if ( sequence.length() != sequence_length ) {
					BOOST_THROW_EXCEPTION(runtime_error_exception(
						"When attempting to parse entry number "
						+ lexical_cast<string>( entry_ctr + 1     )
						+ " of FASTA alignment, the length of the sequence ("
						+ lexical_cast<string>( sequence.length() )
						+ ") does not match the length of the first sequence ("
						+ lexical_cast<string>( sequence_length   )
						+ ")"
					));
				}

				if ( ! icontains( id, name ) ) {
					BOOST_THROW_EXCEPTION(runtime_error_exception(
						"When attempting to parse entry number "
						+ lexical_cast<string>( entry_ctr + 1 )
						+ " of FASTA alignment, name \""
						+ name
						+ "\" could not be found in a case-insensitive search within FASTA header ID \""
						+ id
						+ "\""
					));
				}

				positions.push_back( align_sequence_to_amino_acids( sequence, amino_acids, name, arg_stderr ) );
			}

			return alignment( positions );
		}
		// Catch any I/O exceptions
		catch (const std::exception &ex) {
			const string error_message(string("Cannot read FASTA legacy alignment file [") + ex.what() + "] ");
			perror(error_message.c_str());
			BOOST_THROW_EXCEPTION(runtime_error_exception(error_message));
		};
	}

/// \brief Convenience function for read_alignment_from_cath_ssap_legacy_format() to use
opt_aln_posn cath::align::search_for_residue_in_residue_names(const size_t           &arg_pos,           ///< TODOCUMENT
                                                              const residue_name_vec &arg_residue_names, ///< TODOCUMENT
                                                              const char             &arg_amino_acid,    ///< TODOCUMENT
                                                              const residue_name     &arg_residue_name,  ///< TODOCUMENT
                                                              ostream                &/*arg_stderr*/     ///< TODOCUMENT
                                                              ) {
	const bool residue_is_present = ( arg_amino_acid != '0' );
	if ( residue_is_present ) {
		if (arg_pos >= arg_residue_names.size()) {
			BOOST_THROW_EXCEPTION(runtime_error_exception("Counter has gone past end of list of residues whilst loading alignment"));
		}
		const auto res_itr = find(
			common::cbegin( arg_residue_names ) + numeric_cast<ptrdiff_t>( arg_pos ),
			common::cend  ( arg_residue_names ),
			arg_residue_name
		);
		if ( res_itr == common::cend( arg_residue_names ) ) {
			cerr << "Residue names being searched:\n\n";
			for (const residue_name &the_res_name : arg_residue_names) {
				cerr << " " << the_res_name;
			}
			cerr << "\n" << endl;
			BOOST_THROW_EXCEPTION(runtime_error_exception(
				"Unable to find residue "
				+ lexical_cast<string>( arg_residue_name )
				+ " from alignment in list of residues, starting from position "
				+ lexical_cast<string>( arg_pos )
			));
		}

		const size_t new_pos = numeric_cast<size_t>( distance( common::cbegin( arg_residue_names ), res_itr ) );
		if ( new_pos != arg_pos + 1 && ( new_pos != 0 || arg_pos != 0 ) ) {
			const size_t jump = new_pos - (arg_pos + 1);
			BOOST_LOG_TRIVIAL( warning ) << "Missing some residues whilst loading alignment: jumped "
			                             << jump
			                             << " position(s) from residue "
			                             << arg_residue_names[arg_pos]
			                             << " (in position "
			                             << arg_pos
			                             << ") to residue "
			                             << arg_residue_names[new_pos]
			                             << " (in position "
			                             << new_pos
			                             << ")";
		}
		return opt_aln_posn( new_pos );
	}
	return opt_aln_posn( none );
}


/// \brief Prints residue numbers in alignment
///
/// \relates alignment
void cath::align::write_alignment_as_cath_ssap_legacy_format(const path      &arg_output_file, ///< TODOCUMENT
                                                             const alignment &arg_alignment,   ///< TODOCUMENT
                                                             const protein   &seq_a,           ///< TODOCUMENT
                                                             const protein   &seq_b            ///< TODOCUMENT
                                                             ) {
	ofstream aln_out_stream;
	open_ofstream( aln_out_stream, arg_output_file );

	// Try here to catch any I/O exceptions
	try {
		output_alignment_to_cath_ssap_legacy_format(aln_out_stream, arg_alignment, seq_a, seq_b);

		// Close the file
		aln_out_stream.close();
	}
	// Catch and immediately rethrow any boost::exceptions
	// (so that it won't get caught in the next block if it's a std::exception)
	catch (const boost::exception &ex) {
		throw;
	}
	// Catch any I/O exceptions
	catch (const std::exception &ex) {
		const string error_message("Cannot output alignment to file \"" + arg_output_file.string() + "\" [" + ex.what() + "] ");
		perror(error_message.c_str());
		BOOST_THROW_EXCEPTION(runtime_error_exception(error_message));
	};
}

/// \brief Outputs an alignment in the legacy CATH format for SSAP
///
/// \relates alignment
ostream & cath::align::output_alignment_to_cath_ssap_legacy_format(ostream         &arg_os,            ///< The ostream to which the data should be output
                                                                   const alignment &arg_alignment, ///< The alignment to output
                                                                   const protein   &seq_a,         ///< The first protein in the alignment
                                                                   const protein   &seq_b          ///< The second protein in the alignment
                                                                   ) {
	if (!arg_alignment.is_scored()) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot output legacy format for alignment that has not been scored"));
	}

	const int NO_SCORE = 0;

	for (size_t alignment_ctr = 0; alignment_ctr < arg_alignment.length(); ++alignment_ctr) {
		const int has_posn_a = has_a_position_of_index( arg_alignment, alignment_ctr );
		const int has_posn_b = has_b_position_of_index( arg_alignment, alignment_ctr );

		// Grab the score if both sides of the alignment are present or set it to NO_SCORE (0) otherwise
		const int score = (has_posn_a && has_posn_b)
		                  ? numeric_cast<int>( get_mean_score_of_index( arg_alignment, alignment_ctr ) )
		                  : NO_SCORE;

		// Then convert to a %3d string
		ostringstream score_ss;
		score_ss << setw(3) << score; // ****** %3d ******
		const string score_string = score_ss.str();

		// Grab whichever of the two residues are present in the alignment
		const residue &residue_a = has_posn_a
		                           ? get_residue_ref_of_index__offset_1( seq_a, get_a_offset_1_position_of_index(arg_alignment, alignment_ctr) )
		                           : residue::NULL_RESIDUE;
		const residue &residue_b = has_posn_b
		                           ? get_residue_ref_of_index__offset_1( seq_b, get_b_offset_1_position_of_index(arg_alignment, alignment_ctr) )
		                           : residue::NULL_RESIDUE;

		// Build up a string for this line in the alignment
		const string left_side_string  = has_posn_a
		                                 ? ssap_legacy_alignment_left_side_string( residue_a )
		                                 : ssap_legacy_alignment_left_side_gap_string();
		const string right_side_string = has_posn_b
		                                 ? ssap_legacy_alignment_right_side_string( residue_b )
		                                 : ssap_legacy_alignment_right_side_gap_string();
		arg_os << left_side_string << "  " << score_string << "  " + right_side_string << "\n";
	}

	return arg_os;
}

/// \brief Output an alignment in FASTA format
///
/// \relates alignment
void cath::align::write_alignment_as_fasta_alignment(const path         &arg_output_file, ///< TODOCUMENT
                                                     const alignment    &arg_alignment,   ///< TODOCUMENT
                                                     const protein_list &arg_proteins     ///< TODOCUMENT
                                                     ) {
	ofstream out_stream;
	open_ofstream(out_stream, arg_output_file);
	write_alignment_as_fasta_alignment( out_stream, arg_alignment, arg_proteins );
	out_stream.close();
}

/// \brief Outputs an alignment in FASTA format
///
/// \relates alignment
ostream & cath::align::write_alignment_as_fasta_alignment(ostream            &arg_os,        ///< TODOCUMENT
                                                          const alignment    &arg_alignment, ///< TODOCUMENT
                                                          const protein_list &arg_proteins   ///< TODOCUMENT
                                                          ) {
	// Grab the number of entries in the alignment and sanity-check that the
	// number of names matches
	using size_type = alignment::size_type;
	const size_type num_entries = arg_alignment.num_entries();
	const size_type length      = arg_alignment.length();
	if ( num_entries != arg_proteins.size() ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception(
			"Unable to output alignment in FASTA format because the number of proteins ("
			+ lexical_cast<string>( arg_proteins.size() )
			+ ") doesn't match the number of entries in the alignment ("
			+ lexical_cast<string>( num_entries )
			+ ")"
		));
	}

	// Loop over the entries in the alignment
	for (size_t entry_ctr = 0; entry_ctr < num_entries; ++entry_ctr) {
		const protein &the_protein = arg_proteins[ entry_ctr ];

		// Output the title (ie name of this protein)
		arg_os << ">" << the_protein.get_title() << "\n";

		// Loop over the indices of the alignment
		for (size_t aln_index = 0; aln_index < length; ++aln_index) {
			// If this entry has no position at this index, output a '-' character
			// otherwise, use the position at this index to output the amino acid letter
			const opt_aln_posn position = arg_alignment.position_of_entry_of_index( entry_ctr, aln_index );
			arg_os << ( position ? get_amino_acid_letter_of_index( the_protein, *position )
			                     : '-' );
		}
		arg_os << "\n";
	}
	arg_os << flush;

	// Return a non-const reference to the ostream
	// (to make this interface a bit like a standard insertion operator)
	return arg_os;
}

/// \brief Output an alignment in FASTA format
///
/// \relates alignment
ostream & cath::align::write_alignment_as_fasta_alignment(ostream         &arg_os,        ///< TODOCUMENT
                                                          const alignment &arg_alignment, ///< TODOCUMENT
                                                          const pdb_list  &arg_pdbs,      ///< TODOCUMENT
                                                          const str_vec   &arg_names      ///< TODOCUMENT
                                                          ) {
	return write_alignment_as_fasta_alignment(
		arg_os,
		arg_alignment,
		build_protein_list_of_pdb_list_and_names( arg_pdbs, arg_names )
	);
}

/// \brief Output an alignment in FASTA format
///
/// \relates alignment
string cath::align::alignment_as_fasta_string(const alignment    &arg_alignment, ///< TODOCUMENT
                                              const protein_list &arg_proteins   ///< TODOCUMENT
                                              ) {
	ostringstream the_out_ss;
	write_alignment_as_fasta_alignment( the_out_ss, arg_alignment, arg_proteins);
	return the_out_ss.str();
}

/// \brief Output an alignment in FASTA format
///
/// \relates alignment
string cath::align::alignment_as_fasta_string(const alignment &arg_alignment, ///< TODOCUMENT
                                              const pdb_list  &arg_pdbs,      ///< TODOCUMENT
                                              const str_vec   &arg_names      ///< TODOCUMENT
                                              ) {
	ostringstream the_out_ss;
	write_alignment_as_fasta_alignment( the_out_ss, arg_alignment, arg_pdbs, arg_names);
	return the_out_ss.str();
}
