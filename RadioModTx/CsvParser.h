///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2024 Mihai Ursu                                                 //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
// (at your option) any later version.                                           //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

/*
CsvParser.h

This file contains the definitions for CSV parser.
*/

#ifndef CsvParser_h
#define CsvParser_h

#include "DatasetParser.h"

#include <map>
#include <vector>


//************************************************************************
// Class for handling the Comma Separated Values (CSV) parser (*.csv)
//************************************************************************
class CsvParser : public DatasetParser
{
    Q_OBJECT
    //************************************************************************
    // constants and types
    //************************************************************************
    private:
        static const std::map<int, Modulation::ModulationName> MODULATION_MAPPING;

        static const std::vector<int> MODULATION_SERIES;  //!< sequence of modulations


    //************************************************************************
    // functions
    //************************************************************************
    public:
        CsvParser();

    public slots:
        void parseDataset();

        void parseDatasetSingleModulation();

    signals:
        void parseFinished();

    private:
        Dataset::IQPoint getPoint
            (
            std::string aString     //!< string with complex number
            ) const;
};

#endif // CsvParser_h
