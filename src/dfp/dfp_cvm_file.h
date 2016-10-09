/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *  
 *  Copyright (c) 2016 potuz@potuz.net
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DFP_CVM_FILE_INCLUDED
#define DFP_CVM_FILE_INCLUDED
#include <string>
#include <memory>
#include <ctime>
/*! \file dfp_cvm_file.h */
/// \defgroup Enumerations
/// Public enumeration types
namespace Dfp {
  /// \ingroup Enumerations
  /// \headerfile dfp_cvm_file.h "dfp_cvm_file.h"
  /// Cvm File Types
  enum CvmFileType { 
    DFP_CVM_FILE_FALSE,
    DFP_CVM_FILE_FCA,
    DFP_CVM_FILE_FRE,
    DFP_CVM_FILE_ITR,
    DFP_CVM_FILE_DFP, 
    DFP_CVM_FILE_ITR_AND_DFP, 
    DFP_CVM_FILE_DFP_MC, 
    DFP_CVM_FILE_DFP_LS, 
    DFP_CVM_FILE_DFP_IFRS, 
    DFP_CVM_FILE_IPE, 
    DFP_CVM_FILE_ITR_DFP_IAN
  };
  class Database;
  /// \brief The zipfile from cvm. 
  ///
  /// a CvmFileType holds the basic info about a zipfile downloaded from CVM. 
  class CvmFile {

    int cvm;
    CvmFileType type;  
    std::tm exercise;
    int revision;
    std::string filename;
    
    void unzip ( const std::string& path );

    public:
    CvmFile (const std::string& name); 
    ~CvmFile () {};

    /// Imports all accounts in the file to the Database. 
    /// It imports all accounts. The names for the DMPL are included in the 
    /// "comments" field. Values are stored in thousands except for the
    /// following columns:
    ///   - 1.90.??: The price is the price of a hundred stocks. 
    ///   - 3.99.??: These accounts are ignored since would hold typically 
    ///   float numbers and we store anyway enough information to compute them
    /// 
    /// @param[in] conn The Dfp::Database with an open connection. The file
    ///  holds a reference to this Database even after the Dfp::Database 
    ///  object has been deleted. 
    void import ( const std::shared_ptr<Dfp::Database> conn );

    int get_cvm () { return cvm; };
    CvmFileType get_file_type () { return type; };
    struct std::tm get_exercise () { return exercise; };
    int get_revision () { return revision; };
    std::string get_filename () { return filename; };
  };
}
#endif 
