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
PklParser.h

This file contains the definitions for PKL parser.
*/

#ifndef PklParser_h
#define PklParser_h

#include "DatasetParser.h"


//************************************************************************
// Class for handling the pickle parser (*.pkl)
//************************************************************************
class PklParser : public DatasetParser
{
    Q_OBJECT
    //************************************************************************
    // functions
    //************************************************************************
    public:
        PklParser();

    public slots:
        void parseDataset();

        void parseDatasetSingleModulation();

    signals:
        void parseFinished();
};

#endif // PklParser_h
