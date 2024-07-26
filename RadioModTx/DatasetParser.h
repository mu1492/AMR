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
DatasetParser.h

This file contains the definitions for dataset parser.
*/

#ifndef DatasetParser_h
#define DatasetParser_h

#include "Dataset.h"

#include <string>

#include <QObject>


//************************************************************************
// Class for handling the dataset parser
//************************************************************************
class DatasetParser : public QObject
{
    Q_OBJECT

    //************************************************************************
    // functions
    //************************************************************************
    public:
        DatasetParser();

        Dataset::ModulationSnrSignalDataMap getMap
            (
            bool& aStatus               //!< status
            ) const;

        std::vector<Modulation::ModulationName> getUniqueModVec() const;

        std::vector<int> getUniqueSnrVec() const;

        virtual void parseDataset() = 0;

        virtual void parseDatasetSingleModulation() = 0;

        void removeDuplicates
            (
            std::vector<int>& aVector   //!< vector
            );

        void removeDuplicates
            (
            std::vector<Modulation::ModulationName>& aVector   //!< vector
            );

        void setFile
            (
            std::string aFileName       //!< input filename
            );

        void setSingleModulation
            (
            Modulation::ModulationName aModulation  //!< modulation
            );

    //************************************************************************
    // variables
    //************************************************************************
    protected:
        std::string                             mFileName;      //!< filename
        bool                                    mStatus;        //!< parse status

        std::vector<Modulation::ModulationName> mUniqueModVec;  //!< vector with unique modulations
        std::vector<int>                        mUniqueSnrVec;  //!< vector with unique SNRs
        Modulation::ModulationName              mSingleModulation;  //!< selected modulation
        Dataset::ModulationSnrSignalDataMap     mMap;           //!< map with data signals for modulation-SNR combinations
        double                                  mMaxVal;        //!< maximum value
};

#endif // DatasetParser_h
