/*  This file is part of denet. 
 *  denet is a tool to study Bovespa-listed companies. 
 *
 *  Copyright (c) 2016 - Potuz Vader potuz@potuz.net
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
#ifndef DFP_EXCEPTION_INCLUDED
#define DFP_EXCEPTION_INCLUDED
#include <stdexcept>
/// \file 
namespace Dfp {
  /// \ingroup Enumerations
  /// Possible exceptions encountered
  enum ExceptionType {
    EXCEPTION_NO_ACCT = 1,
    EXCEPTION_NO_INTERNET,
    EXCEPTION_NO_EXERCISE
  };
  class Exception : public std::runtime_error {
    private: 
      ExceptionType codigo_err;
    public:
    ExceptionType getErrorCode () { return codigo_err; };
    Exception ( const char * what_arg, ExceptionType codigo_err_ ) : 
      runtime_error ( what_arg ), codigo_err (codigo_err_) { }; 
  }; 
}
#endif
